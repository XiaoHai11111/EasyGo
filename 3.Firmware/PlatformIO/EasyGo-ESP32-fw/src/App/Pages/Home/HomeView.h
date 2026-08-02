/**
 * @file    HomeView.h
 * @brief   首页视图定义
 * @details 本文件定义 EasyGo 的首页视图定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "../Common/EasyGoUi.h"

namespace Page
{
class HomeView
{
public:
    struct
    {
        lv_obj_t* navigation;
        lv_obj_t* family;
        lv_obj_t* setting;
    } ui;

    void Create(lv_obj_t* root);
    void Delete() {}

private:
    lv_obj_t* CreateCard(lv_obj_t* root, lv_coord_t y, lv_color_t color,
                         int iconType, const char* title);
    void CreateLocationIcon(lv_obj_t* parent, lv_color_t color);
    void CreateFamilyIcon(lv_obj_t* parent, lv_color_t color);
};
}
