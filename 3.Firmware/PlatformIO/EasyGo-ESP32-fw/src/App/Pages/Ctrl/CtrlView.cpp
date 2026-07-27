#include "CtrlView.h"

using namespace Page;

#define COLOR_RED lv_color_hex(0xff0000)

void CtrlView::Creat(lv_obj_t* root)
{
	lv_obj_t* img = lv_img_create(root);
	lv_obj_center(img);
	ui.canvas = img;

	BtnCont_Create(root);
	CtrlLabelCont_Create(root);
	CtrlSliderCont_Create(root);
	ValueLabelCont_Create(root);

	ui.group = lv_group_create();
	lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), ui.group);

	lv_group_add_obj(ui.group, ui.jointsSlider.J1_Slider);
	lv_group_add_obj(ui.group, ui.jointsSlider.J2_Slider);
	lv_group_add_obj(ui.group, ui.jointsSlider.J3_Slider);
	lv_group_add_obj(ui.group, ui.jointsSlider.J4_Slider);
	lv_group_add_obj(ui.group, ui.jointsSlider.J5_Slider);
	lv_group_add_obj(ui.group, ui.jointsSlider.J6_Slider);
	lv_group_add_obj(ui.group, ui.btnCont.btnBack);
	lv_group_add_obj(ui.group, ui.btnCont.btnReset);
	lv_group_add_obj(ui.group, ui.btnCont.btnRecoder);
	lv_group_add_obj(ui.group, ui.btnCont.btnPlay);
	lv_group_focus_obj(ui.btnCont.btnReset);


	ui.anim_timeline = lv_anim_timeline_create();

#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 800, lv_anim_path_ease_out, true}

	lv_coord_t h_tar_btn = lv_obj_get_height(ui.btnCont.btnBack);
	lv_coord_t h_cont = lv_obj_get_height(ui.jointsSlider.cont);

	lv_anim_timeline_wrapper_t wrapper[] =
	{
		ANIM_DEF(400, ui.btnCont.btnBack, height, 0, h_tar_btn),
		ANIM_DEF(600, ui.btnCont.btnReset, height, 0, h_tar_btn),
		ANIM_DEF(400, ui.btnCont.btnRecoder, height, 0, h_tar_btn),
		ANIM_DEF(600, ui.btnCont.btnPlay, height, 0, h_tar_btn),
		LV_ANIM_TIMELINE_WRAPPER_END
	};
	lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);
}

void CtrlView::CtrlLabelCont_Create(lv_obj_t* par)
{
	lv_obj_t* cont = lv_obj_create(par);
	lv_obj_set_size(cont, 30, 160);
	lv_obj_align_to(cont, par, LV_ALIGN_CENTER, -100, 35);
	ui.jointsLabel.cont = cont;

	ui.jointsLabel.J1_label = CtrlLabel_Create(cont, "J1:",  -65);
	ui.jointsLabel.J2_label = CtrlLabel_Create(cont, "J2:",  -40);
	ui.jointsLabel.J3_label = CtrlLabel_Create(cont, "J3:",  -15);
	ui.jointsLabel.J4_label = CtrlLabel_Create(cont, "J4:",  10);
	ui.jointsLabel.J5_label = CtrlLabel_Create(cont, "J5:",  35);
	ui.jointsLabel.J6_label = CtrlLabel_Create(cont, "J6:",  60);
}
lv_obj_t* CtrlView::CtrlLabel_Create(lv_obj_t* par, const char* text,lv_coord_t y_ofs)
{
	lv_obj_t* label = lv_label_create(par);
	lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_17"), 0);
	lv_obj_set_size(label, 40, 20);
	lv_obj_set_style_text_color(label, lv_color_white(), 0);
	lv_obj_align(label, LV_ALIGN_CENTER, 10, y_ofs);
	lv_label_set_text(label, text);
	
	return label;
}

void CtrlView::ValueLabelCont_Create(lv_obj_t* par)
{
	lv_obj_t* cont = lv_obj_create(par);
	lv_obj_set_size(cont, 60, 160);
	lv_obj_align_to(cont, par, LV_ALIGN_CENTER, -70, 35);
	ui.jointsLabel.cont = cont;

	ui.valueLabel.J1value_label = ValueLabel_Create(cont, "0", -65);
	ui.valueLabel.J2value_label = ValueLabel_Create(cont, "0", -40);
	ui.valueLabel.J3value_label = ValueLabel_Create(cont, "90", -15);
	ui.valueLabel.J4value_label = ValueLabel_Create(cont, "0", 10);
	ui.valueLabel.J5value_label = ValueLabel_Create(cont, "0", 35);
	ui.valueLabel.J6value_label = ValueLabel_Create(cont, "0", 60);
}
lv_obj_t* CtrlView::ValueLabel_Create(lv_obj_t* par, const char* text, lv_coord_t y_ofs)
{
	lv_obj_t* label = lv_label_create(par);
	lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
	lv_obj_set_style_bg_color(label, lv_color_white(), 0);
	lv_obj_set_style_radius(label, 0x0003, 0);

	lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_13"), 0);
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_size(label, 40, 15);
	lv_obj_align(label, LV_ALIGN_CENTER, 10, y_ofs);
	lv_label_set_text(label, text);

	return label;
}

void CtrlView::CtrlSliderCont_Create(lv_obj_t* par)
{
	lv_obj_t* cont = lv_obj_create(par);
	lv_obj_set_size(cont, 160, 160);
	lv_obj_align_to(cont, par, LV_ALIGN_CENTER, 40, 35);
	ui.jointsSlider.cont = cont;

	ui.jointsSlider.J1_Slider = CtrlSlider_Create(cont, -170, 0, 170, -65);
	ui.jointsSlider.J2_Slider = CtrlSlider_Create(cont, -75, 0, 90, -40);
	ui.jointsSlider.J3_Slider = CtrlSlider_Create(cont, 35, 90, 180, -15);
	ui.jointsSlider.J4_Slider = CtrlSlider_Create(cont, -180, 0, 180, 10);
	ui.jointsSlider.J5_Slider = CtrlSlider_Create(cont, -120, 0, 120, 35);
	ui.jointsSlider.J6_Slider = CtrlSlider_Create(cont, -360, 0, 360, 60);
}
lv_obj_t* CtrlView::CtrlSlider_Create(lv_obj_t* par,int32_t left_value, int32_t cur_value, int32_t right_value, lv_coord_t y_ofs)
{
	lv_obj_t* slider = lv_slider_create(par);

	lv_obj_remove_style_all(slider);
	lv_obj_clear_flag(slider, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, 0);

	lv_obj_set_size(slider, 130, 10);
	lv_slider_set_range(slider, left_value, right_value);
	lv_slider_set_value(slider, cur_value, LV_ANIM_OFF);
	lv_obj_align(slider, LV_ALIGN_CENTER, 0, y_ofs);

	static lv_style_t style_main;
	static lv_style_t style_indicator;
	static lv_style_t style_knob;
	static lv_style_t style_pressed_color;

	lv_style_init(&style_main);
	lv_style_set_bg_opa(&style_main, LV_OPA_COVER);
	lv_style_set_bg_color(&style_main, lv_color_hex(0xbbbbbb));
	lv_style_set_radius(&style_main, LV_RADIUS_CIRCLE);

	lv_style_init(&style_indicator);
	lv_style_set_bg_opa(&style_indicator, LV_OPA_COVER);
	lv_style_set_bg_color(&style_indicator, lv_color_white());
	lv_style_set_radius(&style_indicator, LV_RADIUS_CIRCLE);

	lv_style_init(&style_knob);
	lv_style_set_bg_opa(&style_knob, LV_OPA_COVER);
	lv_style_set_bg_color(&style_knob, lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_radius(&style_knob, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(&style_knob, 3);

	lv_style_init(&style_pressed_color);
	lv_style_set_bg_color(&style_pressed_color, lv_color_white());

	lv_obj_add_style(slider, &style_main, LV_PART_MAIN);
	lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);
	lv_obj_add_style(slider, &style_knob, LV_PART_KNOB);
	lv_obj_add_style(slider, &style_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);
	lv_obj_add_style(slider, &style_pressed_color, LV_PART_KNOB | LV_STATE_FOCUSED);

	return slider;
}

void CtrlView::BtnCont_Create(lv_obj_t* par)
{
	lv_obj_t* cont = lv_obj_create(par);
	lv_obj_remove_style_all(cont);
	lv_obj_set_size(cont, LV_HOR_RES, 40);
	lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 30);
	ui.btnCont.cont = cont;

	ui.btnCont.btnBack = Btn_Create(cont, Resource.GetImage("back"), -90);
	ui.btnCont.btnReset = Btn_Create(cont, Resource.GetImage("reset"), -30);
	ui.btnCont.btnRecoder = Btn_Create(cont, Resource.GetImage("download"), 30);
	ui.btnCont.btnPlay = Btn_Create(cont, Resource.GetImage("start"),  90);
}

lv_obj_t* CtrlView::Btn_Create(lv_obj_t* par, const void* img1_src, lv_coord_t x_ofs)
{
	lv_obj_t* obj = lv_obj_create(par);
	lv_obj_remove_style_all(obj);
	lv_obj_set_size(obj, 45, 31);
	lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_align(obj, LV_ALIGN_CENTER, x_ofs, 0);
	lv_obj_set_style_bg_img_src(obj, img1_src, 0);

	lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
	lv_obj_set_style_width(obj, 40, LV_STATE_PRESSED);
	lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);
	lv_obj_set_style_bg_color(obj, lv_color_hex(0x666666), 0);
	lv_obj_set_style_bg_color(obj, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
	lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0000), LV_STATE_FOCUSED);
	//lv_obj_set_style_bg_img_src(obj, img2_src, LV_STATE_CHECKED);
	//lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE);
	lv_obj_set_style_radius(obj, 9, 0);

	static lv_style_transition_dsc_t tran;
	static const lv_style_prop_t prop[] = { LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_PROP_INV };
	lv_style_transition_dsc_init(
		&tran,
		prop,
		lv_anim_path_ease_out,
		200,
		0,
		nullptr
	);
	lv_obj_set_style_transition(obj, &tran, LV_STATE_PRESSED);
	lv_obj_set_style_transition(obj, &tran, LV_STATE_FOCUSED);

	lv_obj_update_layout(obj);

	return obj;
}
void CtrlView::AppearAnimStart(bool reverse)
{
	lv_anim_timeline_set_reverse(ui.anim_timeline, reverse);
	lv_anim_timeline_start(ui.anim_timeline);
}