/**
 * @file    Scene3DView.h
 * @brief   3D 场景页视图定义
 * @details 本文件定义 EasyGo 的3D 场景页视图定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#ifndef __SCENE3D_VIEW_H
#define __SCENE3D_VIEW_H

#include "App/Pages/Page.h"

namespace Page
{
    class Scene3DView
    {
    public:
        void Create(lv_obj_t* root);

    public:
        struct
        {
            lv_obj_t* labelTitle;
            lv_obj_t* canvas;
            lv_group_t* group;
        } ui;

    private:
    };
}

#endif // !__VIEW_H
