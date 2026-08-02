/**
 * @file    DeviceStatus.cpp
 * @brief   设备状态页逻辑实现
 * @details 本文件实现 EasyGo 的设备状态页逻辑实现，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "DeviceStatus.h"

using namespace Page;

DeviceStatus::DeviceStatus() : refreshTimer(nullptr) {}
DeviceStatus::~DeviceStatus() {}

void DeviceStatus::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_RIGHT, 260);
}

void DeviceStatus::onViewLoad()
{
    Model.Init();
    View.Create(root);
    View.Update(Model.State());
    lv_obj_add_event_cb(View.ui.back, onBack, LV_EVENT_SHORT_CLICKED, this);
    refreshTimer = lv_timer_create(onRefresh, 500, this);
}

void DeviceStatus::onBack(lv_event_t* event)
{
    DeviceStatus* instance = static_cast<DeviceStatus*>(lv_event_get_user_data(event));
    instance->Manager->Pop();
}

void DeviceStatus::onRefresh(lv_timer_t* timer)
{
    DeviceStatus* instance = static_cast<DeviceStatus*>(timer->user_data);
    if (!instance || instance->refreshTimer != timer || !instance->root ||
        !lv_obj_is_valid(instance->root)) return;
    instance->View.Update(instance->Model.State());
}

void DeviceStatus::onViewDidLoad() {}
void DeviceStatus::onViewWillAppear() {}
void DeviceStatus::onViewDidAppear() {}
void DeviceStatus::onViewWillDisappear()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
}
void DeviceStatus::onViewDidDisappear() {}
void DeviceStatus::onViewUnload() {}
void DeviceStatus::onViewDidUnload()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
    View.Delete();
    Model.Deinit();
}
