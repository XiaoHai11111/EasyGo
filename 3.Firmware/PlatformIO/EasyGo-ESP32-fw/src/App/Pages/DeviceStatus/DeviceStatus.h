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
