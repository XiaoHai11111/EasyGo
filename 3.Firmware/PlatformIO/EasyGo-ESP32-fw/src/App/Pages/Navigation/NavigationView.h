/**
 * @file    NavigationView.h
 * @brief   导航页视图定义
 * @details 本文件定义 EasyGo 的导航页视图定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "../Common/EasyGoUi.h"
#include "App/Accounts/ACT_Def.h"

namespace Page
{
class NavigationView
{
public:
    struct
    {
        lv_obj_t* back;
        lv_obj_t* zoomIn;
        lv_obj_t* zoomOut;
        lv_obj_t* mapViewport;
        lv_obj_t* mapContent;
        lv_obj_t* mapImage;
        lv_obj_t* distance;
        lv_obj_t* minutes;
        lv_obj_t* topDistance;
        lv_obj_t* target;
        lv_obj_t* status;
    } ui;

    void Create(lv_obj_t* root, const char* mapPath, int distanceMeters, int minutes);
    void SetMapSource(const char* mapPath);
    void UpdateState(const AccountSystem::CareGo_State_t& state);
    void Delete() {}

private:
    lv_point_t routePoints[5];
};
}
