#include "Home.h"

using namespace Page;

Home::Home() {}
Home::~Home() {}

void Home::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void Home::Attach(lv_obj_t* object)
{
    lv_obj_add_event_cb(object, onEvent, LV_EVENT_SHORT_CLICKED, this);
}

void Home::onViewLoad()
{
    Model.Init();
    View.Create(root);
    Attach(View.ui.navigation);
    Attach(View.ui.family);
    Attach(View.ui.setting);
}

void Home::onEvent(lv_event_t* event)
{
    Home* instance = static_cast<Home*>(lv_event_get_user_data(event));
    lv_obj_t* target = lv_event_get_current_target(event);
    if (target == instance->View.ui.navigation) instance->Manager->Push("Pages/Navigation");
    else if (target == instance->View.ui.family) instance->Manager->Push("Pages/Family");
    else if (target == instance->View.ui.setting) instance->Manager->Push("Pages/Setting");
}

void Home::onViewDidLoad() {}
void Home::onViewWillAppear() {}
void Home::onViewDidAppear() {}
void Home::onViewWillDisappear() {}
void Home::onViewDidDisappear() {}
void Home::onViewUnload() {}
void Home::onViewDidUnload()
{
    View.Delete();
    Model.Deinit();
}
