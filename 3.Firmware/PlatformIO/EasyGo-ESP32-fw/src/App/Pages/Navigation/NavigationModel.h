/**
 * @file    NavigationModel.h
 * @brief   导航页数据模型定义
 * @details 本文件定义 EasyGo 的导航页数据模型定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#pragma once

#include <stdint.h>
#include "App/Services/CareGoClient.h"

namespace Page
{
class NavigationModel
{
public:
    struct MapBounds
    {
        int32_t southE7;
        int32_t westE7;
        int32_t northE7;
        int32_t eastE7;
    };

    enum
    {
        ZoomLevelMin = 0,
        ZoomLevelDefault = 3,
        ZoomLevelMax = 6,
        MapViewportWidth = 240,
        MapViewportHeight = 164,
        MapCanvasWidth = 720,
        MapCanvasHeight = 492,
        MapCanvasInset = 24,
        MapInitialScrollX = 240,
        MapInitialScrollY = 164
    };

    void Init() { client.Init("NavigationModel"); }
    void Deinit() {}
    AccountSystem::CareGo_State_t State() const { return client.State(); }
    void StartToiletNavigation() { client.Send(AccountSystem::CAREGO_CMD_NAVIGATE_TOILET); }
    void StopNavigation() { client.Send(AccountSystem::CAREGO_CMD_STOP_NAVIGATION); }
    int ClampZoomLevel(int level) const
    {
        if (level < ZoomLevelMin) return ZoomLevelMin;
        if (level > ZoomLevelMax) return ZoomLevelMax;
        return level;
    }
    int ZoomRatio(int level) const
    {
        static const int ratios[] = { 1, 2, 4, 8, 16, 32, 64 };
        return ratios[ClampZoomLevel(level)];
    }
    const MapBounds& Bounds(int level) const
    {
        static const MapBounds bounds[] =
        {
            { 221765675, 1133872741, 232083227, 1142669971 },
            { 222954022, 1136583402, 228112798, 1140982018 },
            { 224243716, 1137683056, 226823104, 1139882364 },
            { 224888563, 1138232883, 226178257, 1139332537 },
            { 225210986, 1138507797, 225855834, 1139057623 },
            { 225372198, 1138645253, 225694622, 1138920167 },
            { 225452804, 1138713982, 225614016, 1138851438 }
        };
        return bounds[ClampZoomLevel(level)];
    }
    bool ProjectWgs84E7(int level, int32_t latitudeE7, int32_t longitudeE7, int* x, int* y) const
    {
        if (!x || !y) return false;
        const MapBounds& bounds = Bounds(level);
        if (latitudeE7 < bounds.southE7 || latitudeE7 > bounds.northE7 ||
            longitudeE7 < bounds.westE7 || longitudeE7 > bounds.eastE7)
        {
            return false;
        }

        const int64_t mapWidth = MapCanvasWidth - MapCanvasInset * 2;
        const int64_t mapHeight = MapCanvasHeight - MapCanvasInset * 2;
        *x = MapCanvasInset + static_cast<int>(
            (static_cast<int64_t>(longitudeE7) - bounds.westE7) * mapWidth /
            (static_cast<int64_t>(bounds.eastE7) - bounds.westE7));
        *y = MapCanvasInset + static_cast<int>(
            (static_cast<int64_t>(bounds.northE7) - latitudeE7) * mapHeight /
            (static_cast<int64_t>(bounds.northE7) - bounds.southE7));
        return true;
    }
    const char* MapPath(int level) const
    {
        level = ClampZoomLevel(level);
#ifdef _WIN32
        static const char* paths[] =
        {
            "/SD/MAP/shenzhen_z0.bin",
            "/SD/MAP/shenzhen_z1.bin",
            "/SD/MAP/shenzhen_z2.bin",
            "/SD/MAP/shenzhen.bin",
            "/SD/MAP/shenzhen_z4.bin",
            "/SD/MAP/shenzhen_z5.bin",
            "/SD/MAP/shenzhen_z6.bin"
        };
#else
        static const char* paths[] =
        {
            "S:/MAP/shenzhen_z0.bin",
            "S:/MAP/shenzhen_z1.bin",
            "S:/MAP/shenzhen_z2.bin",
            "S:/MAP/shenzhen.bin",
            "S:/MAP/shenzhen_z4.bin",
            "S:/MAP/shenzhen_z5.bin",
            "S:/MAP/shenzhen_z6.bin"
        };
#endif
        return paths[level];
    }
    int DistanceMeters() const { return 120; }
    int Minutes() const { return 2; }

private:
    CareGoClient client;
};
}
