#pragma once

#include "App/Services/CareGoClient.h"

namespace Page
{
class DeviceStatusModel
{
public:
    void Init() { client.Init("DeviceStatusModel"); }
    void Deinit() {}
    AccountSystem::CareGo_State_t State() const { return client.State(); }

private:
    CareGoClient client;
};
}
