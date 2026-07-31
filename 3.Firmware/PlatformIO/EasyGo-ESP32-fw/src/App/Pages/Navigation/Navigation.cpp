#include "Navigation.h"

using namespace Page;

Navigation::Navigation() : zoomLevel(NavigationModel::ZoomLevelDefault) {}
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
}

void Navigation::onEvent(lv_event_t* event)
{
    Navigation* instance = static_cast<Navigation*>(lv_event_get_user_data(event));
    lv_obj_t* target = lv_event_get_current_target(event);
    if (target == instance->View.ui.back) instance->Manager->Pop();
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
void Navigation::onViewWillDisappear() {}
void Navigation::onViewDidDisappear() {}
void Navigation::onViewUnload() {}
void Navigation::onViewDidUnload()
{
    View.Delete();
    Model.Deinit();
}
