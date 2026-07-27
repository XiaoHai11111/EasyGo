#ifndef __TEACH_PRESENTER_H
#define __TEACH_PRESENTER_H

#include "TeachView.h"
#include "TeachModel.h"

namespace Page
{

class Teach : public PageBase
{
public:
    typedef struct
    {
        uint16_t time;
        lv_color_t color;
    } Param_t;

public:
    Teach();
    virtual ~Teach();

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
    TeachView View;
    TeachModel Model;
    lv_timer_t* timer;
};

}

#endif
