#include "Ctrl.h"

using namespace Page;
static bool btnRec_flag = false;
static bool btnPlay_flag = false;
Ctrl::Ctrl()
{
}

Ctrl::~Ctrl()
{

}

void Ctrl::onCustomAttrConfig()
{

}

void Ctrl::onViewLoad()
{
	View.Creat(root);

	AttachEvent(root);
	AttachEvent(View.ui.btnCont.btnBack);
	AttachEvent(View.ui.btnCont.btnReset);
	AttachEvent(View.ui.btnCont.btnRecoder);
	AttachEvent(View.ui.btnCont.btnPlay);

	Model.TickSave = Model.GetData();
}

void Ctrl::onViewDidLoad()
{

}

void Ctrl::onViewWillAppear()
{
	Update();
	View.AppearAnimStart();
}

void Ctrl::onViewDidAppear()
{
	timer = lv_timer_create(onTimerUpdate, 100, this);
}

void Ctrl::onViewWillDisappear()
{
	lv_obj_fade_out(root, 300, 0);
}

void Ctrl::onViewDidDisappear()
{
	lv_timer_del(timer);
}

void Ctrl::onViewDidUnload()
{

}

void Ctrl::AttachEvent(lv_obj_t* obj)
{
	lv_obj_set_user_data(obj, this);
	lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void Ctrl::Update()
{
	lv_label_set_text_fmt(View.ui.valueLabel.J1value_label,"%d", lv_slider_get_value(View.ui.jointsSlider.J1_Slider));
	lv_label_set_text_fmt(View.ui.valueLabel.J2value_label,"%d", lv_slider_get_value(View.ui.jointsSlider.J2_Slider));
	lv_label_set_text_fmt(View.ui.valueLabel.J3value_label,"%d", lv_slider_get_value(View.ui.jointsSlider.J3_Slider));
	lv_label_set_text_fmt(View.ui.valueLabel.J4value_label,"%d", lv_slider_get_value(View.ui.jointsSlider.J4_Slider));
	lv_label_set_text_fmt(View.ui.valueLabel.J5value_label,"%d", lv_slider_get_value(View.ui.jointsSlider.J5_Slider));
	lv_label_set_text_fmt(View.ui.valueLabel.J6value_label,"%d", lv_slider_get_value(View.ui.jointsSlider.J6_Slider));
}

void Ctrl::onTimerUpdate(lv_timer_t* timer)
{
	Ctrl* instance = (Ctrl*)timer->user_data;

	instance->Update();
}

void Ctrl::onBtnClicked(lv_obj_t* btn)
{
	if (btn == View.ui.btnCont.btnBack)
	{
		Manager->Pop();
	}
	else if (btn == View.ui.btnCont.btnReset)
	{
		lv_slider_set_value(View.ui.jointsSlider.J1_Slider, 0, LV_ANIM_OFF);
		lv_slider_set_value(View.ui.jointsSlider.J2_Slider, 0, LV_ANIM_OFF);
		lv_slider_set_value(View.ui.jointsSlider.J3_Slider, 90, LV_ANIM_OFF);
		lv_slider_set_value(View.ui.jointsSlider.J4_Slider, 0, LV_ANIM_OFF);
		lv_slider_set_value(View.ui.jointsSlider.J5_Slider, 0, LV_ANIM_OFF);
		lv_slider_set_value(View.ui.jointsSlider.J6_Slider, 0, LV_ANIM_OFF);
	}
	else if (btn == View.ui.btnCont.btnRecoder)
	{
		btnRec_flag = !btnRec_flag;
		if (btnRec_flag)
		{
			lv_obj_set_style_bg_img_src(btn, Resource.GetImage("pause"), 0);
		}
		else
		{
			lv_obj_set_style_bg_img_src(btn, Resource.GetImage("download"), 0);
		}
	}
	else if (btn == View.ui.btnCont.btnPlay)
	{
		btnPlay_flag = !btnPlay_flag;
		if (btnPlay_flag)
		{
			lv_obj_set_style_bg_img_src(btn, Resource.GetImage("pause"), 0);
		}
		else
		{
			lv_obj_set_style_bg_img_src(btn, Resource.GetImage("start"), 0);
		}
	}
}

void Ctrl::onEvent(lv_event_t* event)
{
	lv_obj_t* obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto* instance = (Ctrl*)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_SHORT_CLICKED)
	{
		instance->onBtnClicked(obj);
	}
}
