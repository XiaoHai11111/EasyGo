#include "Navigation.h"

using namespace Page;

Navigation::Navigation() : zoomLevel(NavigationModel::ZoomLevelDefault), refreshTimer(nullptr) {}
Navigation::~Navigation() {}

void Navigation::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_RIGHT, 260);
}

void Navigation::Attach(lv_obj_t* object)
{
    lv_obj_add_event_cb(object, onEvent, LV_EVENT_SHORT_CLICKED, this);
}

void Navigation::onViewLoad()
{
    Model.Init();
    View.Create(root, Model.MapPath(zoomLevel), Model.DistanceMeters(), Model.Minutes());
    Attach(View.ui.back);
    Attach(View.ui.zoomIn);
    Attach(View.ui.zoomOut);
    const AccountSystem::CareGo_Navigation_State_t state = Model.State().navigation;
    if (state == AccountSystem::CAREGO_NAV_IDLE || state == AccountSystem::CAREGO_NAV_FAILED)
        Model.StartToiletNavigation();
    View.UpdateState(Model.State());
    refreshTimer = lv_timer_create(onRefresh, 400, this);
}

void Navigation::onRefresh(lv_timer_t* timer)
{
    Navigation* instance = static_cast<Navigation*>(timer->user_data);
    if (!instance || instance->refreshTimer != timer || !instance->root ||
        !lv_obj_is_valid(instance->root)) return;
    instance->View.UpdateState(instance->Model.State());
}

void Navigation::onEvent(lv_event_t* event)
{
    Navigation* instance = static_cast<Navigation*>(lv_event_get_user_data(event));
    lv_obj_t* target = lv_event_get_current_target(event);
    if (target == instance->View.ui.back)
    {
        instance->Model.StopNavigation();
        instance->Manager->Pop();
    }
    else if (target == instance->View.ui.zoomIn)
    {
        instance->zoomLevel = instance->Model.ClampZoomLevel(instance->zoomLevel + 1);
        instance->View.SetMapSource(instance->Model.MapPath(instance->zoomLevel));
    }
    else if (target == instance->View.ui.zoomOut)
    {
        instance->zoomLevel = instance->Model.ClampZoomLevel(instance->zoomLevel - 1);
        instance->View.SetMapSource(instance->Model.MapPath(instance->zoomLevel));
    }
}

void Navigation::onViewDidLoad() {}
void Navigation::onViewWillAppear() {}
void Navigation::onViewDidAppear() {}
void Navigation::onViewWillDisappear()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
}
void Navigation::onViewDidDisappear() {}
void Navigation::onViewUnload() {}
void Navigation::onViewDidUnload()
{
    if (refreshTimer)
    {
        lv_timer_del(refreshTimer);
        refreshTimer = nullptr;
    }
    View.Delete();
    Model.Deinit();
}
