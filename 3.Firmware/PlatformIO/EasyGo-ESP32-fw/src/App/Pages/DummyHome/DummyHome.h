#pragma once
#ifndef __DUMMYHOME_PRESENTER_H
#define __DUMMYHOME_PRESENTER_H

#include "DummyHomeView.h"
#include "DummyHomeModel.h"

namespace Page
{

    class DummyHome : public PageBase
    {
    public:
        DummyHome();
        virtual ~DummyHome();

        virtual void onCustomAttrConfig();
        virtual void onViewLoad();
        virtual void onViewDidLoad();
        virtual void onViewWillAppear();
        virtual void onViewDidAppear();
        virtual void onViewWillDisappear();
        virtual void onViewDidDisappear();
        virtual void onViewUnload();
        virtual void onViewDidUnload();

    private:
        static void onTimerUpdate(lv_timer_t* timer);
        void AttachEvent(lv_obj_t* obj);
        static void onEvent(lv_event_t* event);
        void onBtnClicked(lv_obj_t* btn);

    private:
        DummyHomeView View;
        DummyHomeModel Model;
        lv_timer_t* timer;
        uint8_t btn_index;
    };

}
#endif
