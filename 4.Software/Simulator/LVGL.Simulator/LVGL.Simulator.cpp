/*
 * PROJECT:   LVGL ported to Windows Desktop
 * FILE:      LVGL.Windows.Desktop.cpp
 * PURPOSE:   Implementation for LVGL ported to Windows Desktop
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <stdio.h>
#include <Windows.h>
#include <string>
#include "resource.h"
#include "App.h"
#include "HAL/HAL.h"

#if _MSC_VER >= 1200
 // Disable compilation warnings.
#pragma warning(push)
// nonstandard extension used : bit field types other than int
#pragma warning(disable:4214)
// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4244)
#endif

#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lv_drivers/win32drv/win32drv.h"
#include "lv_fs_if/lv_fs_if.h"

#if _MSC_VER >= 1200
// Restore compilation warnings.
#pragma warning(pop)
#endif

static bool SetSimulatorAssetRoot()
{
    wchar_t modulePath[MAX_PATH] = {};
    if (GetModuleFileNameW(nullptr, modulePath, MAX_PATH) == 0)
    {
        return false;
    }

    std::wstring directory(modulePath);
    size_t separator = directory.find_last_of(L"\\/");
    if (separator == std::wstring::npos) return false;
    directory.resize(separator);

    for (int level = 0; level < 8; ++level)
    {
        std::wstring mapPath = directory + L"\\SD\\MAP\\shenzhen.bin";
        if (GetFileAttributesW(mapPath.c_str()) != INVALID_FILE_ATTRIBUTES)
        {
            return SetCurrentDirectoryW(directory.c_str()) != FALSE;
        }

        separator = directory.find_last_of(L"\\/");
        if (separator == std::wstring::npos) break;
        directory.resize(separator);
    }
    return false;
}

int main()
{
    lv_init();

    SetSimulatorAssetRoot();
    lv_fs_if_init();

    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        240,
        320,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return -1;
    }

    HAL::HAL_Init();

    App_Init();

    while (!lv_win32_quit_signal)
    {
        lv_timer_handler();
        HAL::HAL_Update();
        Sleep(1);
    }

    App_UnInit();

    return 0;
}
