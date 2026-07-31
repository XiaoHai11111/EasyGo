#include "HAL/HAL.h"
#include "App/Configs/Version.h"

#define DISP_HOR_RES         CONFIG_SCREEN_HOR_RES
#define DISP_VER_RES         CONFIG_SCREEN_VER_RES
#define DISP_BUF_SIZE        CONFIG_SCREEN_BUFFER_SIZE
extern lv_color_t* lv_disp_buf_p;

void HAL::Init()
{
    Serial.begin(115200);
    Serial.println(VERSION_FIRMWARE_NAME);
    Serial.println("Version: " VERSION_SOFTWARE);
    Serial.println("Author: " VERSION_AUTHOR_NAME);

    // Move the malloc process to Init() to make sure that the largest heap can be used for this buffer.
    lv_disp_buf_p = static_cast<lv_color_t*>(malloc(DISP_BUF_SIZE * sizeof(lv_color_t)));
    if (lv_disp_buf_p == nullptr)
        LV_LOG_WARN("lv_port_disp_init malloc failed!\n");

    #if CONFIG_ENABLE_BLUETOOTH
    HAL::BT_Init(); // ToDo: some of the process below will interrupt BLE connection, find it out
    #endif

    #if CONFIG_ENABLE_POWER
    HAL::Power_Init();
    #endif

    HAL::Backlight_Init();

    #if CONFIG_ENABLE_ENCODER
    HAL::Encoder_Init();
    #endif

    #if CONFIG_ENABLE_BUZZER
    HAL::Buzz_init();
    #endif

    #if CONFIG_ENABLE_AUDIO
    HAL::Audio_Init();
    #endif

    #if CONFIG_ENABLE_SD_CARD
    HAL::SD_Init();
    #endif

    #if CONFIG_ENABLE_TOUCH || CONFIG_ENABLE_IMU
    HAL::I2C_Init(false);
    #endif

    #if CONFIG_ENABLE_TOUCH
    HAL::Touch_Init();
    #endif

    #if CONFIG_ENABLE_IMU
    HAL::IMU_Init();
    #endif

    #if CONFIG_ENABLE_AUDIO
    HAL::Audio_PlayMusic("Startup");
    #endif
}

void HAL::Update()
{
    #if CONFIG_ENABLE_POWER
    HAL::Power_Update();
    #endif

    #if CONFIG_ENABLE_ENCODER
    HAL::Encoder_Update();
    #endif

    #if CONFIG_ENABLE_AUDIO
    HAL::Audio_Update();
    #endif

    #if CONFIG_ENABLE_IMU
    HAL::IMU_Update();
    #endif

    #if CONFIG_ENABLE_BLUETOOTH
    HAL::BT_Update();
    #endif

    #if CONFIG_ENABLE_SD_CARD
    __IntervalExecute(HAL::SD_Update(), 500);
    #endif
}
