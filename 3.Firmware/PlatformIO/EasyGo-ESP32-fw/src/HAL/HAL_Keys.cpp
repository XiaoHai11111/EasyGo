#include "HAL/HAL.h"
#include "App/Accounts/Account_Master.h"

namespace
{
struct KeyState
{
    uint8_t pin;
    bool rawPressed;
    bool stablePressed;
    bool longSent;
    uint8_t clickCount;
    uint32_t rawChangedAt;
    uint32_t pressedAt;
    uint32_t releasedAt;
};

KeyState toiletKey = { CONFIG_KEY_TOILET_PIN };
KeyState homeKey = { CONFIG_KEY_HOME_PIN };
uint32_t keysReleasedAt = 0;
bool keysArmed = false;

void Publish(AccountSystem::Input_Action_t action)
{
    AccountSystem::Input_Info_t info = { action, millis() };
    Serial.printf("Keys: action=%u\r\n", static_cast<unsigned>(action));
    AccountSystem::Input_Commit(&info);
}

void Scan(KeyState& key, bool firstKey)
{
    const uint32_t now = millis();
    const bool pressed = digitalRead(key.pin) == LOW;
    if (pressed != key.rawPressed)
    {
        key.rawPressed = pressed;
        key.rawChangedAt = now;
    }

    if (now - key.rawChangedAt >= CONFIG_KEY_DEBOUNCE_MS && pressed != key.stablePressed)
    {
        key.stablePressed = pressed;
        if (pressed)
        {
            key.pressedAt = now;
            key.longSent = false;
        }
        else if (!key.longSent)
        {
            ++key.clickCount;
            key.releasedAt = now;
            if (key.clickCount >= 2)
            {
                Publish(firstKey ? AccountSystem::INPUT_HELP_AUDIO_DOUBLE_CLICK : AccountSystem::INPUT_SOS_DOUBLE_CLICK);
                key.clickCount = 0;
            }
        }
    }

    if (key.stablePressed && !key.longSent && now - key.pressedAt >= CONFIG_KEY_LONG_MS)
    {
        key.longSent = true;
        key.clickCount = 0;
        Publish(firstKey ? AccountSystem::INPUT_TOILET_LONG_PRESS : AccountSystem::INPUT_HOME_LONG_PRESS);
    }

    if (!key.stablePressed && key.clickCount == 1 && now - key.releasedAt >= CONFIG_KEY_DOUBLE_MS)
    {
        Publish(firstKey ? AccountSystem::INPUT_TOILET_CLICK : AccountSystem::INPUT_HOME_CLICK);
        key.clickCount = 0;
    }
}
}

void HAL::Keys_Init()
{
    pinMode(CONFIG_KEY_TOILET_PIN, INPUT_PULLUP);
    pinMode(CONFIG_KEY_HOME_PIN, INPUT_PULLUP);
    toiletKey.rawPressed = toiletKey.stablePressed = digitalRead(toiletKey.pin) == LOW;
    homeKey.rawPressed = homeKey.stablePressed = digitalRead(homeKey.pin) == LOW;
    keysReleasedAt = 0;
    keysArmed = false;
    Serial.printf(
        "Keys: GPIO%d toilet/audio, GPIO%d BOOT home/SOS; internal pull-ups enabled\r\n",
        CONFIG_KEY_TOILET_PIN,
        CONFIG_KEY_HOME_PIN);
}

void HAL::Keys_Update()
{
    const uint32_t now = millis();
    const bool toiletReleased = !CONFIG_KEY_TOILET_ENABLED || digitalRead(toiletKey.pin) == HIGH;
    const bool homeReleased = !CONFIG_KEY_HOME_ENABLED || digitalRead(homeKey.pin) == HIGH;

    /* Never turn an initially held/floating key into a synthetic click. */
    if (!keysArmed)
    {
        toiletKey.rawPressed = toiletKey.stablePressed = !toiletReleased;
        homeKey.rawPressed = homeKey.stablePressed = !homeReleased;
        toiletKey.clickCount = homeKey.clickCount = 0;

        if (!toiletReleased || !homeReleased)
        {
            keysReleasedAt = 0;
            return;
        }
        if (keysReleasedAt == 0)
        {
            keysReleasedAt = now;
            return;
        }
        if (now - keysReleasedAt < CONFIG_KEY_ARM_RELEASE_MS) return;

        toiletKey.rawChangedAt = homeKey.rawChangedAt = now;
        keysArmed = true;
        Serial.println("Keys: armed after stable release");
    }

    if (CONFIG_KEY_TOILET_ENABLED) Scan(toiletKey, true);
    if (CONFIG_KEY_HOME_ENABLED) Scan(homeKey, false);
}
