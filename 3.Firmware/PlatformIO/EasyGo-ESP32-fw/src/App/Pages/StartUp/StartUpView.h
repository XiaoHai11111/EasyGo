/**
 * @file    StartUpView.h
 * @brief   启动页视图定义
 * @details 本文件定义 EasyGo 的启动页视图定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "../Common/EasyGoUi.h"

namespace Page
{
class StartupView
{
public:
    struct
    {
        lv_obj_t* logo;
        lv_obj_t* spinner;
        lv_anim_timeline_t* anim_timeline;
    } ui;

    void Create(lv_obj_t* root);
    void Delete();
};
}
