/**
 * @file    ACT_Network.cpp
 * @brief   网络/4G 账户、POI 查询与降级策略
 * @details 本文件实现 EasyGo 账户系统中的网络/4G 账户、POI 查询与降级策略，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "Account_Master.h"
#include "HAL/HAL.h"
#include "lvgl.h"
#include <math.h>
#include <string.h>
#ifdef _WIN32
#include <time.h>
#endif

using namespace AccountSystem;

static Account* actNetwork = nullptr;
static Network_Info_t networkState = {};

namespace
{
uint16_t ReadU16(const uint8_t* data)
{
    return static_cast<uint16_t>(data[0]) |
        static_cast<uint16_t>(data[1]) << 8;
}

uint32_t ReadU32(const uint8_t* data)
{
    return static_cast<uint32_t>(data[0]) |
        static_cast<uint32_t>(data[1]) << 8 |
        static_cast<uint32_t>(data[2]) << 16 |
        static_cast<uint32_t>(data[3]) << 24;
}

double DistanceMeters(double lat1, double lon1, double lat2, double lon2)
{
    const double radians = 0.017453292519943295;
    const double dLat = (lat2 - lat1) * radians;
    const double dLon = (lon2 - lon1) * radians;
    const double a = sin(dLat / 2) * sin(dLat / 2) +
        cos(lat1 * radians) * cos(lat2 * radians) * sin(dLon / 2) * sin(dLon / 2);
    return 6371000.0 * 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
}

bool ReadExact(lv_fs_file_t* file, void* buffer, uint32_t size)
{
    uint32_t read = 0;
    return lv_fs_read(file, buffer, size, &read) == LV_FS_RES_OK && read == size;
}

bool TryOfflineNearby(const Network_Command_t& command, Network_Info_t* result)
{
    if (!result) return false;
#ifdef _WIN32
    const char* path = "/SD/MAP/shenzhen_toilets.bin";
#else
    const char* path = "S:/MAP/shenzhen_toilets.bin";
#endif
    lv_fs_file_t file;
    if (lv_fs_open(&file, path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;

    uint8_t header[48];
    bool valid = ReadExact(&file, header, sizeof(header));
    valid = valid && memcmp(header, "EGTOIL1", 7) == 0 &&
        ReadU16(header + 8) == 1 && ReadU16(header + 10) == sizeof(header) &&
        ReadU32(header + 16) == 64 && ReadU32(header + 20) == 10000000;
    const uint32_t count = valid ? ReadU32(header + 12) : 0;
    valid = valid && count > 0 && count <= 4096;

    double bestDistance = 1.0e30;
    double bestLatitude = 0.0;
    double bestLongitude = 0.0;
    char bestName[40] = {};
    uint8_t record[64];
    for (uint32_t index = 0; valid && index < count; ++index)
    {
        if (!ReadExact(&file, record, sizeof(record)))
        {
            valid = false;
            break;
        }
        const int32_t latitudeE7 = static_cast<int32_t>(ReadU32(record + 8));
        const int32_t longitudeE7 = static_cast<int32_t>(ReadU32(record + 12));
        const double latitude = latitudeE7 / 10000000.0;
        const double longitude = longitudeE7 / 10000000.0;
        const double distance = DistanceMeters(command.latitude, command.longitude, latitude, longitude);
        if (distance >= bestDistance) continue;
        bestDistance = distance;
        bestLatitude = latitude;
        bestLongitude = longitude;
        const uint8_t nameLength = record[19] < sizeof(bestName) - 1 ? record[19] : sizeof(bestName) - 1;
        memcpy(bestName, record + 20, nameLength);
        bestName[nameLength] = '\0';
    }
    lv_fs_close(&file);
    if (!valid || bestDistance == 1.0e30) return false;

    result->event = NETWORK_EVENT_NEARBY_RESULT;
    result->success = true;
    result->busy = false;
    result->errorCode = 0;
    result->requestId = command.requestId;
    result->distanceMeters = static_cast<int32_t>(bestDistance + 0.5);
    result->targetLatitude = bestLatitude;
    result->targetLongitude = bestLongitude;
    strncpy(result->targetName, bestName[0] ? bestName : "公共厕所", sizeof(result->targetName) - 1);
    strncpy(result->detail, "离线 SD 厕所索引", sizeof(result->detail) - 1);
    return true;
}
}

void AccountSystem::Network_Commit(const Network_Info_t* info)
{
    if (!actNetwork || !info) return;
    networkState = *info;
    if (networkState.event == NETWORK_EVENT_LBS_RESULT && networkState.success)
    {
        Location_Info_t location = {};
        location.valid = true;
        location.source = LOCATION_SOURCE_LBS;
        location.latitude = networkState.targetLatitude;
        location.longitude = networkState.targetLongitude;
        location.horizontalAccuracyMeters = networkState.locationAccuracyMeters;
        location.updatedAtMs = lv_tick_get();
        Location_LbsCommit(&location);
    }
    if (actNetwork->Commit(&networkState, sizeof(networkState))) actNetwork->Publish();
}

static void PublishRejected(const Network_Command_t* command, int16_t error, const char* detail)
{
    Network_Info_t info = networkState;
    if (command->cmd == NETWORK_CMD_SEND_SMS) info.event = NETWORK_EVENT_SMS_RESULT;
    else if (command->cmd == NETWORK_CMD_QUERY_LBS) info.event = NETWORK_EVENT_LBS_RESULT;
    else info.event = NETWORK_EVENT_NEARBY_RESULT;
    info.requestId = command->requestId;
    info.busy = false;
    info.success = false;
    info.errorCode = error;
    strncpy(info.detail, detail, sizeof(info.detail) - 1);
    Network_Commit(&info);
}

static int onEvent(Account*, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_SUB_PULL)
    {
        if (param->size != sizeof(Network_Info_t)) return Account::ERROR_SIZE_MISMATCH;
        memcpy(param->data_p, &networkState, sizeof(networkState));
        return Account::ERROR_NONE;
    }

    if (param->event != Account::EVENT_NOTIFY) return Account::ERROR_UNSUPPORTED_REQUEST;
    if (param->size != sizeof(Network_Command_t)) return Account::ERROR_SIZE_MISMATCH;

    const Network_Command_t* command = static_cast<const Network_Command_t*>(param->data_p);
    if (command->cmd == NETWORK_CMD_QUERY_NEARBY)
    {
        Network_Info_t offline = networkState;
        if (TryOfflineNearby(*command, &offline))
        {
            Network_Commit(&offline);
            return Account::ERROR_NONE;
        }
    }
#ifdef _WIN32
    Network_Info_t result = networkState;
    result.requestId = command->requestId;
    result.busy = false;
    result.success = true;
    result.errorCode = 0;
    if (command->cmd == NETWORK_CMD_QUERY_NEARBY)
    {
        result.event = NETWORK_EVENT_NEARBY_RESULT;
        result.distanceMeters = 120;
        result.targetLatitude = 22.551650;
        result.targetLongitude = 113.880720;
        strncpy(result.targetName, "公共厕所", sizeof(result.targetName) - 1);
        strncpy(result.detail, "模拟器离线厕所索引", sizeof(result.detail) - 1);
    }
    else if (command->cmd == NETWORK_CMD_QUERY_LBS)
    {
        result.event = NETWORK_EVENT_LBS_RESULT;
        result.targetLatitude = 22.551365;
        result.targetLongitude = 113.881238;
        result.locationAccuracyMeters = 450.0f;
        strncpy(result.targetName, "4G LBS", sizeof(result.targetName) - 1);
        strncpy(result.detail, "模拟 4G 基站定位", sizeof(result.detail) - 1);
    }
    else
    {
        result.event = NETWORK_EVENT_SMS_RESULT;
        strncpy(result.detail, "模拟发送，未连接真实网络", sizeof(result.detail) - 1);
    }
    Network_Commit(&result);
    return Account::ERROR_NONE;
#else
    bool accepted = false;
    if (command->cmd == NETWORK_CMD_QUERY_NEARBY)
        accepted = HAL::Cellular_QueryNearby(command->latitude, command->longitude);
    else if (command->cmd == NETWORK_CMD_SEND_SMS)
        accepted = HAL::Cellular_SendSms(command->phone, command->message);
    else if (command->cmd == NETWORK_CMD_QUERY_LBS)
        accepted = HAL::Cellular_QueryLbs();

    if (!accepted) PublishRejected(command, -100, "请求未入队，请检查配置或模块状态");
    return accepted ? Account::ERROR_NONE : Account::ERROR_UNKNOW;
#endif
}

ACCOUNT_INIT_DEF(Network)
{
    actNetwork = account;
    memset(&networkState, 0, sizeof(networkState));
#ifdef _WIN32
    networkState.link = NETWORK_LINK_WIFI;
    networkState.registered = true;
    networkState.signalDbm = -55;
    time_t now = time(nullptr);
    tm local = {};
    localtime_s(&local, &now);
    networkState.timeValid = true;
    networkState.localHour = static_cast<uint8_t>(local.tm_hour);
    networkState.localMinute = static_cast<uint8_t>(local.tm_min);
    networkState.localSecond = static_cast<uint8_t>(local.tm_sec);
    networkState.clockUpdatedAtMs = lv_tick_get();
    strncpy(networkState.operatorName, "SIM", sizeof(networkState.operatorName) - 1);
#endif
    networkState.event = NETWORK_EVENT_STATUS;
    account->SetEventCallback(onEvent);
    account->Commit(&networkState, sizeof(networkState));
}
