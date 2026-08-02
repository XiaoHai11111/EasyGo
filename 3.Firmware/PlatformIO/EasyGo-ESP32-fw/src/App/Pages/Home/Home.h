/**
 * @file    Home.h
 * @brief   首页定义
 * @details 本文件定义 EasyGo 的首页定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "HomeView.h"
#include "HomeModel.h"

namespace Page
{
class Home : public PageBase
{
public:
    Home();
    virtual ~Home();
    virtual void onCustomAttrConfig();
    virtual void onViewLoad();
    virtual void onViewDidLoad();
    virtual void onViewWillAppear();
    virtual void onViewDidAppear();
    virtual void onViewWillDisappear();
    virtual void onViewDidDisappear();
    virtual void onViewUnload();
    virtual void onViewDidUnload();

private:
    static void onEvent(lv_event_t* event);
    void Attach(lv_obj_t* object);
    HomeView View;
    HomeModel Model;
};
}
