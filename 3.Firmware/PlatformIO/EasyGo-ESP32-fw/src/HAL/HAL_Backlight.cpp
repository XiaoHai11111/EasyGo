#include "Port/Display.h"
#include "HAL/HAL.h"

static uint32_t backlightValue = 0;

static void Backlight_AnimCallback(void* obj, int32_t brightness)
{
    HAL::Backlight_SetValue(brightness);
}

void HAL::Backlight_Init()
{
#if CONFIG_SCREEN_BLK_PIN >= 0
    /* 10-bit PWM at 5 kHz. */
    ledcSetup(0, 5000, 10);
    ledcAttachPin(CONFIG_SCREEN_BLK_PIN, 0);
    ledcWrite(0, 0);
#else
    /* The first-light wiring ties LED directly to 3V3. */
    backlightValue = 1000;
#endif
}

void HAL::Backlight_SetGradual(uint32_t target, uint16_t time)
{
    lv_anim_t animation;
    lv_anim_init(&animation);
    lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)Backlight_AnimCallback);
    lv_anim_set_values(&animation, Backlight_GetValue(), target);
    lv_anim_set_time(&animation, time);
    lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
    lv_anim_start(&animation);
}

uint32_t HAL::Backlight_GetValue()
{
    return backlightValue;
}

void HAL::Backlight_SetValue(int32_t val)
{
    val = constrain(val, 0, 1024);
    backlightValue = val;

#if CONFIG_SCREEN_BLK_PIN >= 0
    ledcWrite(0, backlightValue);
#endif
}

void HAL::Backlight_ForceLit(bool en)
{
#if CONFIG_SCREEN_BLK_PIN >= 0
    pinMode(CONFIG_SCREEN_BLK_PIN, OUTPUT);
    digitalWrite(CONFIG_SCREEN_BLK_PIN, en);
#else
    backlightValue = en ? 1000 : 0;
#endif
}
