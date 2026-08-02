/**
 * @file    Scene3D.h
 * @brief   3D 场景页定义
 * @details 本文件定义 EasyGo 的3D 场景页定义，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#ifndef __Scene3D_PRESENTER_H
#define __Scene3D_PRESENTER_H

#include "Scene3DView.h"
#include "Scene3DModel.h"

namespace Page
{
class Scene3D : public PageBase
{
public:
    typedef struct
    {
        uint16_t time;
        lv_color_t color;
    } Param_t;

public:
    Scene3D();
    virtual ~Scene3D();

    virtual void onCustomAttrConfig();
    virtual void onViewLoad();
    virtual void onViewDidLoad();
    virtual void onViewWillAppear();
    virtual void onViewDidAppear();
    virtual void onViewWillDisappear();
    virtual void onViewDidDisappear();
    virtual void onViewDidUnload();

private:
    void Update();
    void AttachEvent(lv_obj_t* obj);
    static void onTimerUpdate(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);

private:
    Scene3DView View;
    Scene3DModel Model;
    lv_timer_t* timer;
};

}

#endif
