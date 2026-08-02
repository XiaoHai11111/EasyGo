/* Data processing node list */

ACT_DEF(Storage, 0);
ACT_DEF(IMU, sizeof(HAL::IMU_Info_t));
ACT_DEF(Location, sizeof(AccountSystem::Location_Info_t));
ACT_DEF(Network, sizeof(AccountSystem::Network_Info_t));
ACT_DEF(Input, sizeof(AccountSystem::Input_Info_t));
ACT_DEF(CareGo, sizeof(AccountSystem::CareGo_State_t));
ACT_DEF(MusicPlayer, sizeof(AccountSystem::MusicPlayer_State_t));
ACT_DEF(SysConfig, 0);
