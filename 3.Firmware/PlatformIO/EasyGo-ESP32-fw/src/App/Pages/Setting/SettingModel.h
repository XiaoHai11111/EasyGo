#pragma once

namespace Page
{
class SettingModel
{
public:
    SettingModel() : voiceEnabled(true), brightness(80) {}
    void Init() {}
    void Deinit() {}
    bool ToggleVoice() { voiceEnabled = !voiceEnabled; return voiceEnabled; }
    bool VoiceEnabled() const { return voiceEnabled; }
    int Brightness() const { return brightness; }

private:
    bool voiceEnabled;
    int brightness;
};
}
