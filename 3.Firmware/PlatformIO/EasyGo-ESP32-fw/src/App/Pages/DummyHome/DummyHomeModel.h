#pragma once
#ifndef __DUMMYHOME_MODEL_H
#define __DUMMYHOME_MODEL_H

#include "lvgl.h"
#include "App/Accounts/Account_Master.h"
#include "HAL/HAL.h"

namespace Page
{

    class DummyHomeModel
    {
    public:
        void Init();
        void Deinit();
        void PlayMusic(const char* music);
        void SetEncoderEnable(bool en);
    private:
        Account* account;
    };
}
#endif
