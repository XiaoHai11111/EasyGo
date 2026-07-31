/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "Display.h"
#include "HAL/HAL.h"

TaskHandle_t handleTaskLvgl;

void TaskLvglUpdate(void* parameter)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    for (;;)
    {
        lv_task_handler();
        delay(5);
    }
}

void Port_Init()
{
    static SCREEN_CLASS screen;

    screen.begin();
    screen.setRotation(CONFIG_SCREEN_ROTATION);
    Serial.printf(
        "Display: ST7789 %d x %d, rotation=%d\r\n",
        screen.width(),
        screen.height(),
        CONFIG_SCREEN_ROTATION);

#if CONFIG_SCREEN_SELF_TEST
    screen.fillScreen(TFT_RED);
    delay(120);
    screen.fillScreen(TFT_GREEN);
    delay(120);
    screen.fillScreen(TFT_BLUE);
    delay(120);
#endif
    screen.fillScreen(TFT_BLACK);

    lv_init();
    lv_port_disp_init(&screen);
    lv_port_indev_init();
    lv_fs_if_init();

    xTaskCreate(
        TaskLvglUpdate,
        "LvglThread",
        20000,
        nullptr,
        configMAX_PRIORITIES - 1,
        &handleTaskLvgl);

    HAL::Backlight_SetGradual(500, 1000);
}
