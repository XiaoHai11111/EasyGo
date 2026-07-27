#include "DummyHomeView.h"

using namespace Page;

void DummyHomeView::Creat(lv_obj_t* root)
{
	lv_obj_t* img = lv_img_create(root);
	lv_img_set_src(img, Resource.GetImage("arm"));
	lv_obj_center(img);
    ui.bg_image = img;

    BtnCont_Create(root);

    ui.group = lv_group_create();
    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), ui.group);
    lv_group_add_obj(ui.group, ui.btnCont.btnCtrl);
    lv_group_add_obj(ui.group, ui.btnCont.btnTeach);
    lv_group_add_obj(ui.group, ui.btnCont.btnMenu);
    lv_group_focus_obj(ui.btnCont.btnCtrl);

    ui.anim_timeline = lv_anim_timeline_create();

#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 800, lv_anim_path_ease_out, true}

    lv_coord_t h_tar_btn = lv_obj_get_height(ui.btnCont.btnMenu);
    lv_coord_t h_bg_img = lv_obj_get_height(ui.bg_image);

    lv_anim_timeline_wrapper_t wrapper[] =
    {
        ANIM_DEF(0, ui.bg_image, height, 0, h_bg_img),
        ANIM_DEF(500, ui.btnCont.btnCtrl, height, 0, h_tar_btn),
        ANIM_DEF(600, ui.btnCont.btnTeach, height, 0, h_tar_btn),
        ANIM_DEF(700, ui.btnCont.btnMenu, height, 0, h_tar_btn),
        LV_ANIM_TIMELINE_WRAPPER_END
    };
    lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);
}

void DummyHomeView::BtnCont_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 50, LV_VER_RES);
    lv_obj_align(cont, LV_ALIGN_CENTER, 60, 40);
    ui.btnCont.cont = cont;

    ui.btnCont.btnCtrl = Btn_Create(cont, Resource.GetImage("ctrl"), -40);
    ui.btnCont.btnTeach = Btn_Create(cont, Resource.GetImage("teach"), 0);
    ui.btnCont.btnMenu = Btn_Create(cont, Resource.GetImage("menu"), 40);
}

lv_obj_t* DummyHomeView::Btn_Create(lv_obj_t* par, const void* img_src, lv_coord_t y_ofs)
{
    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 40, 31);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_align(obj, LV_ALIGN_CENTER, 0, y_ofs);
    lv_obj_set_style_bg_img_src(obj, img_src, 0);

    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_width(obj, 45, LV_STATE_PRESSED);
    lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x666666), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xbbbbbb), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0000), LV_STATE_FOCUSED);
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
void DummyHomeView::Delete() 
{
    if (ui.anim_timeline)
    {
        lv_anim_timeline_del(ui.anim_timeline);
        ui.anim_timeline = nullptr;
    }
}
void DummyHomeView::AppearAnimStart(bool reverse)
{
    lv_anim_timeline_set_reverse(ui.anim_timeline, reverse);
    lv_anim_timeline_start(ui.anim_timeline);
}