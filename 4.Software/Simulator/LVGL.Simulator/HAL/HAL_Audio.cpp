#include "HAL.h"
void HAL::Audio_Init()
{
}

void HAL::Audio_Update()
{
}

bool HAL::Audio_PlayMusic(const char* name)
{
    return name != nullptr && name[0] != '\0';
}

bool HAL::Audio_Stop() { return true; }
bool HAL::Audio_SetVolume(uint8_t) { return true; }
bool HAL::Audio_IsBusy() { return false; }
bool HAL::Audio_HasStorage() { return true; }
