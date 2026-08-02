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
