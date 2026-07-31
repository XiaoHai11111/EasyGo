#include "Setting.h"

using namespace Page;

Setting::Setting() {}
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
    lv_obj_add_event_cb(View.ui.backHome, onEvent, LV_EVENT_SHORT_CLICKED, this);
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
}

void Setting::onViewDidLoad() {}
void Setting::onViewWillAppear() {}
void Setting::onViewDidAppear() {}
void Setting::onViewWillDisappear() {}
void Setting::onViewDidDisappear() {}
void Setting::onViewUnload() {}
void Setting::onViewDidUnload()
{
    View.Delete();
    Model.Deinit();
}
