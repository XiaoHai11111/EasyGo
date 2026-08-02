#ifndef __HAL_H
#define __HAL_H

#include <stdint.h>
#include "HAL_Def.h"
#include "Configs/Config.h"

namespace HAL
{

    void HAL_Init();
    void HAL_Update();

    /* Backlight */
    void Backlight_Init();
    uint16_t Backlight_GetValue();
    void Backlight_SetValue(int16_t val);
    void Backlight_SetGradual(uint16_t target, uint16_t time = 500);
    void Backlight_ForceLit(bool en);

    /* I2C */
    void I2C_Scan(bool startScan);

    /* IMU */
    void IMU_Init();
    void IMU_Update();

    void JointsInfo_Update();

    /* SD */
    bool SD_Init();
    void SD_Update();
    bool SD_GetReady();
    float SD_GetCardSizeMB();
    typedef void(*SD_CallbackFunction_t)(bool insert);
    void SD_SetEventCallback(SD_CallbackFunction_t callback);

    bool  BluetoothConnected();

/* Audio */
    void Audio_Init();
    void Audio_Update();
    bool Audio_PlayMusic(const char* name);
    bool Audio_Stop();
    bool Audio_SetVolume(uint8_t volume);
    bool Audio_IsBusy();
    bool Audio_HasStorage();

}

#endif
