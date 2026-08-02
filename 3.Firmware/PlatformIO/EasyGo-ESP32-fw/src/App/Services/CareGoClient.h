/**
 * @file    CareGoClient.h
 * @brief   CareGo 客户端服务接口声明
 * @details 本文件定义 CareGo 客户端服务的CareGo 客户端服务接口声明，负责订阅账户状态并协调导航、提醒与 SOS 逻辑。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include "App/Accounts/Account_Master.h"
#include <atomic>

class CareGoClient
{
public:
    CareGoClient();
    ~CareGoClient();
    bool Init(const char* accountId);
    void Deinit();
    AccountSystem::CareGo_State_t State() const;
    bool Send(AccountSystem::CareGo_Command_Code_t command, bool enabled = false, const char* phone = nullptr);

private:
    static int onEvent(Account* account, Account::EventParam_t* param);
    void LockState() const;
    void UnlockState() const;
    Account* account;
    mutable std::atomic_flag stateLock = ATOMIC_FLAG_INIT;
    AccountSystem::CareGo_State_t state;
};
