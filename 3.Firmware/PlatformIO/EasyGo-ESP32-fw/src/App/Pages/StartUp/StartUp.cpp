#include "StartUp.h"

using namespace Page;

Startup::Startup() : timer(nullptr) {}
Startup::~Startup() {}

void Startup::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void Startup::onViewLoad()
{
    Model.Init();
    View.Create(root);
    timer = lv_timer_create(onTimer, 2200, this);
    lv_timer_set_repeat_count(timer, 1);
}

void Startup::onTimer(lv_timer_t* timer)
{
    Startup* instance = static_cast<Startup*>(timer->user_data);
    instance->timer = nullptr;
    instance->Manager->Push("Pages/Home");
}

void Startup::onViewDidLoad() {}
void Startup::onViewWillAppear()
{
    lv_anim_timeline_start(View.ui.anim_timeline);
}
void Startup::onViewDidAppear() {}
void Startup::onViewWillDisappear() {}
void Startup::onViewDidDisappear() {}
void Startup::onViewUnload() {}
void Startup::onViewDidUnload()
{
    if (timer != nullptr)
    {
        lv_timer_del(timer);
        timer = nullptr;
    }
    View.Delete();
    Model.DeInit();
}
