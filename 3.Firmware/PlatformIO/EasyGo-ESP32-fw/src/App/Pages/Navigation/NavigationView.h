#pragma once

#include "../Common/EasyGoUi.h"

namespace Page
{
class NavigationView
{
public:
    struct
    {
        lv_obj_t* back;
        lv_obj_t* zoomIn;
        lv_obj_t* zoomOut;
        lv_obj_t* mapViewport;
        lv_obj_t* mapContent;
        lv_obj_t* mapImage;
        lv_obj_t* distance;
        lv_obj_t* minutes;
    } ui;

    void Create(lv_obj_t* root, const char* mapPath, int distanceMeters, int minutes);
    void SetMapSource(const char* mapPath);
    void Delete() {}

private:
    lv_point_t routePoints[5];
};
}
