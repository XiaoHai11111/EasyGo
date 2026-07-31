#include "FamilyModel.h"
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
    if (!LoadFromSd()) LoadFallback();
}

void FamilyModel::Deinit()
{
    contactCount = 0;
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

    return contactCount > 0;
}

void FamilyModel::LoadFallback()
{
    AddContact("女儿", "138 **** 5678", 0);
    AddContact("儿子", "139 **** 4321", 1);
    AddContact("老伴", "137 **** 3344", 2);
}

const FamilyModel::Contact& FamilyModel::GetContact(int index) const
{
    if (index < 0 || index >= contactCount) index = 0;
    return contacts[index];
}
