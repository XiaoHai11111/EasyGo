#pragma once

#include "../Common/EasyGoUi.h"

namespace Page
{
class HomeView
{
public:
    struct
    {
        lv_obj_t* navigation;
        lv_obj_t* family;
        lv_obj_t* setting;
    } ui;

    void Create(lv_obj_t* root);
    void Delete() {}

private:
    lv_obj_t* CreateCard(lv_obj_t* root, lv_coord_t y, lv_color_t color,
                         int iconType, const char* title);
    void CreateLocationIcon(lv_obj_t* parent, lv_color_t color);
    void CreateFamilyIcon(lv_obj_t* parent, lv_color_t color);
};
}
