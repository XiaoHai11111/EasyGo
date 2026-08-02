/**
 * @file    Family.h
 * @brief   亲情联系人页定义
 * @details 本文件定义 EasyGo 的亲情联系人页定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

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
