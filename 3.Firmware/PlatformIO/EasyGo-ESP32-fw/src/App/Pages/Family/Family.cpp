#include "Family.h"

using namespace Page;

Family::Family() : refreshTimer(nullptr) {}
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
    refreshTimer = lv_timer_create(onRefresh, 400, this);
}

void Family::onRefresh(lv_timer_t* timer)
{
    Family* instance = static_cast<Family*>(timer->user_data);
    if (!instance || instance->refreshTimer != timer || !instance->root ||
        !lv_obj_is_valid(instance->root) || !lv_obj_is_valid(instance->View.ui.feedback)) return;
    const AccountSystem::CareGo_State_t state = instance->Model.State();
    if (state.sos == AccountSystem::CAREGO_SOS_SENDING)
        instance->View.ShowFeedback("正在发送求助短信", EasyGoUi::Orange);
    else if (state.sos == AccountSystem::CAREGO_SOS_ACCEPTED)
        instance->View.ShowFeedback("短信已由模块受理", EasyGoUi::Green);
    else if (state.sos == AccountSystem::CAREGO_SOS_FAILED ||
             state.sos == AccountSystem::CAREGO_SOS_NOT_CONFIGURED)
        instance->View.ShowFeedback(state.statusText, EasyGoUi::Red);
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
        instance->Model.RequestSos();
        instance->View.ShowFeedback("正在准备求助", EasyGoUi::Orange);
        return;
    }
    for (uint8_t index = 0; index < instance->View.ui.contactCount; ++index)
    {
        if (target == instance->View.ui.call[index])
        {
            if (instance->Model.RequestSosForContact(index))
                instance->View.ShowFeedback("正在准备发送短信", EasyGoUi::Orange);
            else
                instance->View.ShowFeedback("该联系人号码不可发送", EasyGoUi::Red);
            return;
        }
    }
}

void Family::onViewDidLoad() {}
void Family::onViewWillAppear() {}
void Family::onViewDidAppear() {}
void Family::onViewWillDisappear()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
}
void Family::onViewDidDisappear() {}
void Family::onViewUnload() {}
void Family::onViewDidUnload()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
    View.Delete();
    Model.Deinit();
}
