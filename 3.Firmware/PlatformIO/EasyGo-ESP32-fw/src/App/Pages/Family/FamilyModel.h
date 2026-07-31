#pragma once

#include "lvgl.h"
#include <stdint.h>

namespace Page
{
class FamilyModel
{
public:
    static const uint8_t MaxContacts = 16;

    struct Contact
    {
        char name[32];
        char number[32];
        uint8_t avatarStyle;
    };

    void Init();
    void Deinit();
    uint8_t GetContactCount() const { return contactCount; }
    const Contact& GetContact(int index) const;

private:
    bool LoadFromSd();
    void LoadFallback();
    void AddContact(const char* name, const char* number, uint8_t avatarStyle);
    const char* ContactsPath() const;

    Contact contacts[MaxContacts];
    uint8_t contactCount;
};
}
