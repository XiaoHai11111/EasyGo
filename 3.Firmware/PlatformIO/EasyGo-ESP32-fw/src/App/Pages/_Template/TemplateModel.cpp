/**
 * @file    TemplateModel.cpp
 * @brief   新页面模板数据模型实现
 * @details 本文件实现 EasyGo 的新页面模板数据模型实现，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "TemplateModel.h"

using namespace Page;

uint32_t TemplateModel::GetData()
{
    return lv_tick_get();
}
