/**
 * @file    EasyGoUi.h
 * @brief   EasyGo 通用 UI 组件与字体声明
 * @details 本文件定义 EasyGo 的EasyGo 通用 UI 组件与字体声明，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "../Page.h"
#include "App/Accounts/Account_Master.h"
#include "App/Configs/Config.h"
#include <string.h>

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

inline lv_obj_t* Panel(lv_obj_t* parent, lv_coord_t x, lv_coord_t y,
                       lv_coord_t width, lv_coord_t height, lv_color_t color,
                       lv_coord_t radius);

struct StatusBarContext
{
    lv_obj_t* bars[4];
    lv_obj_t* gps;
    lv_obj_t* clock;
    lv_obj_t* sd;
    lv_timer_t* timer;
};

inline void StatusBarRefresh(StatusBarContext* context)
{
    if (!context || !context->clock || !lv_obj_is_valid(context->clock)) return;
    AccountSystem::CareGo_State_t state = {};
    AccountSystem::Broker()->AccountMaster.Pull("CareGo", &state, sizeof(state));

    uint8_t level = 0;
    if (state.network.registered)
    {
        level = 1;
        if (state.network.signalDbm > -100) level = 2;
        if (state.network.signalDbm > -90) level = 3;
        if (state.network.signalDbm > -80) level = 4;
    }
    for (uint8_t index = 0; index < 4; ++index)
    {
        lv_obj_set_style_bg_color(context->bars[index], index < level ? Green : lv_color_hex(0xC8CDD3), 0);
        lv_obj_set_style_bg_opa(context->bars[index], index < level ? LV_OPA_COVER : LV_OPA_50, 0);
    }

    if (state.location.valid)
    {
        lv_label_set_text(context->gps,
            state.location.source == AccountSystem::LOCATION_SOURCE_LBS ? "LBS" : LV_SYMBOL_GPS);
        lv_obj_clear_flag(context->gps, LV_OBJ_FLAG_HIDDEN);
    }
    else lv_obj_add_flag(context->gps, LV_OBJ_FLAG_HIDDEN);
    if (state.sdPresent) lv_obj_clear_flag(context->sd, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(context->sd, LV_OBJ_FLAG_HIDDEN);

    bool valid = false;
    int seconds = 0;
    uint32_t updatedAt = 0;
    if (state.network.timeValid)
    {
        valid = true;
        seconds = state.network.localHour * 3600 + state.network.localMinute * 60 + state.network.localSecond;
        updatedAt = state.network.clockUpdatedAtMs;
    }
    else if (state.location.timeValid)
    {
        valid = true;
        seconds = state.location.utcHour * 3600 + state.location.utcMinute * 60 +
            state.location.utcSecond + CONFIG_TIMEZONE_OFFSET_MINUTES * 60;
        updatedAt = state.location.clockUpdatedAtMs;
    }
    if (valid)
    {
        seconds = (seconds + static_cast<int>((lv_tick_get() - updatedAt) / 1000U)) % 86400;
        if (seconds < 0) seconds += 86400;
        lv_label_set_text_fmt(context->clock, "%02d:%02d", seconds / 3600, (seconds / 60) % 60);
    }
    else lv_label_set_text(context->clock, "--:--");
}

inline void StatusBarTimer(lv_timer_t* timer)
{
    StatusBarRefresh(static_cast<StatusBarContext*>(timer->user_data));
}

inline void StatusBarDeleted(lv_event_t* event)
{
    StatusBarContext* context = static_cast<StatusBarContext*>(lv_event_get_user_data(event));
    if (!context) return;
    if (context->timer) lv_timer_del(context->timer);
    context->timer = nullptr;
    delete context;
}

inline void StatusBar(lv_obj_t* parent)
{
    StatusBarContext* context = new StatusBarContext();
    memset(context, 0, sizeof(*context));
    for (uint8_t index = 0; index < 4; ++index)
    {
        const lv_coord_t height = static_cast<lv_coord_t>(4 + index * 3);
        context->bars[index] = Panel(parent, static_cast<lv_coord_t>(9 + index * 5),
            static_cast<lv_coord_t>(18 - height), 3, height, lv_color_hex(0xC8CDD3), 1);
    }
    context->gps = Symbol(parent, LV_SYMBOL_GPS, Blue, 31, 4, &lv_font_montserrat_14);
    context->clock = Label(parent, "--:--", &lv_font_montserrat_16, Ink, 91, 5, 58);
    context->sd = Panel(parent, 175, 4, 22, 16, lv_color_white(), 2);
    lv_obj_set_style_border_color(context->sd, Blue, 0);
    lv_obj_set_style_border_width(context->sd, 1, 0);
    Label(context->sd, "SD", &lv_font_montserrat_10, Blue, 1, 2, 20);
    Symbol(parent, LV_SYMBOL_BATTERY_FULL, Green, 205, 5, &lv_font_montserrat_16);
    context->timer = lv_timer_create(StatusBarTimer, 1000, context);
    lv_obj_add_event_cb(parent, StatusBarDeleted, LV_EVENT_DELETE, context);
    StatusBarRefresh(context);
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

inline lv_obj_t* LocationPin(lv_obj_t* parent, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_color_t holeColor)
{
    lv_obj_t* holder = lv_obj_create(parent);
    lv_obj_remove_style_all(holder);
    lv_obj_set_pos(holder, x, y);
    lv_obj_set_size(holder, 42, 43);
    lv_obj_clear_flag(holder, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    Panel(holder, 13, 24, 16, 4, color, 1);
    Panel(holder, 15, 28, 12, 4, color, 1);
    Panel(holder, 17, 32, 8, 4, color, 1);
    Panel(holder, 19, 36, 4, 4, color, 1);
    lv_obj_t* head = Panel(holder, 5, 0, 32, 32, color, LV_RADIUS_CIRCLE);
    Panel(head, 10, 10, 12, 12, holeColor, LV_RADIUS_CIRCLE);
    return holder;
}
}
