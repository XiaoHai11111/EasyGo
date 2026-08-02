/**
 * @file    HAL_GPS.cpp
 * @brief   GPS NMEA 数据解析与定位信息发布
 * @details 本文件实现 EasyGo 适老导航设备硬件抽象层的GPS NMEA 数据解析与定位信息发布，供上层账户与页面统一调用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "HAL/HAL.h"
#include "App/Accounts/Account_Master.h"
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <string.h>

namespace
{
EspSoftwareSerial::UART gpsSerial;
char lineBuffer[CONFIG_GPS_MAX_LINE_LENGTH] = {};
size_t lineLength = 0;
float lastSpeedMps = 0.0f;
float lastCourseDegrees = 0.0f;
uint32_t lastValidFixMs = 0;
uint32_t lastInvalidPublishMs = 0;
uint32_t lastDiagnosticMs = 0;
uint32_t baudStartedMs = 0;
uint32_t totalBytes = 0;
uint32_t validSentences = 0;
uint8_t lastUtcHour = 0;
uint8_t lastUtcMinute = 0;
uint8_t lastUtcSecond = 0;
uint32_t lastClockMs = 0;
const uint32_t baudOptions[] = { CONFIG_GPS_BAUD, 38400, 115200, 4800 };
size_t baudIndex = 0;
bool baudLocked = false;

bool ChecksumValid(const char* line)
{
    if (!line || line[0] != '$') return false;
    const char* star = strchr(line, '*');
    if (!star || strlen(star) < 3) return false;
    uint8_t checksum = 0;
    for (const char* p = line + 1; p < star; ++p) checksum ^= static_cast<uint8_t>(*p);
    return checksum == static_cast<uint8_t>(strtoul(star + 1, nullptr, 16));
}

int SplitFields(char* line, char* fields[], int capacity)
{
    int count = 0;
    char* current = line;
    while (current && count < capacity)
    {
        fields[count++] = current;
        char* comma = strchr(current, ',');
        if (!comma) break;
        *comma = '\0';
        current = comma + 1;
    }
    return count;
}

double NmeaCoordinate(const char* value, char hemisphere)
{
    const double raw = value ? atof(value) : 0.0;
    const int degrees = static_cast<int>(raw / 100.0);
    double decimal = degrees + (raw - degrees * 100.0) / 60.0;
    if (hemisphere == 'S' || hemisphere == 'W') decimal = -decimal;
    return decimal;
}

void ParseUtcTime(const char* value)
{
    if (!value || strlen(value) < 6) return;
    const int hour = (value[0] - '0') * 10 + value[1] - '0';
    const int minute = (value[2] - '0') * 10 + value[3] - '0';
    const int second = (value[4] - '0') * 10 + value[5] - '0';
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 60) return;
    lastUtcHour = static_cast<uint8_t>(hour);
    lastUtcMinute = static_cast<uint8_t>(minute);
    lastUtcSecond = static_cast<uint8_t>(second > 59 ? 59 : second);
    lastClockMs = millis();
}

void FillClock(AccountSystem::Location_Info_t& info)
{
    info.timeValid = lastClockMs != 0;
    info.utcHour = lastUtcHour;
    info.utcMinute = lastUtcMinute;
    info.utcSecond = lastUtcSecond;
    info.clockUpdatedAtMs = lastClockMs;
}

void PublishLocation(double latitude, double longitude, uint8_t fixType,
                     uint8_t satellites, float hdop, float altitude)
{
    AccountSystem::Location_Info_t info = {};
    info.source = AccountSystem::LOCATION_SOURCE_GPS;
    info.valid = true;
    info.latitude = latitude;
    info.longitude = longitude;
    info.fixType = fixType;
    info.satellites = satellites;
    info.hdop = hdop;
    info.horizontalAccuracyMeters = hdop > 0.0f ? hdop * 5.0f : 99.0f;
    info.altitudeMeters = altitude;
    info.speedMetersPerSecond = lastSpeedMps;
    info.courseDegrees = lastCourseDegrees;
    FillClock(info);
    info.updatedAtMs = millis();
    lastValidFixMs = info.updatedAtMs;
    AccountSystem::Location_GpsCommit(&info);
}

void PublishInvalid()
{
    const uint32_t now = millis();
    if (now - lastInvalidPublishMs < 2000) return;
    lastInvalidPublishMs = now;
    AccountSystem::Location_Info_t info = {};
    info.source = AccountSystem::LOCATION_SOURCE_GPS;
    info.valid = false;
    FillClock(info);
    info.updatedAtMs = now;
    AccountSystem::Location_GpsCommit(&info);
}

bool ParseSentence(char* sentence)
{
    if (!ChecksumValid(sentence)) return false;
    ++validSentences;
    baudLocked = true;
    char* star = strchr(sentence, '*');
    if (star) *star = '\0';

    char* fields[20] = {};
    const int count = SplitFields(sentence, fields, 20);
    if (count < 2) return true;

    if ((strcmp(fields[0], "$GPRMC") == 0 || strcmp(fields[0], "$GNRMC") == 0) && count > 8)
    {
        ParseUtcTime(fields[1]);
        if (fields[7][0]) lastSpeedMps = static_cast<float>(atof(fields[7]) * 0.514444);
        if (fields[8][0]) lastCourseDegrees = static_cast<float>(atof(fields[8]));
        if (count > 6 && fields[2][0] == 'A' && fields[3][0] && fields[5][0])
        {
            PublishLocation(
                NmeaCoordinate(fields[3], fields[4][0]),
                NmeaCoordinate(fields[5], fields[6][0]),
                1,
                0,
                0.0f,
                0.0f);
        }
        return true;
    }

    if (strcmp(fields[0], "$GPGGA") != 0 && strcmp(fields[0], "$GNGGA") != 0) return true;
    ParseUtcTime(fields[1]);
    if (count < 10 || atoi(fields[6]) <= 0 || !fields[2][0] || !fields[4][0])
    {
        PublishInvalid();
        return true;
    }
    PublishLocation(
        NmeaCoordinate(fields[2], fields[3][0]),
        NmeaCoordinate(fields[4], fields[5][0]),
        static_cast<uint8_t>(atoi(fields[6])),
        static_cast<uint8_t>(atoi(fields[7])),
        static_cast<float>(atof(fields[8])),
        static_cast<float>(atof(fields[9])));
    return true;
}

void StartBaud(uint32_t baud)
{
    gpsSerial.end();
    gpsSerial.begin(baud, SWSERIAL_8N1, CONFIG_GPS_RX_PIN, -1, false, 256);
    gpsSerial.enableIntTx(false);
    lineLength = 0;
    baudStartedMs = millis();
    Serial.printf("GPS: listening on RX GPIO%d at %lu baud\r\n",
        CONFIG_GPS_RX_PIN, static_cast<unsigned long>(baud));
}
}

void HAL::GPS_Init()
{
    baudIndex = 0;
    baudLocked = false;
    totalBytes = 0;
    validSentences = 0;
    StartBaud(baudOptions[baudIndex]);
}

void HAL::GPS_Update()
{
    while (gpsSerial.available() > 0)
    {
        const char c = static_cast<char>(gpsSerial.read());
        ++totalBytes;
        if (c == '\n')
        {
            lineBuffer[lineLength] = '\0';
            if (lineLength > 6) ParseSentence(lineBuffer);
            lineLength = 0;
        }
        else if (c != '\r')
        {
            if (lineLength + 1 < sizeof(lineBuffer)) lineBuffer[lineLength++] = c;
            else lineLength = 0;
        }
    }

    if (lastValidFixMs == 0 || millis() - lastValidFixMs > 5000) PublishInvalid();

    const uint32_t now = millis();
    if (!baudLocked && now - baudStartedMs >= 5000)
    {
        baudIndex = (baudIndex + 1) % (sizeof(baudOptions) / sizeof(baudOptions[0]));
        StartBaud(baudOptions[baudIndex]);
    }
    if (now - lastDiagnosticMs >= 5000)
    {
        lastDiagnosticMs = now;
        Serial.printf("GPS: baud=%lu bytes=%lu valid_nmea=%lu fix=%s\r\n",
            static_cast<unsigned long>(baudOptions[baudIndex]),
            static_cast<unsigned long>(totalBytes),
            static_cast<unsigned long>(validSentences),
            lastValidFixMs && now - lastValidFixMs <= 5000 ? "yes" : "no");
    }
}
