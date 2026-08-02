#pragma once

#include "../Common/EasyGoUi.h"
#include "App/Accounts/ACT_Def.h"

namespace Page
{
class DeviceStatusView
{
public:
    struct
    {
        lv_obj_t* back;
        lv_obj_t* gpsValue;
        lv_obj_t* networkValue;
        lv_obj_t* audioValue;
        lv_obj_t* storageValue;
        lv_obj_t* motionValue;
        lv_obj_t* safetyNote;
    } ui;

    void Create(lv_obj_t* root);
    void Update(const AccountSystem::CareGo_State_t& state);
    void Delete() {}

private:
    lv_obj_t* CreateRow(lv_obj_t* root, lv_coord_t y, const char* title,
                        const char* symbol, lv_color_t color);
};
}
