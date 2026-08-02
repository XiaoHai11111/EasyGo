/**
 * @file    ACT_CareGo.cpp
 * @brief   CareGo 导航、提醒与 SOS 状态机账户
 * @details 本文件实现 EasyGo 账户系统中的CareGo 导航、提醒与 SOS 状态机账户，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "Account_Master.h"
#include "App/Configs/Config.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

using namespace AccountSystem;

namespace
{
Account* careGoAccount = nullptr;
CareGo_State_t state = {};
uint32_t requestSequence = 0;
uint32_t lastReminderMs = 0;
bool queryForNavigation = false;
bool gpsUnavailableActive = false;
uint32_t gpsUnavailableSinceMs = 0;
uint32_t lastLbsRequestMs = 0;

void CopyText(char* target, size_t size, const char* text)
{
    if (!target || size == 0) return;
    strncpy(target, text ? text : "", size - 1);
    target[size - 1] = '\0';
}

void PublishState(const char* status = nullptr)
{
    if (status) CopyText(state.statusText, sizeof(state.statusText), status);
    if (careGoAccount && careGoAccount->Commit(&state, sizeof(state))) careGoAccount->Publish();
}

void Play(const char* track)
{
    MusicPlayer_Info_t info = { track };
    careGoAccount->Notify("MusicPlayer", &info, sizeof(info));
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

bool MaybeRequestLbs()
{
    const uint32_t now = lv_tick_get();
    if (state.location.valid &&
        (state.location.source == LOCATION_SOURCE_GPS || state.location.source == LOCATION_SOURCE_SIMULATOR))
    {
        gpsUnavailableActive = false;
        gpsUnavailableSinceMs = 0;
        lastLbsRequestMs = 0;
        return false;
    }
    if (state.location.valid && state.location.source == LOCATION_SOURCE_LBS &&
        now - state.location.updatedAtMs <= CONFIG_CELLULAR_LBS_VALID_MS)
        return false;

    if (!gpsUnavailableActive)
    {
        gpsUnavailableActive = true;
        gpsUnavailableSinceMs = now;
        return false;
    }
    if (now - gpsUnavailableSinceMs < CONFIG_CELLULAR_LBS_GPS_GRACE_MS) return false;
    if (lastLbsRequestMs != 0 && now - lastLbsRequestMs < CONFIG_CELLULAR_LBS_RETRY_MS) return false;

    Network_Command_t request = {};
    request.cmd = NETWORK_CMD_QUERY_LBS;
    request.requestId = ++requestSequence;
    lastLbsRequestMs = now == 0 ? 1 : now;
    PublishState("GPS 不可用，正在尝试 4G 定位");
    careGoAccount->Notify("Network", &request, sizeof(request));
    return true;
}

void RequestNearby(bool forNavigation)
{
    if (!state.location.valid)
    {
        if (forNavigation)
        {
            state.navigation = CAREGO_NAV_LOCATING;
            PublishState("正在定位，请到室外开阔处");
            if (state.voiceEnabled) Play("GpsUnavailable");
        }
        return;
    }

    Network_Command_t request = {};
    request.cmd = NETWORK_CMD_QUERY_NEARBY;
    request.requestId = ++requestSequence;
    request.latitude = state.location.latitude;
    request.longitude = state.location.longitude;
    queryForNavigation = forNavigation;
    if (forNavigation)
    {
        state.navigation = CAREGO_NAV_SEARCHING;
        PublishState("正在查找最近厕所");
    }
    careGoAccount->Notify("Network", &request, sizeof(request));
}

void StartHomeNavigation()
{
    if (!state.homeConfigured)
    {
        state.navigation = CAREGO_NAV_FAILED;
        PublishState("请先在设置中将当前位置设为家");
        return;
    }
    if (!state.location.valid)
    {
        state.navigation = CAREGO_NAV_LOCATING;
        PublishState("正在定位，暂时无法导航回家");
        return;
    }

    state.distanceMeters = static_cast<int32_t>(DistanceMeters(
        state.location.latitude,
        state.location.longitude,
        state.homeLatitude,
        state.homeLongitude));
    state.minutes = static_cast<int16_t>((state.distanceMeters + 59) / 60);
    state.navigation = state.distanceMeters <= 30 ? CAREGO_NAV_ARRIVED : CAREGO_NAV_NAVIGATING_HOME;
    CopyText(state.targetName, sizeof(state.targetName), "家");
    PublishState(state.navigation == CAREGO_NAV_ARRIVED ? "已在家附近" : "正在导航回家");
    if (state.voiceEnabled) Play("HomeNavigation");
}

void SendSos(const char* requestedPhone)
{
    const char* phone = requestedPhone && requestedPhone[0] ? requestedPhone : CONFIG_EMERGENCY_PHONE_DEFAULT;
    if (!phone || !phone[0])
    {
        state.sos = CAREGO_SOS_NOT_CONFIGURED;
        PublishState("未配置可发送短信的紧急联系人");
        return;
    }

    Network_Command_t request = {};
    request.cmd = NETWORK_CMD_SEND_SMS;
    request.requestId = ++requestSequence;
    CopyText(request.phone, sizeof(request.phone), phone);
    if (state.location.valid)
    {
        const char* source = state.location.source == LOCATION_SOURCE_LBS ? "Approximate LBS" : "GPS";
        snprintf(request.message, sizeof(request.message),
            "[CareGO SOS] Need help. %s: %.6f,%.6f (~%.0fm). Please contact me.",
            source,
            state.location.latitude,
            state.location.longitude,
            state.location.horizontalAccuracyMeters);
    }
    else
    {
        CopyText(request.message, sizeof(request.message),
            "[CareGO SOS] Need help. Current location is unavailable. Please contact me.");
    }

    state.sos = CAREGO_SOS_SENDING;
    PublishState("正在发送求助短信");
    if (state.voiceEnabled) Play("SosSending");
    careGoAccount->Notify("Network", &request, sizeof(request));
}

void HandleCommand(const CareGo_Command_t& command)
{
    switch (command.cmd)
    {
        case CAREGO_CMD_NAVIGATE_TOILET:
            RequestNearby(true);
            break;
        case CAREGO_CMD_NAVIGATE_HOME:
            StartHomeNavigation();
            break;
        case CAREGO_CMD_PLAY_HELP_AUDIO:
            if (state.voiceEnabled)
            {
                state.audioPlaying = true;
                Play("HelpMandarin");
                PublishState("正在播放普通话求助语音");
            }
            else PublishState("语音提醒已在设置中关闭");
            break;
        case CAREGO_CMD_SEND_SOS:
            SendSos(command.phone);
            break;
        case CAREGO_CMD_SET_HOME_HERE:
            if (!state.location.valid) PublishState("定位无效，不能设置家庭位置");
            else
            {
                state.homeLatitude = state.location.latitude;
                state.homeLongitude = state.location.longitude;
                state.homeConfigured = true;
                PublishState("已将当前位置设为家");
            }
            break;
        case CAREGO_CMD_SET_VOICE_ENABLED:
            state.voiceEnabled = command.enabled;
            PublishState(command.enabled ? "语音提醒已开启" : "语音提醒已关闭");
            break;
        case CAREGO_CMD_STOP_NAVIGATION:
            state.navigation = CAREGO_NAV_IDLE;
            PublishState("已结束导航");
            break;
    }
}

void HandleInput(const Input_Info_t& input)
{
    CareGo_Command_t command = {};
    switch (input.action)
    {
        case INPUT_TOILET_CLICK: command.cmd = CAREGO_CMD_NAVIGATE_TOILET; break;
        case INPUT_HELP_AUDIO_DOUBLE_CLICK: command.cmd = CAREGO_CMD_PLAY_HELP_AUDIO; break;
        case INPUT_HOME_CLICK: command.cmd = CAREGO_CMD_NAVIGATE_HOME; break;
        case INPUT_SOS_DOUBLE_CLICK: command.cmd = CAREGO_CMD_SEND_SOS; break;
        default: return;
    }
    HandleCommand(command);
}

void HandleNetwork(const Network_Info_t& info)
{
    state.network = info;
    if (info.event == NETWORK_EVENT_NEARBY_RESULT)
    {
        if (!info.success)
        {
            if (queryForNavigation)
            {
                state.navigation = CAREGO_NAV_FAILED;
                PublishState("网络不可用，离线厕所数据待接入 SD 卡");
            }
            return;
        }

        state.distanceMeters = info.distanceMeters;
        state.minutes = static_cast<int16_t>((info.distanceMeters + 59) / 60);
        CopyText(state.targetName, sizeof(state.targetName), info.targetName[0] ? info.targetName : "公共厕所");
        if (queryForNavigation)
        {
            state.navigation = CAREGO_NAV_NAVIGATING_TOILET;
            PublishState("已找到最近厕所，开始导航");
        }
        else if (info.distanceMeters <= 200 &&
                 (lastReminderMs == 0 || lv_tick_get() - lastReminderMs >= 30UL * 60UL * 1000UL))
        {
            lastReminderMs = lv_tick_get();
            PublishState("附近有厕所，单击按键开始导航");
            if (state.voiceEnabled) Play("ToiletNearby");
        }
    }
    else if (info.event == NETWORK_EVENT_SMS_RESULT)
    {
        state.sos = info.success ? CAREGO_SOS_ACCEPTED : CAREGO_SOS_FAILED;
        PublishState(info.success ? "求助短信已由模块受理" : "求助短信发送失败");
        if (state.voiceEnabled) Play(info.success ? "SosSent" : "SosFailed");
    }
    else if (info.event == NETWORK_EVENT_LBS_RESULT)
    {
        if (state.navigation == CAREGO_NAV_IDLE)
            PublishState(info.success ? "GPS 不可用，已切换 4G 定位" : "GPS 与 4G 定位暂不可用");
        else PublishState();
    }
    else PublishState();
}

int onEvent(Account*, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_SUB_PULL)
    {
        if (param->size != sizeof(state)) return Account::ERROR_SIZE_MISMATCH;
        memcpy(param->data_p, &state, sizeof(state));
        return Account::ERROR_NONE;
    }
    if (param->event == Account::EVENT_NOTIFY)
    {
        if (param->size != sizeof(CareGo_Command_t)) return Account::ERROR_SIZE_MISMATCH;
        HandleCommand(*static_cast<CareGo_Command_t*>(param->data_p));
        return Account::ERROR_NONE;
    }
    if (param->event == Account::EVENT_TIMER)
    {
        if (state.navigation == CAREGO_NAV_IDLE) RequestNearby(false);
        return Account::ERROR_NONE;
    }
    if (param->event != Account::EVENT_PUB_PUBLISH) return Account::ERROR_UNSUPPORTED_REQUEST;

    if (strcmp(param->tran->ID, "Location") == 0 && param->size == sizeof(Location_Info_t))
    {
        state.location = *static_cast<Location_Info_t*>(param->data_p);
        if (state.navigation == CAREGO_NAV_LOCATING && state.location.valid) RequestNearby(true);
        else if (!MaybeRequestLbs()) PublishState();
    }
    else if (strcmp(param->tran->ID, "Network") == 0 && param->size == sizeof(Network_Info_t))
        HandleNetwork(*static_cast<Network_Info_t*>(param->data_p));
    else if (strcmp(param->tran->ID, "Input") == 0 && param->size == sizeof(Input_Info_t))
        HandleInput(*static_cast<Input_Info_t*>(param->data_p));
    else if (strcmp(param->tran->ID, "MusicPlayer") == 0 && param->size == sizeof(MusicPlayer_State_t))
    {
        const MusicPlayer_State_t& music = *static_cast<MusicPlayer_State_t*>(param->data_p);
        state.audioPlaying = music.playing;
        state.sdPresent = music.storagePresent;
        PublishState();
    }
    return Account::ERROR_NONE;
}
}

ACCOUNT_INIT_DEF(CareGo)
{
    careGoAccount = account;
    memset(&state, 0, sizeof(state));
    gpsUnavailableActive = false;
    gpsUnavailableSinceMs = 0;
    lastLbsRequestMs = 0;
    state.voiceEnabled = CONFIG_SOUND_ENABLE_DEFAULT;
    state.navigation = CAREGO_NAV_IDLE;
    state.sos = CAREGO_SOS_IDLE;
    CopyText(state.statusText, sizeof(state.statusText), "准备就绪");
    account->Subscribe("Location");
    account->Subscribe("Network");
    account->Subscribe("Input");
    account->Subscribe("MusicPlayer");
    account->SetEventCallback(onEvent);
    account->Pull("Network", &state.network, sizeof(state.network));
    account->SetTimerPeriod(120000);
    account->Commit(&state, sizeof(state));

#ifdef _WIN32
    state.sdPresent = true;
    Location_Info_t location = {};
    location.valid = true;
    location.source = LOCATION_SOURCE_SIMULATOR;
    location.latitude = 22.551365;
    location.longitude = 113.881238;
    location.horizontalAccuracyMeters = 8.0f;
    location.satellites = 12;
    location.fixType = 3;
    Location_Commit(&location);
#endif
}
