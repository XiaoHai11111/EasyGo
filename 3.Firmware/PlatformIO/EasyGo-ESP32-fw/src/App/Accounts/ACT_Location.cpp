#include "Account_Master.h"
#include "App/Configs/Config.h"
#include "lvgl.h"

static Account* actLocation = nullptr;
static AccountSystem::Location_Info_t gpsLocation = {};
static AccountSystem::Location_Info_t lbsLocation = {};

namespace
{
bool IsFresh(const AccountSystem::Location_Info_t& info, uint32_t lifetimeMs)
{
    return info.valid && (lv_tick_get() - info.updatedAtMs) <= lifetimeMs;
}

void Publish(const AccountSystem::Location_Info_t* info)
{
    if (!actLocation || !info) return;
    if (actLocation->Commit(info, sizeof(*info))) actLocation->Publish();
}
}

void AccountSystem::Location_Commit(const Location_Info_t* info)
{
    Publish(info);
}

void AccountSystem::Location_GpsCommit(const Location_Info_t* info)
{
    if (!info) return;
    gpsLocation = *info;
    if (IsFresh(gpsLocation, CONFIG_GPS_FIX_VALID_MS)) Publish(&gpsLocation);
    else if (IsFresh(lbsLocation, CONFIG_CELLULAR_LBS_VALID_MS)) Publish(&lbsLocation);
    else Publish(&gpsLocation);
}

void AccountSystem::Location_LbsCommit(const Location_Info_t* info)
{
    if (!info) return;
    lbsLocation = *info;
    if (IsFresh(gpsLocation, CONFIG_GPS_FIX_VALID_MS)) return;
    Publish(&lbsLocation);
}

ACCOUNT_INIT_DEF(Location)
{
    actLocation = account;
    gpsLocation = {};
    lbsLocation = {};
}
