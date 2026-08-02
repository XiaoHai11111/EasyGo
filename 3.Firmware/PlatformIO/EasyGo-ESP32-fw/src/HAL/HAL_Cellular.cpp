#include "HAL/HAL.h"
#include "App/Accounts/Account_Master.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace
{
HardwareSerial modem(1);

struct WorkerRequest
{
    AccountSystem::Network_Command_Code_t command;
    uint32_t requestId;
    double latitude;
    double longitude;
    char phone[24];
    char message[160];
};

QueueHandle_t requestQueue = nullptr;
QueueHandle_t resultQueue = nullptr;
AccountSystem::Network_Info_t currentState = {};
bool initialPublishPending = true;

void CopyText(char* target, size_t size, const char* text)
{
    if (!target || size == 0) return;
    strncpy(target, text ? text : "", size - 1);
    target[size - 1] = '\0';
}

String ReadResponse(uint32_t timeoutMs)
{
    String response;
    const uint32_t started = millis();
    while (millis() - started < timeoutMs)
    {
        while (modem.available() > 0) response += static_cast<char>(modem.read());
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return response;
}

String SendRaw(const String& command, uint32_t timeoutMs)
{
    while (modem.available() > 0) modem.read();
    modem.println(command);
    return ReadResponse(timeoutMs);
}

bool Send(const String& command, const char* expected, uint32_t timeoutMs)
{
    return SendRaw(command, timeoutMs).indexOf(expected) >= 0;
}

int16_t ParseSignalDbm(const String& response)
{
    const int marker = response.indexOf("+CSQ:");
    if (marker < 0) return -127;
    int rssi = 99;
    if (sscanf(response.c_str() + marker, "+CSQ: %d", &rssi) != 1 || rssi < 0 || rssi > 31)
        return -127;
    return static_cast<int16_t>(-113 + 2 * rssi);
}

void ParseNetworkClock(const String& response, AccountSystem::Network_Info_t& status)
{
    const int marker = response.indexOf("+CCLK:");
    if (marker < 0) return;
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    if (sscanf(response.c_str() + marker,
               "+CCLK: \"%d/%d/%d,%d:%d:%d",
               &year, &month, &day, &hour, &minute, &second) != 6) return;
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) return;
    status.timeValid = true;
    status.localHour = static_cast<uint8_t>(hour);
    status.localMinute = static_cast<uint8_t>(minute);
    status.localSecond = static_cast<uint8_t>(second);
    status.clockUpdatedAtMs = millis();
}

void FillNearbyResult(AccountSystem::Network_Info_t& result, const String& json)
{
    DynamicJsonDocument document(1536);
    const DeserializationError error = deserializeJson(document, json);
    if (error)
    {
        result.errorCode = -31;
        CopyText(result.detail, sizeof(result.detail), "厕所查询 JSON 解析失败");
        return;
    }

    result.distanceMeters = document["distance_m"] | -1;
    result.targetLatitude = document["target_lat"] | 0.0;
    result.targetLongitude = document["target_lng"] | 0.0;
    CopyText(result.targetName, sizeof(result.targetName), document["name"] | "公共厕所");
    CopyText(result.detail, sizeof(result.detail), document["address"] | "");
    result.success = result.distanceMeters >= 0 && result.targetLatitude != 0.0 && result.targetLongitude != 0.0;
    if (!result.success) result.errorCode = -32;
}

void QueryNearbyWithWifi(const WorkerRequest& request, AccountSystem::Network_Info_t& result)
{
    HTTPClient http;
    String url = String("http://") + CONFIG_CELLULAR_SERVER + ":" + CONFIG_CELLULAR_SERVER_PORT +
        "/api/nearby?lat=" + String(request.latitude, 6) + "&lng=" + String(request.longitude, 6);
    http.setConnectTimeout(7000);
    http.setTimeout(12000);
    if (!http.begin(url))
    {
        result.errorCode = -20;
        CopyText(result.detail, sizeof(result.detail), "Wi-Fi HTTP 初始化失败");
        return;
    }
    const int status = http.GET();
    if (status == HTTP_CODE_OK) FillNearbyResult(result, http.getString());
    else
    {
        result.errorCode = static_cast<int16_t>(status);
        CopyText(result.detail, sizeof(result.detail), "Wi-Fi 厕所查询失败");
    }
    http.end();
}

void QueryNearbyWithModem(const WorkerRequest& request, AccountSystem::Network_Info_t& result)
{
    if (!Send("AT", "OK", 1500) || !Send("AT+CPIN?", "READY", 2500))
    {
        result.errorCode = -10;
        CopyText(result.detail, sizeof(result.detail), "4G 模块或 SIM 未就绪");
        return;
    }

    Send(String("AT+CGDCONT=1,\"IP\",\"") + CONFIG_CELLULAR_APN + "\"", "OK", 2500);
    if (!Send("AT+CGACT=1,1", "OK", 10000))
    {
        result.errorCode = -11;
        CopyText(result.detail, sizeof(result.detail), "4G 数据承载激活失败");
        return;
    }

    Send("AT+HTTPTERM", "OK", 1000);
    if (!Send("AT+HTTPINIT", "OK", 3000))
    {
        result.errorCode = -12;
        return;
    }
    const String url = String("AT+HTTPPARA=\"URL\",\"http://") + CONFIG_CELLULAR_SERVER + ":" +
        CONFIG_CELLULAR_SERVER_PORT + "/api/nearby?lat=" + String(request.latitude, 6) +
        "&lng=" + String(request.longitude, 6) + "\"";
    if (!Send(url, "OK", 3000) || !Send("AT+HTTPACTION=0", "+HTTPACTION:", 20000))
    {
        result.errorCode = -13;
        Send("AT+HTTPTERM", "OK", 1000);
        return;
    }

    String raw = SendRaw("AT+HTTPREAD=0,2048", 10000);
    Send("AT+HTTPTERM", "OK", 1000);
    const int start = raw.indexOf('{');
    if (start < 0)
    {
        result.errorCode = -14;
        CopyText(result.detail, sizeof(result.detail), "4G 查询响应没有 JSON");
        return;
    }
    FillNearbyResult(result, raw.substring(start));
}

bool ParseLbsResponse(const String& response, AccountSystem::Network_Info_t& result)
{
    const int marker = response.indexOf("+CLBS:");
    if (marker < 0)
    {
        result.errorCode = -53;
        CopyText(result.detail, sizeof(result.detail), "4G LBS 响应缺少 +CLBS");
        return false;
    }

    int lineEnd = response.indexOf('\r', marker);
    if (lineEnd < 0) lineEnd = response.indexOf('\n', marker);
    if (lineEnd < 0) lineEnd = response.length();
    String body = response.substring(marker + 6, lineEnd);
    body.trim();

    char buffer[128] = {};
    body.toCharArray(buffer, sizeof(buffer));
    char* fields[6] = {};
    size_t fieldCount = 0;
    char* save = nullptr;
    for (char* token = strtok_r(buffer, ",", &save);
         token && fieldCount < sizeof(fields) / sizeof(fields[0]);
         token = strtok_r(nullptr, ",", &save))
    {
        while (*token == ' ') ++token;
        fields[fieldCount++] = token;
    }
    if (fieldCount < 4 || atoi(fields[0]) != 0)
    {
        result.errorCode = -54;
        CopyText(result.detail, sizeof(result.detail), "4G LBS 服务未返回有效位置");
        return false;
    }

    const double first = strtod(fields[1], nullptr);
    const double second = strtod(fields[2], nullptr);
    const double accuracy = strtod(fields[3], nullptr);
    double latitude = first;
    double longitude = second;
    if (fabs(first) > 90.0 && fabs(second) <= 90.0)
    {
        longitude = first;
        latitude = second;
    }
    if (!isfinite(latitude) || !isfinite(longitude) || !isfinite(accuracy) ||
        fabs(latitude) > 90.0 || fabs(longitude) > 180.0 ||
        (latitude == 0.0 && longitude == 0.0) || accuracy <= 0.0 || accuracy > 100000.0)
    {
        result.errorCode = -55;
        CopyText(result.detail, sizeof(result.detail), "4G LBS 坐标或精度越界");
        return false;
    }

    result.targetLatitude = latitude;
    result.targetLongitude = longitude;
    result.locationAccuracyMeters = static_cast<float>(accuracy);
    result.success = true;
    result.errorCode = 0;
    CopyText(result.targetName, sizeof(result.targetName), "4G LBS");
    CopyText(result.detail, sizeof(result.detail), "4G 基站定位成功");
    return true;
}

void QueryLbsWithModem(AccountSystem::Network_Info_t& result)
{
    if (!Send("AT", "OK", 1500) || !Send("AT+CPIN?", "READY", 2500))
    {
        result.errorCode = -50;
        CopyText(result.detail, sizeof(result.detail), "4G 模块或 SIM 未就绪");
        return;
    }

    String registration = SendRaw("AT+CEREG?", 2500);
    if (registration.indexOf(",1") < 0 && registration.indexOf(",5") < 0)
        registration = SendRaw("AT+CREG?", 2500);
    if (registration.indexOf(",1") < 0 && registration.indexOf(",5") < 0)
        registration = SendRaw("AT+CGREG?", 2500);
    if (registration.indexOf(",1") < 0 && registration.indexOf(",5") < 0)
    {
        result.errorCode = -51;
        CopyText(result.detail, sizeof(result.detail), "4G 网络尚未注册，无法 LBS 定位");
        return;
    }

    Send(String("AT+CGDCONT=1,\"IP\",\"") + CONFIG_CELLULAR_APN + "\"", "OK", 2500);
    const String activation = SendRaw("AT+CGACT=1,1", 10000);
    if (activation.indexOf("OK") < 0)
    {
        const String state = SendRaw("AT+CGACT?", 2500);
        if (state.indexOf("+CGACT: 1,1") < 0)
        {
            result.errorCode = -52;
            CopyText(result.detail, sizeof(result.detail), "4G 数据承载未激活");
            return;
        }
    }

    ParseLbsResponse(SendRaw("AT+CLBS=1", 12000), result);
    Serial.printf("4G LBS: %s accuracy=%.0f m error=%d\r\n",
        result.success ? "valid" : "unavailable",
        result.locationAccuracyMeters,
        result.errorCode);
}

void SendSms(const WorkerRequest& request, AccountSystem::Network_Info_t& result)
{
    if (!Send("AT", "OK", 1500) || !Send("AT+CMGF=1", "OK", 2000))
    {
        result.errorCode = -40;
        return;
    }
    while (modem.available() > 0) modem.read();
    modem.print("AT+CMGS=\"");
    modem.print(request.phone);
    modem.println("\"");
    if (ReadResponse(3000).indexOf('>') < 0)
    {
        result.errorCode = -41;
        return;
    }
    modem.print(request.message);
    modem.write(0x1A);
    const String response = ReadResponse(20000);
    result.success = response.indexOf("+CMGS:") >= 0 && response.indexOf("OK") >= 0;
    result.errorCode = result.success ? 0 : -42;
    CopyText(result.detail, sizeof(result.detail),
        result.success ? "4G 模块已受理短信" : "4G 模块未受理短信");
}

void Worker(void*)
{
    uint32_t lastProbe = millis() - 15000UL;
    for (;;)
    {
        WorkerRequest request = {};
        if (xQueueReceive(requestQueue, &request, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            AccountSystem::Network_Info_t result = currentState;
            result.requestId = request.requestId;
            result.busy = false;
            result.success = false;
            result.errorCode = 0;
            result.locationAccuracyMeters = 0.0f;
            result.targetLatitude = 0.0;
            result.targetLongitude = 0.0;
            if (request.command == AccountSystem::NETWORK_CMD_SEND_SMS)
                result.event = AccountSystem::NETWORK_EVENT_SMS_RESULT;
            else if (request.command == AccountSystem::NETWORK_CMD_QUERY_LBS)
                result.event = AccountSystem::NETWORK_EVENT_LBS_RESULT;
            else
                result.event = AccountSystem::NETWORK_EVENT_NEARBY_RESULT;
            if (request.command == AccountSystem::NETWORK_CMD_SEND_SMS) SendSms(request, result);
            else if (request.command == AccountSystem::NETWORK_CMD_QUERY_LBS) QueryLbsWithModem(result);
            else if (WiFi.status() == WL_CONNECTED) QueryNearbyWithWifi(request, result);
            else QueryNearbyWithModem(request, result);
            xQueueSend(resultQueue, &result, portMAX_DELAY);
        }

        if (millis() - lastProbe >= 15000)
        {
            lastProbe = millis();
            AccountSystem::Network_Info_t status = currentState;
            status.event = AccountSystem::NETWORK_EVENT_STATUS;
            status.busy = false;
            status.success = true;
            if (WiFi.status() == WL_CONNECTED)
            {
                status.link = AccountSystem::NETWORK_LINK_WIFI;
                status.registered = true;
                status.signalDbm = static_cast<int16_t>(WiFi.RSSI());
                CopyText(status.operatorName, sizeof(status.operatorName), "Wi-Fi");
            }
            else
            {
                String response = SendRaw("AT+CEREG?", 1200);
                if (response.indexOf("+CEREG:") < 0) response = SendRaw("AT+CREG?", 1200);
                status.link = AccountSystem::NETWORK_LINK_CELLULAR;
                status.registered = response.indexOf(",1") >= 0 || response.indexOf(",5") >= 0;
                status.signalDbm = ParseSignalDbm(SendRaw("AT+CSQ", 1000));
                ParseNetworkClock(SendRaw("AT+CCLK?", 1000), status);
                CopyText(status.operatorName, sizeof(status.operatorName), status.registered ? "4G" : "搜索网络");
            }
            Serial.printf("4G: registered=%s signal=%d dBm clock=%s\r\n",
                status.registered ? "yes" : "no",
                status.signalDbm,
                status.timeValid ? "valid" : "unavailable");
            xQueueSend(resultQueue, &status, 0);
        }
    }
}

bool QueueRequest(const WorkerRequest& request)
{
    return requestQueue && xQueueSend(requestQueue, &request, 0) == pdTRUE;
}
}

void HAL::Cellular_Init()
{
    if (CONFIG_WIFI_SSID[0] != '\0')
    {
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
        Serial.printf("Network: connecting to hotspot %s; 4G remains fallback\r\n", CONFIG_WIFI_SSID);
    }
    modem.begin(CONFIG_CELLULAR_BAUD, SERIAL_8N1, CONFIG_CELLULAR_RX_PIN, CONFIG_CELLULAR_TX_PIN);
    requestQueue = xQueueCreate(4, sizeof(WorkerRequest));
    resultQueue = xQueueCreate(6, sizeof(AccountSystem::Network_Info_t));
    memset(&currentState, 0, sizeof(currentState));
    currentState.link = AccountSystem::NETWORK_LINK_CELLULAR;
    currentState.event = AccountSystem::NETWORK_EVENT_STATUS;
    CopyText(currentState.operatorName, sizeof(currentState.operatorName), "初始化");
    if (requestQueue && resultQueue)
        xTaskCreatePinnedToCore(Worker, "CareGoNetwork", 6144, nullptr, 1, nullptr, 0);
    Serial.printf("4G: UART1 RX%d/TX%d at %d baud\r\n",
        CONFIG_CELLULAR_RX_PIN, CONFIG_CELLULAR_TX_PIN, CONFIG_CELLULAR_BAUD);
}

void HAL::Cellular_Update()
{
    if (initialPublishPending)
    {
        initialPublishPending = false;
        AccountSystem::Network_Commit(&currentState);
    }
    AccountSystem::Network_Info_t result = {};
    while (resultQueue && xQueueReceive(resultQueue, &result, 0) == pdTRUE)
    {
        currentState = result;
        AccountSystem::Network_Commit(&currentState);
    }
}

bool HAL::Cellular_QueryNearby(double latitude, double longitude)
{
#if !CONFIG_CELLULAR_ENABLE_EXTERNAL_REQUESTS
    (void)latitude;
    (void)longitude;
    Serial.println("Network: external toilet query disabled in Config.h");
    return false;
#else
    WorkerRequest request = {};
    request.command = AccountSystem::NETWORK_CMD_QUERY_NEARBY;
    request.requestId = millis();
    request.latitude = latitude;
    request.longitude = longitude;
    return QueueRequest(request);
#endif
}

bool HAL::Cellular_QueryLbs()
{
#if !CONFIG_CELLULAR_ENABLE_LBS
    Serial.println("4G LBS: disabled in Config.h");
    return false;
#else
    WorkerRequest request = {};
    request.command = AccountSystem::NETWORK_CMD_QUERY_LBS;
    request.requestId = millis();
    return QueueRequest(request);
#endif
}

bool HAL::Cellular_SendSms(const char* phone, const char* message)
{
#if !CONFIG_CELLULAR_ENABLE_SMS
    (void)phone;
    (void)message;
    Serial.println("Network: real SMS disabled in Config.h");
    return false;
#else
    if (!phone || !phone[0] || !message || !message[0]) return false;
    WorkerRequest request = {};
    request.command = AccountSystem::NETWORK_CMD_SEND_SMS;
    request.requestId = millis();
    CopyText(request.phone, sizeof(request.phone), phone);
    CopyText(request.message, sizeof(request.message), message);
    return QueueRequest(request);
#endif
}
