/**
 * @file    ACT_IMU.cpp
 * @brief   IMU 账户数据提交
 * @details 本文件实现 EasyGo 账户系统中的IMU 账户数据提交，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "Account_Master.h"
#include "HAL/HAL.h"

static Account* actIMU;

void AccountSystem::IMU_Commit(HAL::IMU_Info_t* info)
{
    actIMU->Commit(info, sizeof(HAL::IMU_Info_t));
}

ACCOUNT_INIT_DEF(IMU)
{
    actIMU = account;
}
