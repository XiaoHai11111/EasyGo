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
