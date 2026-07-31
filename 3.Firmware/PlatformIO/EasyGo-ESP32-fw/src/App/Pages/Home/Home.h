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
