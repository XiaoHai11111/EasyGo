/**
 * @file    SettingModel.h
 * @brief   设置页数据模型定义
 * @details 本文件定义 EasyGo 的设置页数据模型定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once
#include "App/Services/CareGoClient.h"

namespace Page
{
class SettingModel
{
public:
    SettingModel() : brightness(80) {}
    void Init() { client.Init("SettingModel"); }
    void Deinit() {}
    bool ToggleVoice()
    {
        const bool enabled = !client.State().voiceEnabled;
        client.Send(AccountSystem::CAREGO_CMD_SET_VOICE_ENABLED, enabled);
        return enabled;
    }
    bool VoiceEnabled() const { return client.State().voiceEnabled; }
    int Brightness() const { return brightness; }
    bool SetHomeHere() { return client.Send(AccountSystem::CAREGO_CMD_SET_HOME_HERE); }
    AccountSystem::CareGo_State_t State() const { return client.State(); }

private:
    int brightness;
    CareGoClient client;
};
}
