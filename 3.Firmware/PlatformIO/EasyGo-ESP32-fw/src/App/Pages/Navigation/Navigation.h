/**
 * @file    Navigation.h
 * @brief   导航页定义
 * @details 本文件定义 EasyGo 的导航页定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

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
    static void onRefresh(lv_timer_t* timer);
    void Attach(lv_obj_t* object);
    NavigationView View;
    NavigationModel Model;
    int zoomLevel;
    lv_timer_t* refreshTimer;
};
}
