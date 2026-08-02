/**
 * @file    Account_Master.h
 * @brief   账户系统注册与对外接口声明
 * @details 本文件定义 EasyGo 账户系统中的账户系统注册与对外接口声明，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#ifndef __DATA_PROC_H
#define __DATA_PROC_H

#include "App/Utils/AccountSystem/AccountBroker.h"
#include "HAL/HAL_Def.h"
#include "ACT_Def.h"

#define ACCOUNT_INIT_DEF(name) void _ACT_##name##_Init(Account* account)

void Accounts_Init();

namespace AccountSystem
{

AccountBroker* Broker();

void IMU_Commit(HAL::IMU_Info_t* info);
void BT_Commit(HAL::Dummy_Info_t* info);
void Location_Commit(const Location_Info_t* info);
void Location_GpsCommit(const Location_Info_t* info);
void Location_LbsCommit(const Location_Info_t* info);
void Network_Commit(const Network_Info_t* info);
void Input_Commit(const Input_Info_t* info);
}

#endif
