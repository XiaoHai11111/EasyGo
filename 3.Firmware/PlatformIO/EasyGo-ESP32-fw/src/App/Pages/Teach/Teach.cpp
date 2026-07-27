#include "Teach.h"

using namespace Page;

Teach::Teach()
{
}

Teach::~Teach()
{

}

void Teach::onCustomAttrConfig()
{
	/*SetCustomCacheEnable(true);
	SetCustomLoadAnimType(PageManager::LOAD_ANIM_MOVE_TOP, 500, lv_anim_path_bounce);*/
}

void Teach::onViewLoad()
{
	View.Create(root);
	lv_label_set_text(View.ui.labelTitle, Name);

	AttachEvent(root);
	AttachEvent(View.ui.canvas);

	Model.TickSave = Model.GetData();
}

void Teach::onViewDidLoad()
{

}

void Teach::onViewWillAppear()
{
	Param_t param;
	param.color = lv_color_white();
	param.time = 100;
	PAGE_STASH_POP(param);

	lv_obj_set_style_bg_color(root, param.color, LV_PART_MAIN);

	timer = lv_timer_create(onTimerUpdate, param.time, this);
}

void Teach::onViewDidAppear()
{

}

void Teach::onViewWillDisappear()
{
	lv_obj_fade_out(root, 300, 0);
}

void Teach::onViewDidDisappear()
{
	lv_timer_del(timer);
}

void Teach::onViewDidUnload()
{

}

void Teach::AttachEvent(lv_obj_t* obj)
{
	lv_obj_set_user_data(obj, this);
	lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void Teach::Update()
{
	lv_label_set_text_fmt(View.ui.labelTick, "tick = %d save = %d", Model.GetData(), Model.TickSave);
}

void Teach::onTimerUpdate(lv_timer_t* timer)
{
	Teach* instance = (Teach*)timer->user_data;

	instance->Update();
}

void Teach::onEvent(lv_event_t* event)
{
	lv_obj_t* obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto* instance = (Teach*)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_PRESSED)
	{
		if (lv_obj_has_state(obj, LV_STATE_FOCUSED))
		{
			instance->Manager->Pop();
		}
	}
}
