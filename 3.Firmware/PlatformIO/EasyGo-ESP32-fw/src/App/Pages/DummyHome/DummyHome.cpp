#include "DummyHome.h"

using namespace Page;

DummyHome::DummyHome():btn_index(0)
{
}

DummyHome::~DummyHome()
{
}

void DummyHome::onCustomAttrConfig()
{
	SetCustomCacheEnable(false);
	SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
	//SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_BOTTOM, 500, lv_anim_path_bounce);
}

void DummyHome::onViewLoad()
{
	Model.Init();
	View.Creat(root);

	AttachEvent(View.ui.btnCont.btnCtrl);
	AttachEvent(View.ui.btnCont.btnTeach);
	AttachEvent(View.ui.btnCont.btnMenu);
}

void DummyHome::onViewDidLoad()
{
	
}

void DummyHome::onViewWillAppear()
{
	switch (btn_index)
	{
	case 1:
		lv_group_focus_obj(View.ui.btnCont.btnCtrl);
		break;
	case 2:lv_group_focus_obj(View.ui.btnCont.btnTeach);
		break;
	case 3:lv_group_focus_obj(View.ui.btnCont.btnMenu);
		break;
	default:
		lv_group_focus_obj(View.ui.btnCont.btnCtrl);
		break;
	}
	View.AppearAnimStart();
}

void DummyHome::onViewDidAppear()
{
	timer = lv_timer_create(onTimerUpdate, 1000, this);
}

void DummyHome::onViewWillDisappear()
{
	lv_timer_del(timer);
}

void DummyHome::onViewDidDisappear()
{
}

void DummyHome::onViewUnload()
{

}

void DummyHome::onViewDidUnload()
{
	View.Delete();
	Model.Deinit();
}
void DummyHome::AttachEvent(lv_obj_t* obj)
{
	lv_obj_set_user_data(obj, this);
	lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void DummyHome::onTimerUpdate(lv_timer_t* timer)
{
	DummyHome* instance = (DummyHome*)timer->user_data;
}

void DummyHome::onBtnClicked(lv_obj_t* btn)
{
	if (btn == View.ui.btnCont.btnCtrl)
	{
		Manager->Push("Pages/Ctrl");
		btn_index = 1;
	}
	else if (btn == View.ui.btnCont.btnMenu)
	{
		Manager->Push("Pages/SystemInfos");
		btn_index = 3;
	}
	else if (btn == View.ui.btnCont.btnTeach)
	{
		btn_index = 2;
		Manager->Push("Pages/Teach");
	}
}

void DummyHome::onEvent(lv_event_t* event)
{
	DummyHome* instance = (DummyHome*)lv_event_get_user_data(event);
	lv_obj_t* obj = lv_event_get_current_target(event);
	lv_event_code_t code = lv_event_get_code(event);

	if (code == LV_EVENT_SHORT_CLICKED)
	{
		instance->onBtnClicked(obj);
	}
}
