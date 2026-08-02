/**
 * @file    _ACT_LIST.inc
 * @brief   账户节点注册列表
 * @details 本文件维护账户系统的账户节点注册列表，新增账户时需同步更新。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

/* Data processing node list */

ACT_DEF(Storage, 0);
ACT_DEF(IMU, sizeof(HAL::IMU_Info_t));
ACT_DEF(Location, sizeof(AccountSystem::Location_Info_t));
ACT_DEF(Network, sizeof(AccountSystem::Network_Info_t));
ACT_DEF(Input, sizeof(AccountSystem::Input_Info_t));
ACT_DEF(CareGo, sizeof(AccountSystem::CareGo_State_t));
ACT_DEF(MusicPlayer, sizeof(AccountSystem::MusicPlayer_State_t));
ACT_DEF(SysConfig, 0);
