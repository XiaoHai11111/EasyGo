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
