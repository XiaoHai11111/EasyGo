#pragma once

#include "lvgl.h"
#include "App/Services/CareGoClient.h"
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
    AccountSystem::CareGo_State_t State() const { return client.State(); }
    bool RequestSos();
    bool RequestSosForContact(uint8_t index);

private:
    bool LoadFromSd();
    void LoadFallback();
    void AddContact(const char* name, const char* number, uint8_t avatarStyle);
    bool CopyDialableNumber(const char* source, char* target, size_t targetSize) const;
    const char* ContactsPath() const;

    Contact contacts[MaxContacts];
    uint8_t contactCount;
    CareGoClient client;
    char sosPhone[24];
};
}
