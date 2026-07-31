#include "HAL/HAL.h"

#if CONFIG_AUDIO_USE_DFPLAYER

#include <HardwareSerial.h>
#include <string.h>

namespace
{
enum class InitState
{
    Disabled,
    WaitForPower,
    WaitAfterReset,
    WaitAfterDeviceSelect,
    Ready
};

struct TrackMapping
{
    const char* name;
    uint16_t number;
};

const TrackMapping tracks[] =
{
    {"Startup", 1},
    {"Shutdown", 2},
    {"Error", 3},
    {"Connect", 4},
    {"Disconnect", 5},
    {"UnstableConnect", 6},
    {"BattChargeStart", 7},
    {"BattChargeEnd", 8},
    {"DeviceInsert", 9},
    {"DevicePullout", 10},
    {"NoOperationWarning", 11}
};

InitState initState = InitState::Disabled;
uint32_t stateDeadline = 0;
uint32_t readySince = 0;
uint16_t pendingTrack = 0;
bool responseSeen = false;
bool missingResponseReported = false;

uint8_t receiveFrame[10] = {};
uint8_t receivePosition = 0;

void SendCommand(uint8_t command, uint16_t parameter, bool requestFeedback)
{
    uint8_t frame[10] =
    {
        0x7E,
        0xFF,
        0x06,
        command,
        requestFeedback ? static_cast<uint8_t>(0x01) : static_cast<uint8_t>(0x00),
        static_cast<uint8_t>(parameter >> 8),
        static_cast<uint8_t>(parameter & 0xFF),
        0,
        0,
        0xEF
    };

    const uint16_t checksum = static_cast<uint16_t>(
        0U - (frame[1] + frame[2] + frame[3] + frame[4] + frame[5] + frame[6]));
    frame[7] = static_cast<uint8_t>(checksum >> 8);
    frame[8] = static_cast<uint8_t>(checksum & 0xFF);
    Serial2.write(frame, sizeof(frame));
}

uint16_t TrackNumberForName(const char* name)
{
    if (name == nullptr)
        return 0;

    for (const TrackMapping& track : tracks)
    {
        if (strcmp(name, track.name) == 0)
            return track.number;
    }

    return 0;
}

void PlayTrack(uint16_t number)
{
    if (number == 0)
        return;

    /* Command 0x12 plays /mp3/NNNN.mp3 by its four-digit number. */
    SendCommand(0x12, number, true);
    Serial.printf("DFPlayer: play /mp3/%04u.mp3\r\n", number);
}

bool FrameChecksumValid(const uint8_t* frame)
{
    const uint16_t expected = static_cast<uint16_t>(
        0U - (frame[1] + frame[2] + frame[3] + frame[4] + frame[5] + frame[6]));
    const uint16_t received =
        static_cast<uint16_t>((frame[7] << 8) | frame[8]);
    return expected == received;
}

void HandleResponse(const uint8_t* frame)
{
    if (frame[0] != 0x7E || frame[9] != 0xEF || !FrameChecksumValid(frame))
    {
        Serial.println("DFPlayer: invalid UART response");
        return;
    }

    responseSeen = true;
    const uint8_t command = frame[3];
    const uint16_t value = static_cast<uint16_t>((frame[5] << 8) | frame[6]);

    switch (command)
    {
        case 0x3A:
            Serial.println("DFPlayer: SD card inserted");
            break;
        case 0x3B:
            Serial.println("DFPlayer: SD card removed");
            break;
        case 0x3D:
            Serial.printf("DFPlayer: track %u finished\r\n", value);
            break;
        case 0x3F:
            Serial.printf("DFPlayer: storage online, mask=0x%04X\r\n", value);
            break;
        case 0x40:
            Serial.printf("DFPlayer: module error 0x%04X\r\n", value);
            break;
        case 0x41:
            /* Normal command acknowledgement. */
            break;
        default:
            Serial.printf(
                "DFPlayer: response command=0x%02X value=0x%04X\r\n",
                command,
                value);
            break;
    }
}

void PollResponses()
{
    while (Serial2.available() > 0)
    {
        const uint8_t value = static_cast<uint8_t>(Serial2.read());

        if (receivePosition == 0 && value != 0x7E)
            continue;

        receiveFrame[receivePosition++] = value;
        if (receivePosition == sizeof(receiveFrame))
        {
            HandleResponse(receiveFrame);
            receivePosition = 0;
        }
    }
}
}

void HAL::Audio_Init()
{
    pinMode(CONFIG_DFPLAYER_BUSY_PIN, INPUT);
    Serial2.begin(
        9600,
        SERIAL_8N1,
        CONFIG_DFPLAYER_RX_PIN,
        CONFIG_DFPLAYER_TX_PIN);

    while (Serial2.available() > 0)
        Serial2.read();

    initState = InitState::WaitForPower;
    stateDeadline = millis() + 2000;
    pendingTrack = 0;
    responseSeen = false;
    missingResponseReported = false;
    receivePosition = 0;

    Serial.println("DFPlayer: waiting for module and SD card...");
}

void HAL::Audio_Update()
{
    PollResponses();

    const uint32_t now = millis();
    if (static_cast<int32_t>(now - stateDeadline) < 0 &&
        initState != InitState::Ready)
    {
        return;
    }

    switch (initState)
    {
        case InitState::WaitForPower:
            SendCommand(0x0C, 0, false);
            initState = InitState::WaitAfterReset;
            stateDeadline = now + 1500;
            break;

        case InitState::WaitAfterReset:
            /* Select the microSD/TF card as the playback device. */
            SendCommand(0x09, 0x0002, true);
            initState = InitState::WaitAfterDeviceSelect;
            stateDeadline = now + 500;
            break;

        case InitState::WaitAfterDeviceSelect:
            SendCommand(0x06, CONFIG_DFPLAYER_VOLUME, true);
            initState = InitState::Ready;
            readySince = now;
            Serial.printf(
                "DFPlayer: initialization commands sent, volume=%d\r\n",
                CONFIG_DFPLAYER_VOLUME);

            if (pendingTrack != 0)
            {
                PlayTrack(pendingTrack);
                pendingTrack = 0;
            }
            break;

        case InitState::Ready:
            if (!responseSeen &&
                !missingResponseReported &&
                now - readySince >= 3000)
            {
                missingResponseReported = true;
                Serial.println(
                    "DFPlayer: no UART response; check SD card, RX/TX crossing and power.");
            }
            break;

        case InitState::Disabled:
        default:
            break;
    }
}

bool HAL::Audio_PlayMusic(const char* name)
{
    const uint16_t number = TrackNumberForName(name);
    if (number == 0)
    {
        Serial.printf("DFPlayer: no track mapping for '%s'\r\n", name ? name : "(null)");
        return false;
    }

    if (initState == InitState::Ready)
        PlayTrack(number);
    else
        pendingTrack = number;

    return true;
}

#else

#include "App/Utils/TonePlayer/TonePlayer.h"
#include "App/Utils/TonePlayer/MusicCode.h"
#include "lvgl.h"

static TonePlayer player;

static void Tone_Callback(uint32_t freq, uint16_t volume)
{
    HAL::Buzz_Tone(freq);
}

void HAL::Audio_Init()
{
    player.SetCallback(Tone_Callback);
}

void HAL::Audio_Update()
{
    player.Update(lv_tick_get());
}

bool HAL::Audio_PlayMusic(const char* name)
{
    for (const MusicList_t& music : MusicList)
    {
        if (strcmp(name, music.name) == 0)
        {
            player.Play(music.mc, music.length);
            return true;
        }
    }

    return false;
}

#endif
