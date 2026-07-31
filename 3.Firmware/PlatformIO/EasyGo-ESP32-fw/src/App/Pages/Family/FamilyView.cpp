#include "FamilyView.h"

using namespace Page;

lv_obj_t* FamilyView::CreateContact(lv_obj_t* root, int index, const FamilyModel::Contact& contact)
{
    static const uint32_t avatarColors[] =
    {
        0xFF8DA1, 0x72B8F4, 0xFFD166, 0xA78BFA,
        0x60D5A5, 0xFF9F6E, 0x68C8D9, 0xB5BEC9
    };
    uint8_t avatarStyle = contact.avatarStyle % 8;
    lv_coord_t y = static_cast<lv_coord_t>(index * 61);
    lv_obj_t* card = EasyGoUi::Panel(root, 12, y, 216, 54, lv_color_white(), 13);
    EasyGoUi::CardShadow(card);

    lv_obj_t* avatar = EasyGoUi::Panel(
        card, 10, 7, 40, 40, lv_color_hex(avatarColors[avatarStyle]), LV_RADIUS_CIRCLE);
    lv_color_t hair = (avatarStyle % 3 == 1) ? lv_color_hex(0x5D4638) : lv_color_hex(0x3E2B23);
    EasyGoUi::Panel(avatar, 9, 5, 22, 23, hair, LV_RADIUS_CIRCLE);
    lv_obj_t* face = EasyGoUi::Panel(avatar, 11, 8, 18, 19, lv_color_hex(0xFFD2B1), LV_RADIUS_CIRCLE);
    EasyGoUi::Panel(face, 4, 8, 2, 2, EasyGoUi::Ink, LV_RADIUS_CIRCLE);
    EasyGoUi::Panel(face, 12, 8, 2, 2, EasyGoUi::Ink, LV_RADIUS_CIRCLE);
    EasyGoUi::Panel(avatar, 8, 27, 24, 13,
                    (avatarStyle % 3 == 1) ? EasyGoUi::Navy : lv_color_hex(0xE65F7C), 7);
    EasyGoUi::Label(card, contact.name, &font_easygo_20, EasyGoUi::Ink, 62, 5);
    EasyGoUi::Label(card, contact.number, &lv_font_montserrat_14, EasyGoUi::Ink, 62, 31);

    lv_obj_t* call = EasyGoUi::Panel(card, 171, 8, 38, 38, lv_color_hex(0xE9F7EC), LV_RADIUS_CIRCLE);
    EasyGoUi::Pressable(call);
    EasyGoUi::Symbol(call, LV_SYMBOL_CALL, EasyGoUi::Green, 8, 8, &lv_font_montserrat_20);
    return call;
}

void FamilyView::Create(lv_obj_t* root, const FamilyModel& model)
{
    EasyGoUi::Screen(root, lv_color_hex(0xF8F9FB));
    EasyGoUi::StatusBar(root);

    ui.back = EasyGoUi::Panel(root, 8, 34, 29, 27, lv_color_hex(0xEEF2F7), 8);
    EasyGoUi::Pressable(ui.back);
    EasyGoUi::Symbol(ui.back, LV_SYMBOL_LEFT, EasyGoUi::Ink, 5, 4, &lv_font_montserrat_20);
    EasyGoUi::Label(root, "紧急联系人", &font_easygo_20, EasyGoUi::Ink, 0, 37, 240);

    ui.list = lv_obj_create(root);
    lv_obj_remove_style_all(ui.list);
    lv_obj_set_pos(ui.list, 0, 65);
    lv_obj_set_size(ui.list, 240, 180);
    lv_obj_set_style_bg_opa(ui.list, LV_OPA_TRANSP, 0);
    lv_obj_set_scroll_dir(ui.list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(ui.list, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_width(ui.list, 4, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(ui.list, EasyGoUi::Blue, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(ui.list, LV_OPA_60, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(ui.list, 2, LV_PART_SCROLLBAR);

    ui.contactCount = model.GetContactCount();
    for (uint8_t index = 0; index < ui.contactCount; ++index)
    {
        ui.call[index] = CreateContact(ui.list, index, model.GetContact(index));
    }

    ui.sos = EasyGoUi::Panel(root, 12, 252, 216, 55, EasyGoUi::Red, 13);
    EasyGoUi::Pressable(ui.sos);
    lv_obj_t* badge = EasyGoUi::Panel(ui.sos, 12, 9, 38, 38, lv_color_white(), LV_RADIUS_CIRCLE);
    EasyGoUi::Label(badge, "SOS", &lv_font_montserrat_14, EasyGoUi::Red, 2, 11, 34);
    EasyGoUi::Label(ui.sos, "一键求助", &font_easygo_20, lv_color_white(), 75, 12);
    //ui.feedback = EasyGoUi::Label(ui.sos, "演示：发送位置给联系人", &font_easygo_14,
    //                              lv_color_white(), 63, 33);
}

void FamilyView::ShowFeedback(const char* text, lv_color_t color)
{
    lv_label_set_text(ui.feedback, text);
    lv_obj_set_style_bg_color(ui.sos, color, 0);
}
