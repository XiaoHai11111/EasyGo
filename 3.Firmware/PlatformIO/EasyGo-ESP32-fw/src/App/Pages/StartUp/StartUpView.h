#pragma once

#include "../Common/EasyGoUi.h"

namespace Page
{
class StartupView
{
public:
    struct
    {
        lv_obj_t* logo;
        lv_obj_t* spinner;
        lv_anim_timeline_t* anim_timeline;
    } ui;

    void Create(lv_obj_t* root);
    void Delete();
};
}
