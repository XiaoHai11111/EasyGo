/**
 * @file    SettingView.h
 * @brief   设置页视图定义
 * @details 本文件定义 EasyGo 的设置页视图定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "../Common/EasyGoUi.h"
#include "App/Accounts/ACT_Def.h"

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
        lv_obj_t* homeRow;
        lv_obj_t* diagnosticsRow;
        lv_obj_t* feedback;
        lv_obj_t* backHome;
    } ui;

    void Create(lv_obj_t* root, bool voiceEnabled, int brightness);
    void SetVoice(bool enabled);
    void UpdateState(const AccountSystem::CareGo_State_t& state);
    void Delete() {}

private:
    lv_obj_t* CreateRow(lv_obj_t* root, int index, const char* symbol, lv_color_t color,
                        const char* title, const char* detail, bool arrow);
};
}
