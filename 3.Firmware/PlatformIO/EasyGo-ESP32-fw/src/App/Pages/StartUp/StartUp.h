/**
 * @file    StartUp.h
 * @brief   启动页定义
 * @details 本文件定义 EasyGo 的启动页定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "StartupView.h"
#include "StartupModel.h"

namespace Page
{
class Startup : public PageBase
{
public:
    Startup();
    virtual ~Startup();
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
    static void onTimer(lv_timer_t* timer);
    StartupView View;
    StartupModel Model;
    lv_timer_t* timer;
};
}
