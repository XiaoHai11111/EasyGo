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
