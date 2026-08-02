/**
 * @file    HAL_IMU.cpp
 * @brief   IMU 传感器数据读取与姿态信息发布
 * @details 本文件实现 EasyGo 适老导航设备硬件抽象层的IMU 传感器数据读取与姿态信息发布，供上层账户与页面统一调用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "HAL/HAL.h"
#include "MPU6050.h"
#include "App/Accounts/Account_Master.h"

static MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t roll, yaw, pitch;

void HAL::IMU_Init()
{
    mpu.initialize();
    if (mpu.testConnection())
    {
        Serial.println("MPU connection failed.");
    }
}

void HAL::IMU_Update()
{
    IMU_Info_t imuInfo;
   // mpu.update();
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    mpu.getRotation(&roll,&yaw,&pitch);
    imuInfo.ax = ax;
    imuInfo.ay = ay;
    imuInfo.az = az;
    imuInfo.gx = gx;
    imuInfo.gy = gy;
    imuInfo.gz = gz;
    imuInfo.mx = 0; //6050无
    imuInfo.my = 0;
    imuInfo.mz = 0;
    imuInfo.roll = roll;
    imuInfo.yaw = yaw;
    imuInfo.pitch = pitch;

    AccountSystem::IMU_Commit(&imuInfo);
}
