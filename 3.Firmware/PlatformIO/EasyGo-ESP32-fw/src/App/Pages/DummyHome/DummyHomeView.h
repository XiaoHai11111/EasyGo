#pragma once
#ifndef __DUMMYHOME_VIEW_H
#define __DUMMYHOME_VIEW_H

#include "../Page.h"

namespace Page
{
    class DummyHomeView
    {
    public:
        void Creat(lv_obj_t* root);
        void Delete();
        void AppearAnimStart(bool reverse = false);
    public:
        struct
        {   
            struct
            {
                lv_obj_t* cont;
                lv_obj_t* btnCtrl;
                lv_obj_t* btnTeach;
                lv_obj_t* btnMenu;
            }btnCont;

            lv_obj_t* bg_image;
            lv_group_t* group;
            lv_anim_timeline_t* anim_timeline;
        } ui;  
    private:
        void BtnCont_Create(lv_obj_t* par);
        lv_obj_t* Btn_Create(lv_obj_t* par, const void* img_src, lv_coord_t y_ofs);
    };

}
#endif // !__VIEW_H