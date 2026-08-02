/**
 * @file    FamilyView.h
 * @brief   亲情联系人页视图定义
 * @details 本文件定义 EasyGo 的亲情联系人页视图定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "../Common/EasyGoUi.h"
#include "FamilyModel.h"

namespace Page
{
class FamilyView
{
public:
    struct
    {
        lv_obj_t* back;
        lv_obj_t* list;
        lv_obj_t* call[FamilyModel::MaxContacts];
        lv_obj_t* sos;
        lv_obj_t* feedback;
        uint8_t contactCount;
    } ui;

    void Create(lv_obj_t* root, const FamilyModel& model);
    void ShowFeedback(const char* text, lv_color_t color);
    void Delete() {}

private:
    lv_obj_t* CreateContact(lv_obj_t* root, int index, const FamilyModel::Contact& contact);
};
}
