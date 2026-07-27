#ifndef __CTRL_PRESENTER_H
#define __CTRL_PRESENTER_H

#include "CtrlView.h"
#include "CtrlModel.h"

namespace Page
{

class Ctrl : public PageBase
{
public:

public:
    Ctrl();
    virtual ~Ctrl();

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
    void onBtnClicked(lv_obj_t* btn);

private:
    CtrlView View;
    CtrlModel Model;
    lv_timer_t* timer;
};

}

#endif
