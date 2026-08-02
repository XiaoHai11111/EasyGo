#ifndef __DATA_PROC_DEF_H
#define __DATA_PROC_DEF_H

#include <stdint.h>

namespace AccountSystem
{

enum Location_Source_t
{
    LOCATION_SOURCE_NONE,
    LOCATION_SOURCE_GPS,
    LOCATION_SOURCE_LBS,
    LOCATION_SOURCE_SIMULATOR
};

typedef struct
{
    bool valid;
    Location_Source_t source;
    double latitude;
    double longitude;
    float altitudeMeters;
    float speedMetersPerSecond;
    float courseDegrees;
    float horizontalAccuracyMeters;
    float hdop;
    uint8_t satellites;
    uint8_t fixType;
    bool timeValid;
    uint8_t utcHour;
    uint8_t utcMinute;
    uint8_t utcSecond;
    uint32_t clockUpdatedAtMs;
    uint32_t updatedAtMs;
} Location_Info_t;

enum Network_Link_t
{
    NETWORK_LINK_NONE,
    NETWORK_LINK_WIFI,
    NETWORK_LINK_CELLULAR
};

enum Network_Event_t
{
    NETWORK_EVENT_STATUS,
    NETWORK_EVENT_NEARBY_RESULT,
    NETWORK_EVENT_SMS_RESULT,
    NETWORK_EVENT_LBS_RESULT
};

typedef struct
{
    Network_Link_t link;
    Network_Event_t event;
    bool registered;
    bool busy;
    bool success;
    bool timeValid;
    uint8_t localHour;
    uint8_t localMinute;
    uint8_t localSecond;
    uint32_t clockUpdatedAtMs;
    int16_t signalDbm;
    int16_t errorCode;
    uint32_t requestId;
    int32_t distanceMeters;
    float locationAccuracyMeters;
    double targetLatitude;
    double targetLongitude;
    char operatorName[20];
    char targetName[40];
    char detail[72];
} Network_Info_t;

enum Network_Command_Code_t
{
    NETWORK_CMD_QUERY_NEARBY,
    NETWORK_CMD_SEND_SMS,
    NETWORK_CMD_QUERY_LBS
};

typedef struct
{
    Network_Command_Code_t cmd;
    uint32_t requestId;
    double latitude;
    double longitude;
    char phone[24];
    char message[160];
} Network_Command_t;

enum Input_Action_t
{
    INPUT_NONE,
    INPUT_TOILET_CLICK,
    INPUT_HELP_AUDIO_DOUBLE_CLICK,
    INPUT_TOILET_LONG_PRESS,
    INPUT_HOME_CLICK,
    INPUT_SOS_DOUBLE_CLICK,
    INPUT_HOME_LONG_PRESS
};

typedef struct
{
    Input_Action_t action;
    uint32_t timestampMs;
} Input_Info_t;

enum CareGo_Command_Code_t
{
    CAREGO_CMD_NAVIGATE_TOILET,
    CAREGO_CMD_NAVIGATE_HOME,
    CAREGO_CMD_PLAY_HELP_AUDIO,
    CAREGO_CMD_SEND_SOS,
    CAREGO_CMD_SET_HOME_HERE,
    CAREGO_CMD_SET_VOICE_ENABLED,
    CAREGO_CMD_STOP_NAVIGATION
};

typedef struct
{
    CareGo_Command_Code_t cmd;
    bool enabled;
    char phone[24];
} CareGo_Command_t;

enum CareGo_Navigation_State_t
{
    CAREGO_NAV_IDLE,
    CAREGO_NAV_LOCATING,
    CAREGO_NAV_SEARCHING,
    CAREGO_NAV_NAVIGATING_TOILET,
    CAREGO_NAV_NAVIGATING_HOME,
    CAREGO_NAV_ARRIVED,
    CAREGO_NAV_FAILED
};

enum CareGo_Sos_State_t
{
    CAREGO_SOS_IDLE,
    CAREGO_SOS_SENDING,
    CAREGO_SOS_ACCEPTED,
    CAREGO_SOS_FAILED,
    CAREGO_SOS_NOT_CONFIGURED
};

typedef struct
{
    CareGo_Navigation_State_t navigation;
    CareGo_Sos_State_t sos;
    Location_Info_t location;
    Network_Info_t network;
    bool voiceEnabled;
    bool audioPlaying;
    bool sdPresent;
    bool homeConfigured;
    double homeLatitude;
    double homeLongitude;
    int32_t distanceMeters;
    int16_t minutes;
    char targetName[40];
    char statusText[72];
} CareGo_State_t;

/* Recorder */
typedef enum
{
    RECORDER_CMD_START,
    RECORDER_CMD_PAUSE,
    RECORDER_CMD_CONTINUE,
    RECORDER_CMD_STOP,
} Recorder_Cmd_t;

typedef struct
{
    Recorder_Cmd_t cmd;
    uint16_t time;
} Recorder_Info_t;

/* Storage */
typedef enum
{
    STORAGE_CMD_LOAD,
    STORAGE_CMD_SAVE,
    STORAGE_CMD_ADD,
    STORAGE_CMD_REMOVE
} Storage_Cmd_t;

typedef enum
{
    STORAGE_TYPE_UNKNOW,
    STORAGE_TYPE_INT,
    STORAGE_TYPE_FLOAT,
    STORAGE_TYPE_DOUBLE,
    STORAGE_TYPE_STRING
} Storage_Type_t;

typedef struct
{
    Storage_Cmd_t cmd;
    const char* key;
    void* value;
    uint16_t size;
    Storage_Type_t type;
} Storage_Info_t;

#define STORAGE_VALUE_REG(act, data, dataType)\
do{\
    AccountSystem::Storage_Info_t info; \
    info.cmd = AccountSystem::STORAGE_CMD_ADD; \
    info.key = #data; \
    info.value = &data; \
    info.size = sizeof(data); \
    info.type = dataType; \
    act->Notify("Storage", &info, sizeof(info)); \
}while(0)

typedef struct
{
    bool isDetect;
    float totalSizeMB;
    float freeSizeMB;
} Storage_Basic_Info_t;

/* StatusBar */
typedef struct
{
    bool showLabelRec;
    const char* labelRecStr;
} StatusBar_Info_t;

/* MusicPlayer */
typedef struct
{
    const char* music;
} MusicPlayer_Info_t;

typedef struct
{
    bool playing;
    bool storagePresent;
} MusicPlayer_State_t;

/* SysConfig */
typedef enum
{
    SYSCONFIG_CMD_LOAD,
    SYSCONFIG_CMD_SAVE,
} SysConfig_Cmd_t;

typedef struct
{
    SysConfig_Cmd_t cmd;
    bool soundEnable;
    double longitudeDefault;
    double latitudeDefault;
    char language[8];
    char mapDirPath[32];
    bool WGS84;
    char arrowTheme[8];
} SysConfig_Info_t;

/* TrackFilter */
typedef enum
{
    TRACK_FILTER_CMD_START = RECORDER_CMD_START,
    TRACK_FILTER_CMD_PAUSE = RECORDER_CMD_PAUSE,
    TRACK_FILTER_CMD_CONTINUE = RECORDER_CMD_CONTINUE,
    TRACK_FILTER_CMD_STOP = RECORDER_CMD_STOP,
} TrackFilter_Cmd_t;

typedef struct
{
    float longitude;
    float latitude;
} TrackFilter_Point_t;

typedef struct
{
    TrackFilter_Cmd_t cmd;
    TrackFilter_Point_t* points;
    uint32_t size;
    bool isActive;
} TrackFilter_Info_t;

}

#endif
