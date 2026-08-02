#include "Setting.h"

using namespace Page;

Setting::Setting() : refreshTimer(nullptr) {}
Setting::~Setting() {}

void Setting::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_RIGHT, 260);
}

void Setting::onViewLoad()
{
    Model.Init();
    View.Create(root, Model.VoiceEnabled(), Model.Brightness());
    lv_obj_add_event_cb(View.ui.voiceSwitch, onEvent, LV_EVENT_SHORT_CLICKED, this);
    lv_obj_add_event_cb(View.ui.homeRow, onEvent, LV_EVENT_SHORT_CLICKED, this);
    lv_obj_add_event_cb(View.ui.diagnosticsRow, onEvent, LV_EVENT_SHORT_CLICKED, this);
    lv_obj_add_event_cb(View.ui.backHome, onEvent, LV_EVENT_SHORT_CLICKED, this);
    View.UpdateState(Model.State());
    refreshTimer = lv_timer_create(onRefresh, 500, this);
}

void Setting::onRefresh(lv_timer_t* timer)
{
    Setting* instance = static_cast<Setting*>(timer->user_data);
    if (!instance || instance->refreshTimer != timer || !instance->root ||
        !lv_obj_is_valid(instance->root)) return;
    instance->View.UpdateState(instance->Model.State());
}

void Setting::onEvent(lv_event_t* event)
{
    Setting* instance = static_cast<Setting*>(lv_event_get_user_data(event));
    lv_obj_t* target = lv_event_get_current_target(event);
    if (target == instance->View.ui.voiceSwitch)
    {
        instance->View.SetVoice(instance->Model.ToggleVoice());
    }
    else if (target == instance->View.ui.backHome)
    {
        instance->Manager->Pop();
    }
    else if (target == instance->View.ui.homeRow)
    {
        instance->Model.SetHomeHere();
        instance->View.UpdateState(instance->Model.State());
    }
    else if (target == instance->View.ui.diagnosticsRow)
    {
        instance->Manager->Push("Pages/DeviceStatus");
    }
}

void Setting::onViewDidLoad() {}
void Setting::onViewWillAppear() {}
void Setting::onViewDidAppear() {}
void Setting::onViewWillDisappear()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
}
void Setting::onViewDidDisappear() {}
void Setting::onViewUnload() {}
void Setting::onViewDidUnload()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
    View.Delete();
    Model.Deinit();
}
