/**
 * @file    DeviceStatusModel.h
 * @brief   设备状态页数据模型定义
 * @details 本文件定义 EasyGo 的设备状态页数据模型定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "App/Services/CareGoClient.h"

namespace Page
{
class DeviceStatusModel
{
public:
    void Init() { client.Init("DeviceStatusModel"); }
    void Deinit() {}
    AccountSystem::CareGo_State_t State() const { return client.State(); }

private:
    CareGoClient client;
};
}
