#ifndef __CTRL_VIEW_H
#define __CTRL_VIEW_H

#include "../Page.h"

namespace Page
{

class CtrlView
{
public:
    void Creat(lv_obj_t* root);

public:
    struct
    {
        lv_obj_t* canvas;
        lv_anim_timeline_t* anim_timeline;
        lv_group_t* group;
        

        struct
        {
            lv_obj_t* cont;
            lv_obj_t* btnBack;
            lv_obj_t* btnReset;
            lv_obj_t* btnRecoder;
            lv_obj_t* btnPlay;
        }btnCont;

        struct
        {
            lv_obj_t* cont;
            lv_obj_t* J1_Slider;
            lv_obj_t* J2_Slider;
            lv_obj_t* J3_Slider;
            lv_obj_t* J4_Slider;
            lv_obj_t* J5_Slider;
            lv_obj_t* J6_Slider;
        }jointsSlider;

        struct
        {
            lv_obj_t* cont;
            lv_obj_t* J1_label;
            lv_obj_t* J2_label;
            lv_obj_t* J3_label;
            lv_obj_t* J4_label;
            lv_obj_t* J5_label;
            lv_obj_t* J6_label;
        }jointsLabel;

        struct
        {
            lv_obj_t* cont;
            lv_obj_t* J1value_label;
            lv_obj_t* J2value_label;
            lv_obj_t* J3value_label;
            lv_obj_t* J4value_label;
            lv_obj_t* J5value_label;
            lv_obj_t* J6value_label;
        }valueLabel;

    } ui;
public:
    void AppearAnimStart(bool reverse = false);

private:
    void BtnCont_Create(lv_obj_t* par);
    lv_obj_t* Btn_Create(lv_obj_t* par, const void* img1_src, lv_coord_t x_ofs);
    void CtrlLabelCont_Create(lv_obj_t* par);
    lv_obj_t* CtrlLabel_Create(lv_obj_t* par, const char* text, lv_coord_t y_ofs);
    void ValueLabelCont_Create(lv_obj_t* par);
    lv_obj_t* ValueLabel_Create(lv_obj_t* par, const char* text, lv_coord_t y_ofs);
    void CtrlSliderCont_Create(lv_obj_t* par);
    lv_obj_t* CtrlSlider_Create(lv_obj_t* par, int32_t left_value, int32_t cur_value, int32_t right_value, lv_coord_t y_ofs);
};

}

#endif // !__VIEW_H
