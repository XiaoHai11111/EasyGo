#include "HAL/HAL.h"

static uint32_t Power_LastHandleTime = 0;
static uint16_t Power_AutoLowPowerTimeout = 60;
static bool Power_AutoLowPowerEnable = false;
static bool Power_IsShutdown = false;

static volatile uint16_t Power_ADCValue = 0;
static uint16_t Power_ADCValue_last[10] = {4095};

static HAL::Power_CallbackFunction_t Power_EventCallback = NULL;

#define BATT_MAX_VOLTAGE 3900
#define BATT_MIN_VOLTAGE 3300

static void Power_ADC_Init()
{
    pinMode(CONFIG_BAT_DET_PIN, INPUT);
    pinMode(CONFIG_BAT_CHG_DET_PIN, INPUT_PULLUP);
}

void HAL::Power_Init()
{
    pinMode(CONFIG_BAT_CHG_DET_PIN, INPUT);

    Serial.println("Power: Waiting...");
    pinMode(CONFIG_POWER_EN_PIN, OUTPUT);
    digitalWrite(CONFIG_POWER_EN_PIN, LOW);

    const uint64_t startTime = millis();
    while (millis() - startTime < 1000)
    {
        HAL::BT_Update();
        delay(100);
    }

    digitalWrite(CONFIG_POWER_EN_PIN, HIGH);
    Serial.println("Power: ON");

    Power_ADC_Init();
    Power_SetAutoLowPowerTimeout(60);
    Power_HandleTimeUpdate();
    Power_SetAutoLowPowerEnable(false);
}

static void Power_ADC_TrigUpdate()
{
    Power_ADCValue = analogRead(CONFIG_BAT_DET_PIN);
}

void HAL::Power_HandleTimeUpdate()
{
    Power_LastHandleTime = millis();
}

void HAL::Power_SetAutoLowPowerTimeout(uint16_t sec)
{
    Power_AutoLowPowerTimeout = sec;
}

uint16_t HAL::Power_GetAutoLowPowerTimeout()
{
    return Power_AutoLowPowerTimeout;
}

void HAL::Power_SetAutoLowPowerEnable(bool en)
{
    Power_AutoLowPowerEnable = en;
    Power_HandleTimeUpdate();
}

void HAL::Power_Shutdown()
{
#if !CONFIG_ENABLE_POWER
    /* GPIO21 is the touch SDA pin in the ESP32 Dev Module bring-up profile. */
    Backlight_SetGradual(0, 500);
    return;
#else
    Backlight_SetGradual(0, 500);
    digitalWrite(CONFIG_POWER_EN_PIN, LOW);
    Power_IsShutdown = true;
#endif
}

void HAL::Power_Update()
{
    __IntervalExecute(Power_ADC_TrigUpdate(), 1000);

    if (!Power_AutoLowPowerEnable)
        return;

    if (Power_AutoLowPowerTimeout == 0)
        return;

    if (millis() - Power_LastHandleTime >= (Power_AutoLowPowerTimeout * 1000))
        Power_Shutdown();
}

void HAL::Power_GetInfo(Power_Info_t* info)
{
#if !CONFIG_ENABLE_POWER
    /*
     * The ESP32 Dev Module bring-up wiring has no battery divider or charge
     * signal. Return a stable full-battery value so the UI remains usable
     * without touching unavailable GPIO37/GPIO38.
     */
    info->usage = 100;
    info->isCharging = false;
    info->voltage = 0;
    return;
#else
    uint32_t sum = Power_ADCValue;
    for (int i = 9; i > 0; i--)
    {
        Power_ADCValue_last[i] = Power_ADCValue_last[i - 1];
        sum += Power_ADCValue_last[i - 1];
    }
    Power_ADCValue_last[0] = Power_ADCValue;

    int voltage = map(
        sum / 10,
        0, 4095,
        0, 3300);

    voltage *= 2;
    __LimitValue(voltage, BATT_MIN_VOLTAGE, BATT_MAX_VOLTAGE);

    const int usage = map(
        voltage,
        BATT_MIN_VOLTAGE, BATT_MAX_VOLTAGE,
        0, 100);

    info->usage = usage;
    info->isCharging = usage != 100 && !digitalRead(CONFIG_BAT_CHG_DET_PIN);
    info->voltage = voltage;
#endif
}
