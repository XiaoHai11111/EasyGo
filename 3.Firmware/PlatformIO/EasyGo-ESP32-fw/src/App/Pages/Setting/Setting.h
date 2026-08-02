#pragma once

#include "SettingView.h"
#include "SettingModel.h"

namespace Page
{
class Setting : public PageBase
{
public:
    Setting();
    virtual ~Setting();
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
    static void onRefresh(lv_timer_t* timer);
    SettingView View;
    SettingModel Model;
    lv_timer_t* refreshTimer;
};
}
