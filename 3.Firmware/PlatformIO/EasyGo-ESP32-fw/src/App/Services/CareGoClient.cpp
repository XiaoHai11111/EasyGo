/**
 * @file    CareGoClient.cpp
 * @brief   CareGo 客户端服务实现
 * @details 本文件实现 CareGo 客户端服务的CareGo 客户端服务实现，负责订阅账户状态并协调导航、提醒与 SOS 逻辑。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "CareGoClient.h"
#include <string.h>

CareGoClient::CareGoClient() : account(nullptr)
{
    memset(&state, 0, sizeof(state));
}

CareGoClient::~CareGoClient()
{
    Deinit();
}

bool CareGoClient::Init(const char* accountId)
{
    if (account) return true;
    if (!accountId) return false;
    account = new Account(accountId, AccountSystem::Broker(), 0, this);
    if (!account) return false;
    account->SetEventCallback(onEvent);
    if (!account->Subscribe("CareGo"))
    {
        delete account;
        account = nullptr;
        return false;
    }
    AccountSystem::CareGo_State_t pulled = {};
    account->Pull("CareGo", &pulled, sizeof(pulled));
    LockState();
    state = pulled;
    UnlockState();
    return true;
}

void CareGoClient::LockState() const
{
    while (stateLock.test_and_set(std::memory_order_acquire)) {}
}

void CareGoClient::UnlockState() const
{
    stateLock.clear(std::memory_order_release);
}

AccountSystem::CareGo_State_t CareGoClient::State() const
{
    AccountSystem::CareGo_State_t snapshot;
    LockState();
    snapshot = state;
    UnlockState();
    return snapshot;
}

void CareGoClient::Deinit()
{
    if (!account) return;
    delete account;
    account = nullptr;
}

bool CareGoClient::Send(AccountSystem::CareGo_Command_Code_t command, bool enabled, const char* phone)
{
    if (!account) return false;
    AccountSystem::CareGo_Command_t message = {};
    message.cmd = command;
    message.enabled = enabled;
    if (phone)
    {
        strncpy(message.phone, phone, sizeof(message.phone) - 1);
        message.phone[sizeof(message.phone) - 1] = '\0';
    }
    return account->Notify("CareGo", &message, sizeof(message)) == Account::ERROR_NONE;
}

int CareGoClient::onEvent(Account* account, Account::EventParam_t* param)
{
    if (!account || !param || param->event != Account::EVENT_PUB_PUBLISH)
        return Account::ERROR_UNSUPPORTED_REQUEST;
    if (param->size != sizeof(AccountSystem::CareGo_State_t))
        return Account::ERROR_SIZE_MISMATCH;
    CareGoClient* instance = static_cast<CareGoClient*>(account->UserData);
    if (!instance) return Account::ERROR_PARAM_ERROR;
    instance->LockState();
    memcpy(&instance->state, param->data_p, sizeof(instance->state));
    instance->UnlockState();
    return Account::ERROR_NONE;
}
