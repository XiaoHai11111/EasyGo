/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "HAL/HAL.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_indev_t* touch_indev;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static lv_indev_drv_t touch_drv;

void lv_port_indev_init(void)
{
#if CONFIG_ENABLE_TOUCH
    /* Register the capacitive panel as a pointer device. */
    lv_indev_drv_init(&touch_drv);
    touch_drv.type = LV_INDEV_TYPE_POINTER;
    touch_drv.read_cb = touchpad_read;
    touch_indev = lv_indev_drv_register(&touch_drv);
#endif

}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void touchpad_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    static uint16_t last_x = 0;
    static uint16_t last_y = 0;

    uint16_t x = last_x;
    uint16_t y = last_y;
    const bool pressed = HAL::Touch_GetPoint(&x, &y);
    if (pressed)
    {
        last_x = x;
        last_y = y;
    }

    data->point.x = last_x;
    data->point.y = last_y;
    data->state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
