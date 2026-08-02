/**
 * @file    ACT_Input.cpp
 * @brief   输入按键账户数据提交与事件发布
 * @details 本文件实现 EasyGo 账户系统中的输入按键账户数据提交与事件发布，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "Account_Master.h"

static Account* actInput = nullptr;

void AccountSystem::Input_Commit(const Input_Info_t* info)
{
    if (!actInput || !info) return;
    if (actInput->Commit(info, sizeof(*info))) actInput->Publish();
}

ACCOUNT_INIT_DEF(Input)
{
    actInput = account;
}
