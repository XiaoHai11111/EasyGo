#include "HAL/HAL.h"
#include "Accounts/Account_Master.h"

bool HAL::BluetoothConnected()
{
    return true;
}

void HAL::JointsInfo_Update()
{
    Dummy_Info_t dummyinfo;
    dummyinfo.joint1 = rand() % 100 - 50;
    dummyinfo.joint2 = rand() % 100 - 50;
    dummyinfo.joint3 = rand() % 100 - 50;
    dummyinfo.joint4 = rand() % 100 - 50;
    dummyinfo.joint5 = rand() % 100 - 50;
    dummyinfo.joint6 = rand() % 100 - 50;
    dummyinfo.x = rand() % 100 - 50;
    dummyinfo.y = rand() % 100 - 50;
    dummyinfo.z = rand() % 100 - 50;
    dummyinfo.roll = rand() % 100 - 50;
    dummyinfo.pitch = rand() % 100 - 50;
    dummyinfo.yaw = rand() % 100 - 50;
    AccountSystem::BT_Commit(&dummyinfo);
}
