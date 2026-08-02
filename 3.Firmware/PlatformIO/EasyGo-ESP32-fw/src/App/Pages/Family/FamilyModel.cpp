/**
 * @file    FamilyModel.cpp
 * @brief   亲情联系人页数据模型实现
 * @details 本文件实现 EasyGo 的亲情联系人页数据模型实现，遵循 Page/View/Model 分层架构，固件与模拟器共用。
 * @author  CareBridge
 * @date    2026-08-02
 * @version 0.0.1
 */

#include "FamilyModel.h"
#include "App/Configs/Config.h"
#include <stdlib.h>
#include <string.h>

using namespace Page;

const char* FamilyModel::ContactsPath() const
{
#ifdef _WIN32
    return "/SD/CONTACTS/contacts.csv";
#else
    return "S:/CONTACTS/contacts.csv";
#endif
}

void FamilyModel::Init()
{
    contactCount = 0;
    sosPhone[0] = '\0';
    AddContact("紧急联系人", CONFIG_EMERGENCY_PHONE_DEFAULT, 3);
    if (!LoadFromSd()) LoadFallback();
    CopyDialableNumber(CONFIG_EMERGENCY_PHONE_DEFAULT, sosPhone, sizeof(sosPhone));
    for (uint8_t i = 0; i < contactCount && !sosPhone[0]; ++i)
    {
        CopyDialableNumber(contacts[i].number, sosPhone, sizeof(sosPhone));
    }
    client.Init("FamilyModel");
}

void FamilyModel::Deinit()
{
    contactCount = 0;
}

bool FamilyModel::RequestSos()
{
    return client.Send(AccountSystem::CAREGO_CMD_SEND_SOS, false, sosPhone);
}

bool FamilyModel::RequestSosForContact(uint8_t index)
{
    if (index >= contactCount) return false;
    char phone[24] = {};
    if (!CopyDialableNumber(contacts[index].number, phone, sizeof(phone))) return false;
    return client.Send(AccountSystem::CAREGO_CMD_SEND_SOS, false, phone);
}

bool FamilyModel::CopyDialableNumber(const char* source, char* target, size_t targetSize) const
{
    if (!source || !target || targetSize == 0 || strchr(source, '*')) return false;
    size_t write = 0;
    for (const char* p = source; *p && write + 1 < targetSize; ++p)
        if ((*p >= '0' && *p <= '9') || (*p == '+' && write == 0)) target[write++] = *p;
    target[write] = '\0';
    if (write >= 5) return true;
    target[0] = '\0';
    return false;
}

void FamilyModel::AddContact(const char* name, const char* number, uint8_t avatarStyle)
{
    if (contactCount >= MaxContacts || name == nullptr || number == nullptr) return;

    Contact& contact = contacts[contactCount++];
    strncpy(contact.name, name, sizeof(contact.name) - 1);
    contact.name[sizeof(contact.name) - 1] = '\0';
    strncpy(contact.number, number, sizeof(contact.number) - 1);
    contact.number[sizeof(contact.number) - 1] = '\0';
    contact.avatarStyle = avatarStyle;
}

bool FamilyModel::LoadFromSd()
{
    const uint8_t countBeforeLoad = contactCount;
    lv_fs_file_t file;
    if (lv_fs_open(&file, ContactsPath(), LV_FS_MODE_RD) != LV_FS_RES_OK) return false;

    char buffer[4096];
    uint32_t totalRead = 0;
    while (totalRead < sizeof(buffer) - 1)
    {
        uint32_t bytesRead = 0;
        lv_fs_res_t result = lv_fs_read(
            &file,
            buffer + totalRead,
            static_cast<uint32_t>(sizeof(buffer) - 1 - totalRead),
            &bytesRead);
        if (result != LV_FS_RES_OK || bytesRead == 0) break;
        totalRead += bytesRead;
    }
    lv_fs_close(&file);
    buffer[totalRead] = '\0';

    char* cursor = buffer;
    while (*cursor != '\0' && contactCount < MaxContacts)
    {
        char* line = cursor;
        char* lineEnd = strchr(cursor, '\n');
        if (lineEnd != nullptr)
        {
            *lineEnd = '\0';
            cursor = lineEnd + 1;
        }
        else
        {
            cursor += strlen(cursor);
        }

        size_t lineLength = strlen(line);
        if (lineLength > 0 && line[lineLength - 1] == '\r') line[lineLength - 1] = '\0';
        if (*line == '\0' || *line == '#') continue;

        char* firstComma = strchr(line, ',');
        if (firstComma == nullptr) continue;
        *firstComma = '\0';

        char* secondComma = strchr(firstComma + 1, ',');
        if (secondComma == nullptr) continue;
        *secondComma = '\0';

        uint8_t avatarStyle = static_cast<uint8_t>(atoi(secondComma + 1));
        AddContact(line, firstComma + 1, avatarStyle);
    }

    return contactCount > countBeforeLoad;
}

void FamilyModel::LoadFallback()
{
    AddContact("xiaohai", "133 5293 4299", 0);
    AddContact("儿子", "139 **** 4321", 1);
    AddContact("老伴", "137 **** 3344", 2);
}

const FamilyModel::Contact& FamilyModel::GetContact(int index) const
{
    if (index < 0 || index >= contactCount) index = 0;
    return contacts[index];
}
