#pragma once

#include "NavigationView.h"
#include "NavigationModel.h"

namespace Page
{
class Navigation : public PageBase
{
public:
    Navigation();
    virtual ~Navigation();
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
    NavigationView View;
    NavigationModel Model;
    int zoomLevel;
};
}
