/**
 * @file    StartUpModel.cpp
 * @brief   启动页数据模型实现
 * @details 本文件实现 EasyGo 的启动页数据模型实现，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "StartupModel.h"

using namespace Page;

void StartupModel::Init()
{
    account = new Account("StartupModel", AccountSystem::Broker(), 0, this);
}

void StartupModel::DeInit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

void StartupModel::PlayMusic(const char* music)
{
    AccountSystem::MusicPlayer_Info_t info;
    info.music = music;
    account->Notify("MusicPlayer", &info, sizeof(info));
}
