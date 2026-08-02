/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "App.h"
#include "HAL/HAL.h"
#include "Utils/PageManager/PageManager.h"
#include "Accounts/Account_Master.h"
#include "Resources/ResourcePool.h"
#include "Pages/AppFactory.h"

namespace
{
PageManager* g_pageManager = nullptr;
Account* g_inputRouter = nullptr;
volatile AccountSystem::Input_Action_t g_pendingAction = AccountSystem::INPUT_NONE;
uint32_t g_lastInputRouteAt = 0;

int onInputEvent(Account*, Account::EventParam_t* param)
{
    if (!param || param->event != Account::EVENT_PUB_PUBLISH)
        return Account::ERROR_UNSUPPORTED_REQUEST;
    if (param->size != sizeof(AccountSystem::Input_Info_t))
        return Account::ERROR_SIZE_MISMATCH;
    const AccountSystem::Input_Info_t* input =
        static_cast<const AccountSystem::Input_Info_t*>(param->data_p);
    g_pendingAction = input->action;
    return Account::ERROR_NONE;
}

void onInputRoute(lv_timer_t*)
{
    const AccountSystem::Input_Action_t action = g_pendingAction;
    g_pendingAction = AccountSystem::INPUT_NONE;
    if (!g_pageManager) return;
    const uint32_t now = lv_tick_get();
    if (now < 3000 || action == AccountSystem::INPUT_NONE) return;
    if (g_lastInputRouteAt != 0 && now - g_lastInputRouteAt < 700) return;

    PageBase* routed = nullptr;
    if (action == AccountSystem::INPUT_TOILET_CLICK ||
        action == AccountSystem::INPUT_HOME_CLICK)
        routed = g_pageManager->Push("Pages/Navigation");
    else if (action == AccountSystem::INPUT_SOS_DOUBLE_CLICK)
        routed = g_pageManager->Push("Pages/Family");
    if (routed) g_lastInputRouteAt = now;
}
}

void App_Init()
{
    static AppFactory factory;
    static PageManager manager(&factory);
    g_pageManager = &manager;

    Accounts_Init();
    g_inputRouter = new Account("AppInputRouter", AccountSystem::Broker());
    if (g_inputRouter)
    {
        g_inputRouter->SetEventCallback(onInputEvent);
        g_inputRouter->Subscribe("Input");
    }
    lv_timer_create(onInputRoute, 100, nullptr);
    Resource.Init();

    /*----------------------- Pages Init -----------------------*/
    manager.Install("Startup", "Pages/Startup");
    manager.Install("Home", "Pages/Home");
    manager.Install("Navigation", "Pages/Navigation");
    manager.Install("Family", "Pages/Family");
    manager.Install("Setting", "Pages/Setting");
    manager.Install("DeviceStatus", "Pages/DeviceStatus");

    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_RIGHT, 260);
    manager.Push("Pages/Startup");

    ACCOUNT_SEND_NOTIFY_CMD(Storage, STORAGE_CMD_LOAD);
    ACCOUNT_SEND_NOTIFY_CMD(SysConfig, SYSCONFIG_CMD_LOAD);

    INIT_DONE();
}

void App_UnInit()
{
    ACCOUNT_SEND_NOTIFY_CMD(SysConfig, SYSCONFIG_CMD_SAVE);
    ACCOUNT_SEND_NOTIFY_CMD(Storage, STORAGE_CMD_SAVE);
    ACCOUNT_SEND_NOTIFY_CMD(Recorder, RECORDER_CMD_STOP);
}
