/**
 * @file    DeviceStatusView.cpp
 * @brief   设备状态页视图实现
 * @details 本文件实现 EasyGo 的设备状态页视图实现，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "DeviceStatusView.h"
#include "App/Configs/Config.h"

using namespace Page;

lv_obj_t* DeviceStatusView::CreateRow(lv_obj_t* root, lv_coord_t y, const char* title,
                                      const char* symbol, lv_color_t color)
{
    lv_obj_t* row = EasyGoUi::Panel(root, 12, y, 216, 38, lv_color_white(), 10);
    EasyGoUi::CardShadow(row);
    EasyGoUi::Symbol(row, symbol, color, 10, 8, &lv_font_montserrat_20);
    EasyGoUi::Label(row, title, &font_easygo_14, EasyGoUi::Ink, 42, 3);
    return EasyGoUi::Label(row, "--", &font_easygo_12, EasyGoUi::Muted, 42, 20, 162);
}

void DeviceStatusView::Create(lv_obj_t* root)
{
    EasyGoUi::Screen(root, EasyGoUi::Paper);
    EasyGoUi::StatusBar(root);
    EasyGoUi::Label(root, "设备状态", &font_easygo_28, EasyGoUi::Navy, 0, 30, 240);

    ui.gpsValue = CreateRow(root, 68, "定位来源", LV_SYMBOL_GPS, EasyGoUi::Green);
    ui.networkValue = CreateRow(root, 111, "通信链路", LV_SYMBOL_WIFI, EasyGoUi::Blue);
    ui.audioValue = CreateRow(root, 154, "语音模块", LV_SYMBOL_VOLUME_MAX, EasyGoUi::Orange);
    ui.storageValue = CreateRow(root, 197, "SD 卡", LV_SYMBOL_SD_CARD, EasyGoUi::Blue);
    ui.motionValue = CreateRow(root, 240, "IMU / 地磁计", LV_SYMBOL_REFRESH, EasyGoUi::Muted);

#if CONFIG_CELLULAR_ENABLE_EXTERNAL_REQUESTS || CONFIG_CELLULAR_ENABLE_SMS || CONFIG_CELLULAR_ENABLE_LBS
    ui.safetyNote = EasyGoUi::Label(root, "外部通信：已启用", &font_easygo_12,
                                    EasyGoUi::Orange, 12, 281, 216);
#else
    ui.safetyNote = EasyGoUi::Label(root, "外部请求与短信默认关闭", &font_easygo_12,
                                    EasyGoUi::Muted, 12, 281, 216);
#endif
    ui.back = EasyGoUi::Panel(root, 12, 300, 216, 18, EasyGoUi::Navy, 8);
    EasyGoUi::Pressable(ui.back);
    EasyGoUi::Label(ui.back, "返回设置", &font_easygo_14, lv_color_white(), 0, 0, 216);
}

void DeviceStatusView::Update(const AccountSystem::CareGo_State_t& state)
{
    if (state.location.valid)
    {
        if (state.location.source == AccountSystem::LOCATION_SOURCE_LBS)
            lv_label_set_text_fmt(ui.gpsValue, "4G LBS 定位  精度约 %.0f 米",
                                  state.location.horizontalAccuracyMeters);
        else
            lv_label_set_text_fmt(ui.gpsValue, "GPS %u 星  精度约 %.0f 米",
                                  state.location.satellites, state.location.horizontalAccuracyMeters);
    }
    else
    {
        lv_label_set_text(ui.gpsValue, "等待定位数据 (RX36)");
    }

    const char* link = "未连接";
    if (state.network.link == AccountSystem::NETWORK_LINK_WIFI) link = "Wi-Fi";
    else if (state.network.link == AccountSystem::NETWORK_LINK_CELLULAR) link = "4G";
    if (state.network.registered)
        lv_label_set_text_fmt(ui.networkValue, "%s 已注册  %d dBm", link, state.network.signalDbm);
    else
        lv_label_set_text_fmt(ui.networkValue, "%s 等待网络注册", link);

    lv_label_set_text(ui.audioValue, state.audioPlaying ? "DFPlayer 正在播放" : "DFPlayer 已配置 (UART2)");
    lv_label_set_text(ui.storageValue, state.sdPresent ? "DFPlayer TF 卡已插入" : "未检测到 DFPlayer TF 卡");
    lv_label_set_text(ui.motionValue, "待后续硬件接入");
}
