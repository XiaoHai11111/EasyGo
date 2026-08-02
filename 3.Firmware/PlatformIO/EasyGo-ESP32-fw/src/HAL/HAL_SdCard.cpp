/**
 * @file    HAL_SdCard.cpp
 * @brief   SD 卡挂载、文件系统与数据读写
 * @details 本文件实现 EasyGo 适老导航设备硬件抽象层的SD 卡挂载、文件系统与数据读写，供上层账户与页面统一调用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "HAL/HAL.h"
#include "SPI.h"
#include "SD.h"

namespace
{
bool cardReady = false;
float cardSizeMb = 0.0f;
HAL::SD_CallbackFunction_t eventCallback = nullptr;
}

bool HAL::SD_Init()
{
    if (CONFIG_SD_DET_PIN >= 0) pinMode(CONFIG_SD_DET_PIN, INPUT);
    pinMode(CONFIG_SD_CS_PIN, OUTPUT);
    digitalWrite(CONFIG_SD_CS_PIN, HIGH);

    /* Share VSPI with the display; each device keeps an independent CS pin. */
    if (!SD.begin(CONFIG_SD_CS_PIN, SPI, 16000000))
    {
        Serial.printf("SD: mount failed on shared VSPI, CS%d\r\n", CONFIG_SD_CS_PIN);
        return false;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("SD: no card attached");
        return false;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    } else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    } else
    {
        Serial.println("UNKNOWN");
    }

    cardSizeMb = static_cast<float>(SD.cardSize()) / (1024.0f * 1024.0f);
    cardReady = true;
    Serial.printf("SD: %.0f MB, shared VSPI CS%d\r\n", cardSizeMb, CONFIG_SD_CS_PIN);
    return true;
}

bool HAL::SD_GetReady()
{
#if CONFIG_ENABLE_SD_CARD
    return CONFIG_SD_DET_PIN >= 0 ? digitalRead(CONFIG_SD_DET_PIN) == LOW : cardReady;
#else
    return false;
#endif
}

float HAL::SD_GetCardSizeMB()
{
    return cardSizeMb;
}

void HAL::SD_SetEventCallback(SD_CallbackFunction_t callback)
{
    eventCallback = callback;
}

void HAL::SD_Update()
{
    static bool previous = cardReady;
    const bool current = SD_GetReady();
    if (current != previous && eventCallback) eventCallback(current);
    previous = current;
}
