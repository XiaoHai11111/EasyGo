#include "SettingView.h"

using namespace Page;

lv_obj_t* SettingView::CreateRow(lv_obj_t* root, int index, const char* symbol, lv_color_t color,
                                 const char* title, const char* detail, bool arrow)
{
    lv_obj_t* row = EasyGoUi::Panel(root, 12, static_cast<lv_coord_t>(76 + index * 34),
                                    216, 32, lv_color_white(), 10);
    EasyGoUi::CardShadow(row);
    EasyGoUi::Symbol(row, symbol, color, 12, 6, &lv_font_montserrat_20);
    EasyGoUi::Label(row, title, &font_easygo_14, EasyGoUi::Ink, 47, 0);
    if (detail != nullptr) EasyGoUi::Label(row, detail, &font_easygo_12, EasyGoUi::Muted, 47, 16);
    if (arrow) EasyGoUi::Symbol(row, LV_SYMBOL_RIGHT, EasyGoUi::Ink, 192, 6, &lv_font_montserrat_18);
    return row;
}

void SettingView::Create(lv_obj_t* root, bool voiceEnabled, int brightness)
{
    EasyGoUi::Screen(root, lv_color_hex(0xF7F8FA));
    EasyGoUi::StatusBar(root);
    EasyGoUi::Label(root, "设置", &font_easygo_28, EasyGoUi::Navy, 0, 31, 240);

    ui.voiceRow = CreateRow(root, 0, LV_SYMBOL_VOLUME_MAX, EasyGoUi::Blue, "语音播报", "开启后将语音提示", false);
    ui.voiceSwitch = EasyGoUi::Panel(ui.voiceRow, 166, 6, 39, 21,
                                     voiceEnabled ? EasyGoUi::Green : lv_color_hex(0xC8CDD2),
                                     LV_RADIUS_CIRCLE);
    EasyGoUi::Pressable(ui.voiceSwitch);
    ui.voiceKnob = EasyGoUi::Panel(ui.voiceSwitch, voiceEnabled ? 20 : 2, 2, 17, 17,
                                   lv_color_white(), LV_RADIUS_CIRCLE);

    CreateRow(root, 1, LV_SYMBOL_LIST, EasyGoUi::Blue, "语言选择", "当前语言: 简体中文", true);
    (void)brightness;
    CreateRow(root, 2, LV_SYMBOL_EYE_OPEN, EasyGoUi::Orange, "屏幕亮度", "当前亮度: 80%", true);
    ui.homeRow = CreateRow(root, 3, LV_SYMBOL_HOME, EasyGoUi::Green, "家庭位置", "点击将当前位置设为家", true);
    ui.diagnosticsRow = CreateRow(root, 4, LV_SYMBOL_GPS, EasyGoUi::Blue, "设备状态", "GPS、4G、语音与存储", true);
    ui.feedback = EasyGoUi::Label(root, "", &font_easygo_12, EasyGoUi::Muted, 12, 251, 216);

    ui.backHome = EasyGoUi::Panel(root, 12, 284, 216, 31, EasyGoUi::Orange, 10);
    EasyGoUi::Pressable(ui.backHome);
    EasyGoUi::Symbol(ui.backHome, LV_SYMBOL_HOME, lv_color_white(), 49, 6, &lv_font_montserrat_18);
    EasyGoUi::Label(ui.backHome, "返回首页", &font_easygo_20, lv_color_white(), 82, 3);
}

void SettingView::SetVoice(bool enabled)
{
    lv_obj_set_style_bg_color(ui.voiceSwitch,
                              enabled ? EasyGoUi::Green : lv_color_hex(0xC8CDD2), 0);
    lv_obj_set_x(ui.voiceKnob, enabled ? 20 : 2);
}

void SettingView::UpdateState(const AccountSystem::CareGo_State_t& state)
{
    SetVoice(state.voiceEnabled);
    if (ui.feedback) lv_label_set_text(ui.feedback, state.statusText);
}
