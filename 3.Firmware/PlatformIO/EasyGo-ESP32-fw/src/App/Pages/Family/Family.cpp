#include "Family.h"

using namespace Page;

Family::Family() {}
Family::~Family() {}

void Family::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_RIGHT, 260);
}

void Family::Attach(lv_obj_t* object)
{
    lv_obj_add_event_cb(object, onEvent, LV_EVENT_SHORT_CLICKED, this);
}

void Family::onViewLoad()
{
    Model.Init();
    View.Create(root, Model);
    Attach(View.ui.back);
    for (uint8_t index = 0; index < View.ui.contactCount; ++index) Attach(View.ui.call[index]);
    Attach(View.ui.sos);
}

void Family::onEvent(lv_event_t* event)
{
    Family* instance = static_cast<Family*>(lv_event_get_user_data(event));
    lv_obj_t* target = lv_event_get_current_target(event);
    if (target == instance->View.ui.back)
    {
        instance->Manager->Pop();
        return;
    }
    if (target == instance->View.ui.sos)
    {
        instance->View.ShowFeedback("演示求助，未发送", EasyGoUi::Red);
        return;
    }
    for (uint8_t index = 0; index < instance->View.ui.contactCount; ++index)
    {
        if (target == instance->View.ui.call[index])
        {
            instance->View.ShowFeedback("模拟呼叫，未拨号", EasyGoUi::Blue);
            return;
        }
    }
}

void Family::onViewDidLoad() {}
void Family::onViewWillAppear() {}
void Family::onViewDidAppear() {}
void Family::onViewWillDisappear() {}
void Family::onViewDidDisappear() {}
void Family::onViewUnload() {}
void Family::onViewDidUnload()
{
    View.Delete();
    Model.Deinit();
}
