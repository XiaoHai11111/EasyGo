#pragma once

#include "../Common/EasyGoUi.h"
#include "FamilyModel.h"

namespace Page
{
class FamilyView
{
public:
    struct
    {
        lv_obj_t* back;
        lv_obj_t* list;
        lv_obj_t* call[FamilyModel::MaxContacts];
        lv_obj_t* sos;
        lv_obj_t* feedback;
        uint8_t contactCount;
    } ui;

    void Create(lv_obj_t* root, const FamilyModel& model);
    void ShowFeedback(const char* text, lv_color_t color);
    void Delete() {}

private:
    lv_obj_t* CreateContact(lv_obj_t* root, int index, const FamilyModel::Contact& contact);
};
}
