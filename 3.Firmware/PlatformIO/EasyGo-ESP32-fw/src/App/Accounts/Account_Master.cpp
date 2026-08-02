/**
 * @file    Account_Master.cpp
 * @brief   账户系统初始化与账户注册实现
 * @details 本文件实现 EasyGo 账户系统中的账户系统初始化与账户注册实现，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "Account_Master.h"
#include "HAL/HAL.h"

static AccountBroker dataCenter("MASTER");

AccountBroker* AccountSystem::Broker()
{
    return &dataCenter;
}

void Accounts_Init()
{
#define ACT_DEF(NodeName, bufferSize)\
    Account* act##NodeName = new Account(#NodeName, &dataCenter, bufferSize);

#include "_ACT_LIST.inc"

#undef ACT_DEF

#define ACT_DEF(NodeName, bufferSize)\
do{\
    extern void _ACT_##NodeName##_Init(Account* account);\
    _ACT_##NodeName##_Init(act##NodeName);\
}while(0)

#include "_ACT_LIST.inc"

#undef ACT_DEF
}
