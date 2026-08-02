/**
 * @file    ACT_BT.cpp
 * @brief   蓝牙账户数据提交
 * @details 本文件实现 EasyGo 账户系统中的蓝牙账户数据提交，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "Account_Master.h"
#include "HAL/HAL.h"

static Account* actBT;

void AccountSystem::BT_Commit(HAL::Dummy_Info_t* info)
{
    if (actBT == nullptr || info == nullptr)
    {
        return;
    }

    actBT->Commit(info, sizeof(HAL::Dummy_Info_t));
}

ACCOUNT_INIT_DEF(BT)
{
    actBT = account;
}
