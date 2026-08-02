/**
 * @file    TemplateModel.h
 * @brief   新页面模板数据模型定义
 * @details 本文件定义 EasyGo 的新页面模板数据模型定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#ifndef __TEMPLATE_MODEL_H
#define __TEMPLATE_MODEL_H

#include "lvgl.h"

namespace Page
{

class TemplateModel
{
public:
    uint32_t TickSave;
    uint32_t GetData();
private:

};

}

#endif
