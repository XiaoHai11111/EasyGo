/**
 * @file    main.cpp
 * @brief   固件程序入口与系统初始化
 * @details 本文件为固件程序入口，负责 HAL、显示与页面系统初始化并驱动主循环。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include <Arduino.h>
#include "HAL/HAL.h"
#include "Port/Display.h"
#include "App/App.h"

void setup()
{
    HAL::Init();
    Port_Init();
    App_Init();
}

void loop()
{
    HAL::Update();

    delay(20);
}