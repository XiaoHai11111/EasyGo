#pragma once

#include "../Common/EasyGoUi.h"

namespace Page
{
class SettingView
{
public:
    struct
    {
        lv_obj_t* voiceRow;
        lv_obj_t* voiceSwitch;
        lv_obj_t* voiceKnob;
        lv_obj_t* backHome;
    } ui;

    void Create(lv_obj_t* root, bool voiceEnabled, int brightness);
    void SetVoice(bool enabled);
    void Delete() {}

private:
    lv_obj_t* CreateRow(lv_obj_t* root, int index, const char* symbol, lv_color_t color,
                        const char* title, const char* detail, bool arrow);
};
}
