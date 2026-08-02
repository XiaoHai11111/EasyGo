#include "StartupView.h"

using namespace Page;

void StartupView::Create(lv_obj_t* root)
{
    EasyGoUi::Screen(root, lv_color_hex(0xFFF9EF));

    lv_obj_t* wave = EasyGoUi::Panel(root, -20, -44, 280, 103, lv_color_hex(0xFFF1DB), LV_RADIUS_CIRCLE);
    lv_obj_set_style_border_color(wave, lv_color_white(), 0);
    lv_obj_set_style_border_width(wave, 5, 0);

    EasyGoUi::LocationPin(root, 99, 42, lv_color_hex(0x8CB9F8), lv_color_hex(0xFFF9EF));
    lv_obj_t* shadow = EasyGoUi::Panel(root, 97, 93, 46, 7, lv_color_hex(0xF4E7D2), LV_RADIUS_CIRCLE);
    lv_obj_set_style_bg_opa(shadow, LV_OPA_70, 0);

    ui.logo = EasyGoUi::Panel(root, 24, 108, 192, 66, EasyGoUi::Blue, 16);
    EasyGoUi::CardShadow(ui.logo);
    EasyGoUi::Label(ui.logo, "CAREGO", &lv_font_montserrat_36, lv_color_white(), 10, 8, 172);
    lv_obj_t* underline = EasyGoUi::Panel(ui.logo, 18, 53, 156, 4, EasyGoUi::Red, 2);
    (void)underline;

    EasyGoUi::Label(root, "安心出行  方便随行", &font_easygo_20, EasyGoUi::Blue, 0, 190, 240);
    EasyGoUi::Label(root, "♥", &font_easygo_28, EasyGoUi::Red, 105, 224, 32);
    EasyGoUi::Panel(root, 99, 222, 3, 9, lv_color_hex(0xF8C06A), 2);
    EasyGoUi::Panel(root, 139, 222, 3, 9, lv_color_hex(0xF8C06A), 2);
    EasyGoUi::Panel(root, 119, 216, 3, 9, lv_color_hex(0xF8C06A), 2);

    lv_obj_t* loading = EasyGoUi::Panel(root, 52, 272, 136, 34, lv_color_hex(0xE9F1FC), 17);
    ui.spinner = lv_spinner_create(loading, 900, 90);
    lv_obj_set_size(ui.spinner, 21, 21);
    lv_obj_set_pos(ui.spinner, 10, 6);
    lv_obj_set_style_arc_color(ui.spinner, EasyGoUi::Blue, LV_PART_INDICATOR);
    EasyGoUi::Label(loading, "正在启动...", &font_easygo_16, EasyGoUi::Blue, 40, 7);

    ui.anim_timeline = lv_anim_timeline_create();
#define STARTUP_ANIM(start_time, object, property, start_value, end_value) \
    { start_time, object, LV_ANIM_EXEC(property), start_value, end_value, 500, lv_anim_path_ease_out, true }
    lv_anim_timeline_wrapper_t wrapper[] =
    {
        STARTUP_ANIM(0, ui.logo, width, 0, 192),
        LV_ANIM_TIMELINE_WRAPPER_END
    };
    lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);
}

void StartupView::Delete()
{
    if (ui.anim_timeline != nullptr)
    {
        lv_anim_timeline_del(ui.anim_timeline);
        ui.anim_timeline = nullptr;
    }
}
