#include "HAL/HAL.h"
#include <Wire.h>

namespace
{
enum class TouchController
{
    None,
    FT6x36,
    CST816,
    GT911
};

TouchController controller = TouchController::None;
uint8_t controllerAddress = 0;
bool lastPressed = false;
uint16_t lastX = 0;
uint16_t lastY = 0;

bool Probe(uint8_t address)
{
    Wire.beginTransmission(address);
    return Wire.endTransmission() == 0;
}

bool ReadRegister8(uint8_t address, uint8_t reg, uint8_t* data, size_t length)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0)
        return false;

    if (Wire.requestFrom(address, static_cast<uint8_t>(length)) != length)
        return false;

    for (size_t i = 0; i < length; ++i)
        data[i] = Wire.read();

    return true;
}

bool ReadRegister16(uint8_t address, uint16_t reg, uint8_t* data, size_t length)
{
    Wire.beginTransmission(address);
    Wire.write(static_cast<uint8_t>(reg >> 8));
    Wire.write(static_cast<uint8_t>(reg & 0xFF));
    if (Wire.endTransmission(false) != 0)
        return false;

    if (Wire.requestFrom(address, static_cast<uint8_t>(length)) != length)
        return false;

    for (size_t i = 0; i < length; ++i)
        data[i] = Wire.read();

    return true;
}

bool WriteRegister16(uint8_t address, uint16_t reg, uint8_t value)
{
    Wire.beginTransmission(address);
    Wire.write(static_cast<uint8_t>(reg >> 8));
    Wire.write(static_cast<uint8_t>(reg & 0xFF));
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

void TransformPoint(uint16_t* x, uint16_t* y)
{
#if CONFIG_TOUCH_SWAP_XY
    const uint16_t tmp = *x;
    *x = *y;
    *y = tmp;
#endif

#if CONFIG_TOUCH_INVERT_X
    *x = *x < CONFIG_SCREEN_HOR_RES
        ? CONFIG_SCREEN_HOR_RES - 1 - *x
        : 0;
#endif

#if CONFIG_TOUCH_INVERT_Y
    *y = *y < CONFIG_SCREEN_VER_RES
        ? CONFIG_SCREEN_VER_RES - 1 - *y
        : 0;
#endif

    if (*x >= CONFIG_SCREEN_HOR_RES)
        *x = CONFIG_SCREEN_HOR_RES - 1;
    if (*y >= CONFIG_SCREEN_VER_RES)
        *y = CONFIG_SCREEN_VER_RES - 1;
}

bool ReadFT6x36OrCST816(uint16_t* x, uint16_t* y)
{
    uint8_t data[5] = {};
    if (!ReadRegister8(controllerAddress, 0x02, data, sizeof(data)))
        return false;

    const uint8_t touches = data[0] & 0x0F;
    if (touches == 0)
        return false;

    *x = static_cast<uint16_t>(((data[1] & 0x0F) << 8) | data[2]);
    *y = static_cast<uint16_t>(((data[3] & 0x0F) << 8) | data[4]);
    return true;
}

bool ReadGT911(uint16_t* x, uint16_t* y)
{
    uint8_t status = 0;
    if (!ReadRegister16(controllerAddress, 0x814E, &status, 1))
        return false;

    if ((status & 0x80) == 0)
    {
        if (lastPressed)
        {
            *x = lastX;
            *y = lastY;
        }
        return lastPressed;
    }

    const uint8_t touches = status & 0x0F;
    if (touches == 0)
    {
        lastPressed = false;
        WriteRegister16(controllerAddress, 0x814E, 0);
        return false;
    }

    uint8_t point[4] = {};
    const bool ok = ReadRegister16(controllerAddress, 0x8150, point, sizeof(point));
    WriteRegister16(controllerAddress, 0x814E, 0);
    if (!ok)
        return false;

    *x = static_cast<uint16_t>(point[0] | (point[1] << 8));
    *y = static_cast<uint16_t>(point[2] | (point[3] << 8));
    return true;
}
}

bool HAL::Touch_Init()
{
#if !CONFIG_ENABLE_TOUCH
    return false;
#else
    /*
     * Driving INT low during reset also selects GT911 address 0x5D.
     * It is harmless for the FT6x36/CST816 modules covered by this adapter.
     */
    pinMode(CONFIG_TOUCH_INT_PIN, OUTPUT);
    digitalWrite(CONFIG_TOUCH_INT_PIN, LOW);
    pinMode(CONFIG_TOUCH_RST_PIN, OUTPUT);
    digitalWrite(CONFIG_TOUCH_RST_PIN, LOW);
    delay(10);
    digitalWrite(CONFIG_TOUCH_RST_PIN, HIGH);
    delay(60);
    pinMode(CONFIG_TOUCH_INT_PIN, INPUT_PULLUP);

    if (Probe(0x38))
    {
        controller = TouchController::FT6x36;
        controllerAddress = 0x38;
        Serial.println("Touch: FT6x36-compatible controller at 0x38");
    }
    else if (Probe(0x15))
    {
        controller = TouchController::CST816;
        controllerAddress = 0x15;
        Serial.println("Touch: CST816-compatible controller at 0x15");
    }
    else if (Probe(0x5D) || Probe(0x14))
    {
        controller = TouchController::GT911;
        controllerAddress = Probe(0x5D) ? 0x5D : 0x14;
        Serial.printf("Touch: GT911-compatible controller at 0x%02X\r\n", controllerAddress);
    }
    else
    {
        controller = TouchController::None;
        Serial.println("Touch: no supported controller detected.");
        Serial.println("Touch: scanning I2C bus; expected 0x38, 0x15, 0x5D or 0x14.");
        for (uint8_t address = 1; address < 127; ++address)
        {
            if (Probe(address))
                Serial.printf("Touch: I2C device found at 0x%02X\r\n", address);
        }
        return false;
    }

    return true;
#endif
}

bool HAL::Touch_GetPoint(uint16_t* x, uint16_t* y)
{
#if !CONFIG_ENABLE_TOUCH
    return false;
#else
    if (controller == TouchController::None || x == nullptr || y == nullptr)
        return false;

    bool pressed = false;
    if (controller == TouchController::GT911)
        pressed = ReadGT911(x, y);
    else
        pressed = ReadFT6x36OrCST816(x, y);

    if (pressed)
    {
        TransformPoint(x, y);
        lastX = *x;
        lastY = *y;
    }

    lastPressed = pressed;
    return pressed;
#endif
}
