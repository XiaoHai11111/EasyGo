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
