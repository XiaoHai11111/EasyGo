#include "HAL/HAL.h"
#include "App/Configs/Version.h"

void HAL::HAL_Init()
{
    Audio_Init();
}

void HAL::HAL_Update()
{
    IMU_Update();
    Audio_Update();
}
