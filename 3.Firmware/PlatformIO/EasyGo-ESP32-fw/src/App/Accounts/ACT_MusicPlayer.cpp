#include "Account_Master.h"
#include "HAL/HAL.h"
#include <string.h>

namespace
{
Account* musicAccount = nullptr;
AccountSystem::MusicPlayer_State_t state = {};

void PublishState()
{
    AccountSystem::MusicPlayer_State_t next = {};
    next.playing = HAL::Audio_IsBusy();
    next.storagePresent = HAL::Audio_HasStorage();
    if (next.playing == state.playing && next.storagePresent == state.storagePresent) return;
    state = next;
    if (musicAccount && musicAccount->Commit(&state, sizeof(state))) musicAccount->Publish();
}
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_SUB_PULL)
    {
        if (param->size != sizeof(state)) return Account::ERROR_SIZE_MISMATCH;
        memcpy(param->data_p, &state, sizeof(state));
        return Account::ERROR_NONE;
    }
    if (param->event == Account::EVENT_TIMER)
    {
        PublishState();
        return Account::ERROR_NONE;
    }
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(AccountSystem::MusicPlayer_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    AccountSystem::MusicPlayer_Info_t* info = (AccountSystem::MusicPlayer_Info_t*)param->data_p;

    HAL::Audio_PlayMusic(info->music);
    PublishState();

    return 0;
}

ACCOUNT_INIT_DEF(MusicPlayer)
{
    musicAccount = account;
    memset(&state, 0, sizeof(state));
    state.playing = HAL::Audio_IsBusy();
    state.storagePresent = HAL::Audio_HasStorage();
    account->SetEventCallback(onEvent);
    account->SetTimerPeriod(500);
    account->Commit(&state, sizeof(state));
}
