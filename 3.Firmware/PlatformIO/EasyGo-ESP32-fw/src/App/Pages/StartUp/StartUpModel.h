/**
 * @file    StartUpModel.h
 * @brief   启动页数据模型定义
 * @details 本文件定义 EasyGo 的启动页数据模型定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#ifndef __STARTUP_MODEL_H
#define __STARTUP_MODEL_H

#include "App/Accounts/Account_Master.h"
#include "HAL/HAL.h"

namespace Page
{

class StartupModel
{
public:
    void Init();
    void DeInit();
    void PlayMusic(const char* music);
private:
    Account* account;
};

}

#endif
