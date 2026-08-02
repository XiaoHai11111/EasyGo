/**
 * @file    TemplateView.h
 * @brief   新页面模板视图定义
 * @details 本文件定义 EasyGo 的新页面模板视图定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#ifndef __TEMPLATE_VIEW_H
#define __TEMPLATE_VIEW_H

#include "../Page.h"

namespace Page
{

class TemplateView
{
public:
    void Create(lv_obj_t* root);

public:
    struct
    {
        lv_obj_t* labelTitle;
        lv_obj_t* labelTick;
        lv_obj_t* canvas;
        lv_group_t* group;
    } ui;

private:

};

}

#endif // !__VIEW_H
