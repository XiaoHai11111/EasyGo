#ifndef __HAL_H
#define __HAL_H

#include <stdint.h>
#include <Arduino.h>
#include "HAL_Def.h"
#include "App/Configs/Config.h"
#include "CommonMacro.h"
#include "FreeRTOS/FreeRTOS.h"

namespace HAL
{
    void Init();
    void Update();

/* Backlight */
    void Backlight_Init();
    uint32_t Backlight_GetValue();
    void Backlight_SetValue(int32_t val);
    void Backlight_SetGradual(uint32_t target, uint16_t time = 500);
    void Backlight_ForceLit(bool en);

/* I2C */
    void I2C_Init(bool startScan);

/* Capacitive touch */
    bool Touch_Init();
    bool Touch_GetPoint(uint16_t* x, uint16_t* y);

/* IMU */
    void IMU_Init();
    void IMU_Update();

/* SD */
    bool SD_Init();
    void SD_Update();
    bool SD_GetReady();
    float SD_GetCardSizeMB();
    typedef void(* SD_CallbackFunction_t)(bool insert);
    void SD_SetEventCallback(SD_CallbackFunction_t callback);

    bool BluetoothConnected();

/* Audio */
    void Audio_Init();
    void Audio_Update();
    bool Audio_PlayMusic(const char* name);
    bool Audio_Stop();
    bool Audio_SetVolume(uint8_t volume);
    bool Audio_IsBusy();
    bool Audio_HasStorage();

/* GPS */
    void GPS_Init();
    void GPS_Update();

/* Cellular / network */
    void Cellular_Init();
    void Cellular_Update();
    bool Cellular_QueryNearby(double latitude, double longitude);
    bool Cellular_QueryLbs();
    bool Cellular_SendSms(const char* phone, const char* message);

/* Physical keys */
    void Keys_Init();
    void Keys_Update();


/* Bluetooth */
    bool BT_Init();
    void BT_Update();

}

#endif
