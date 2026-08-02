/**
 * @file    HAL_I2C_Scan.cpp
 * @brief   I2C 总线初始化与设备地址扫描
 * @details 本文件实现 EasyGo 适老导航设备硬件抽象层的I2C 总线初始化与设备地址扫描，供上层账户与页面统一调用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "HAL/HAL.h"
#include "Wire.h"

void HAL::I2C_Init(bool startScan)
{
    Wire.begin(CONFIG_MCU_SDA_PIN,CONFIG_MCU_SCL_PIN);

    if(!startScan)
        return;

    uint8_t error, address;
    int nDevices;

    Serial.println("I2C: device scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++ )
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C: device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");

            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("I2C: unknow error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    Serial.printf("I2C: %d devices was found\r\n", nDevices);
}
