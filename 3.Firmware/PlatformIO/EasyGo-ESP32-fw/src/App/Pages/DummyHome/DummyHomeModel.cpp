#include "DummyHomeModel.h"

using namespace Page;

void DummyHomeModel::Init()
{
    account = new Account("DummyHomeModel", AccountSystem::Broker(), 0, this);
}

void DummyHomeModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}
void DummyHomeModel::PlayMusic(const char* music)
{
    AccountSystem::MusicPlayer_Info_t info;
    info.music = music;
    account->Notify("MusicPlayer", &info, sizeof(info));
}

void DummyHomeModel::SetEncoderEnable(bool en)
{
    HAL::Encoder_SetEnable(en);
}