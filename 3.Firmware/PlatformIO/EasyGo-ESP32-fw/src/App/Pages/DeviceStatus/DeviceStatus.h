/**
 * @file    DeviceStatus.h
 * @brief   设备状态页定义
 * @details 本文件定义 EasyGo 的设备状态页定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "DeviceStatusModel.h"
#include "DeviceStatusView.h"

namespace Page
{
class DeviceStatus : public PageBase
{
public:
    DeviceStatus();
    virtual ~DeviceStatus();
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
    static void onBack(lv_event_t* event);
    static void onRefresh(lv_timer_t* timer);
    DeviceStatusModel Model;
    DeviceStatusView View;
    lv_timer_t* refreshTimer;
};
}
