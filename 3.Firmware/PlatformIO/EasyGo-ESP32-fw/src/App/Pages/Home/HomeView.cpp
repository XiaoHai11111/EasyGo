/**
 * @file    HomeView.cpp
 * @brief   首页视图实现
 * @details 本文件实现 EasyGo 的首页视图实现，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "HomeView.h"

using namespace Page;

void HomeView::CreateLocationIcon(lv_obj_t* parent, lv_color_t color)
{
    EasyGoUi::LocationPin(parent, 2, 3, color, lv_color_white());
}

void HomeView::CreateFamilyIcon(lv_obj_t* parent, lv_color_t color)
{
    lv_obj_t* bodyLeft = EasyGoUi::Panel(parent, 7, 26, 21, 14, color, 8);
    lv_obj_t* bodyRight = EasyGoUi::Panel(parent, 22, 26, 18, 14, color, 8);
    (void)bodyLeft;
    (void)bodyRight;
    EasyGoUi::Panel(parent, 10, 9, 15, 15, color, LV_RADIUS_CIRCLE);
    EasyGoUi::Panel(parent, 25, 12, 12, 12, color, LV_RADIUS_CIRCLE);
}

lv_obj_t* HomeView::CreateCard(lv_obj_t* root, lv_coord_t y, lv_color_t color,
                               int iconType, const char* title)
{
    lv_obj_t* card = EasyGoUi::Panel(root, 12, y, 216, 62, color, 14);
    EasyGoUi::CardShadow(card);
    EasyGoUi::Pressable(card);

    lv_obj_t* icon = EasyGoUi::Panel(card, 10, 8, 46, 46, lv_color_white(), 12);
    lv_obj_set_style_bg_opa(icon, LV_OPA_90, 0);
    if (iconType == 0) CreateLocationIcon(icon, color);
    else if (iconType == 1) CreateFamilyIcon(icon, color);
    else
    {
        lv_obj_t* iconLabel = EasyGoUi::Symbol(icon, LV_SYMBOL_SETTINGS, color, 0, 10, &lv_font_montserrat_28);
        lv_obj_set_width(iconLabel, 46);
        lv_obj_set_style_text_align(iconLabel, LV_TEXT_ALIGN_CENTER, 0);
    }

    EasyGoUi::Label(card, title, &font_easygo_20, lv_color_white(), 69, 15);
    EasyGoUi::Symbol(card, LV_SYMBOL_RIGHT, lv_color_white(), 190, 19, &lv_font_montserrat_24);
    return card;
}

void HomeView::Create(lv_obj_t* root)
{
    EasyGoUi::Screen(root, lv_color_hex(0xFAFBFC));
    EasyGoUi::StatusBar(root);

    EasyGoUi::Label(root, "寻厕宝", &font_easygo_28, EasyGoUi::Navy, 0, 31, 240);
    EasyGoUi::Label(root, "安心出行  方便随行", &font_easygo_16, EasyGoUi::Navy, 0, 66, 240);

    ui.navigation = CreateCard(root, 91, EasyGoUi::Green, 0, "寻厕引路");
    ui.family = CreateCard(root, 160, EasyGoUi::Blue, 1, "亲情守护");
    ui.setting = CreateCard(root, 229, EasyGoUi::Orange, 2, "设置");
}
