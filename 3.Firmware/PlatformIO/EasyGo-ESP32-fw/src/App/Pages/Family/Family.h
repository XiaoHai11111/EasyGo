#pragma once

#include "FamilyView.h"
#include "FamilyModel.h"

namespace Page
{
class Family : public PageBase
{
public:
    Family();
    virtual ~Family();
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
    void Attach(lv_obj_t* object);
    FamilyView View;
    FamilyModel Model;
    lv_timer_t* refreshTimer;
};
}
