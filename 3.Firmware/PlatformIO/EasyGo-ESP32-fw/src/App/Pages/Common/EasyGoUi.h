#pragma once

#include "../Page.h"

extern "C"
{
LV_FONT_DECLARE(font_easygo_16)
LV_FONT_DECLARE(font_easygo_20)
LV_FONT_DECLARE(font_easygo_28)
LV_FONT_DECLARE(font_easygo_14)
LV_FONT_DECLARE(font_easygo_12)
}

namespace EasyGoUi
{
static const lv_color_t Navy = lv_color_hex(0x18345C);
static const lv_color_t Blue = lv_color_hex(0x2F80ED);
static const lv_color_t Green = lv_color_hex(0x45B35A);
static const lv_color_t Orange = lv_color_hex(0xF2A316);
static const lv_color_t Red = lv_color_hex(0xFF5158);
static const lv_color_t Ink = lv_color_hex(0x101318);
static const lv_color_t Muted = lv_color_hex(0x7A7F87);
static const lv_color_t Paper = lv_color_hex(0xF7F8FA);

inline void Screen(lv_obj_t* root, lv_color_t color = Paper)
{
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(root, color, 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* background = lv_obj_create(root);
    lv_obj_remove_style_all(background);
    lv_obj_set_pos(background, 0, 0);
    lv_obj_set_size(background, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(background, color, 0);
    lv_obj_set_style_bg_opa(background, LV_OPA_COVER, 0);
    lv_obj_clear_flag(background, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_move_background(background);
}

inline lv_obj_t* Label(lv_obj_t* parent, const char* text, const lv_font_t* font,
                       lv_color_t color, lv_coord_t x, lv_coord_t y,
                       lv_coord_t width = LV_SIZE_CONTENT)
{
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_pos(label, x, y);
    if (width != LV_SIZE_CONTENT)
    {
        lv_obj_set_width(label, width);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    }
    return label;
}

inline lv_obj_t* Symbol(lv_obj_t* parent, const char* text, lv_color_t color,
                        lv_coord_t x, lv_coord_t y, const lv_font_t* font = &lv_font_montserrat_20)
{
    return Label(parent, text, font, color, x, y);
}

inline void StatusBar(lv_obj_t* parent)
{
    Label(parent, "10:30", &lv_font_montserrat_16, Ink, 91, 5, 58);
    Label(parent, "||||", &lv_font_montserrat_12, Ink, 9, 7);
    Symbol(parent, LV_SYMBOL_BATTERY_FULL, Green, 202, 5, &lv_font_montserrat_16);
}

inline lv_obj_t* Panel(lv_obj_t* parent, lv_coord_t x, lv_coord_t y,
                       lv_coord_t width, lv_coord_t height, lv_color_t color,
                       lv_coord_t radius = 14)
{
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_remove_style_all(panel);
    lv_obj_set_pos(panel, x, y);
    lv_obj_set_size(panel, width, height);
    lv_obj_set_style_bg_color(panel, color, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(panel, radius, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    return panel;
}

inline void CardShadow(lv_obj_t* object)
{
    lv_obj_set_style_shadow_color(object, lv_color_hex(0x9AA3AE), 0);
    lv_obj_set_style_shadow_width(object, 8, 0);
    lv_obj_set_style_shadow_opa(object, LV_OPA_20, 0);
    lv_obj_set_style_shadow_ofs_y(object, 2, 0);
}

inline void Pressable(lv_obj_t* object)
{
    lv_obj_add_flag(object, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_transform_zoom(object, 245, LV_STATE_PRESSED);
    lv_obj_set_style_opa(object, LV_OPA_80, LV_STATE_PRESSED);
}
}
