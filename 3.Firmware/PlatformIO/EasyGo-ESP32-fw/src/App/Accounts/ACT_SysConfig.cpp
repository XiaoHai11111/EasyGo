/**
 * @file    ACT_SysConfig.cpp
 * @brief   系统配置账户与应用设置管理
 * @details 本文件实现 EasyGo 账户系统中的系统配置账户与应用设置管理，通过 Account 机制与页面、HAL 层解耦。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "Account_Master.h"
#include "HAL/HAL.h"
#include "App/Configs/Config.h"

using namespace AccountSystem;

static SysConfig_Info_t sysConfig =
    {
        .cmd = SYSCONFIG_CMD_LOAD,
        .soundEnable = CONFIG_SOUND_ENABLE_DEFAULT,
        .longitudeDefault = CONFIG_GPS_LNG_DEFAULT,
        .latitudeDefault = CONFIG_GPS_LAT_DEFAULT,
        .language = CONFIG_SYSTEM_LANGUAGE_DEFAULT,
        .mapDirPath = CONFIG_MAP_DIR_PATH,
        .WGS84 = CONFIG_MAP_USE_WGS84_DEFAULT,
        .arrowTheme = CONFIG_ARROW_THEME_DEFAULT,
    };

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->size != sizeof(SysConfig_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    SysConfig_Info_t* info = (SysConfig_Info_t*) param->data_p;

    switch (param->event)
    {
        case Account::EVENT_NOTIFY:
        {
            if (info->cmd == SYSCONFIG_CMD_LOAD) sysConfig.cmd = SYSCONFIG_CMD_LOAD;
        }
            break;
        case Account::EVENT_SUB_PULL:
        {
            memcpy(info, &sysConfig, sizeof(sysConfig));
        }
            break;
        default:
            return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    return 0;
}

ACCOUNT_INIT_DEF(SysConfig)
{
    account->Subscribe("Storage");
    account->SetEventCallback(onEvent);

    STORAGE_VALUE_REG(account, sysConfig.soundEnable, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sysConfig.longitudeDefault, STORAGE_TYPE_DOUBLE);
    STORAGE_VALUE_REG(account, sysConfig.latitudeDefault, STORAGE_TYPE_DOUBLE);
    STORAGE_VALUE_REG(account, sysConfig.language, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.mapDirPath, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.WGS84, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sysConfig.arrowTheme, STORAGE_TYPE_STRING);
}
