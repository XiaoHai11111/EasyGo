#ifndef __CONFIG_H
#define __CONFIG_H

/*=========================
   Application configuration
 *=========================*/

#define CONFIG_SYSTEM_SAVE_FILE_PATH          "S:/SystemSave.json"
#define CONFIG_SYSTEM_LANGUAGE_DEFAULT        {'e','n','-','G','B'}
#define CONFIG_SYSTEM_GMT_OFFSET_DEFAULT      8 // GMT+ 8

#define CONFIG_WEIGHT_DEFAULT                 65 // kg

#define CONFIG_GPS_REFR_PERIOD                1000 // ms
#define CONFIG_GPS_LNG_DEFAULT                116.391332
#define CONFIG_GPS_LAT_DEFAULT                39.907415

#define CONFIG_TRACK_FILTER_OFFSET_THRESHOLD  2 // pixel
#define CONFIG_TRACK_RECORD_FILE_DIR_NAME     "Track"

#define CONFIG_MAP_USE_WGS84_DEFAULT          false

#define CONFIG_MAP_DIR_PATH                   {'/','M','A','P'}//"/MAP"

#define CONFIG_ARROW_THEME_DEFAULT            "default"

#define CONFIG_LIVE_MAP_LEVEL_DEFAULT         16
#define CONFIG_LIVE_MAP_VIEW_WIDTH            LV_HOR_RES
#define CONFIG_LIVE_MAP_VIEW_HEIGHT           LV_VER_RES

/* Simulator */
#define CONFIG_TRACK_VIRTUAL_GPX_FILE_PATH    "S:/TRK_20210801_203324.gpx"

/*=========================
   Hardware Configuration
 *=========================*/

#define NULL_PIN                    PD0

/*
 * ESP32 Dev Module bring-up profile.
 *
 * The original project targeted a different board and initialized peripherals
 * on pins that conflict with the ST7789 display and I2C capacitive touch panel.
 * Keep those peripherals disabled until their wiring is redesigned for this
 * board.
 */
#define CONFIG_ENABLE_BLUETOOTH     0
#define CONFIG_ENABLE_AUDIO         1
#define CONFIG_ENABLE_SD_CARD       0
#define CONFIG_ENABLE_IMU           0
#define CONFIG_ENABLE_TOUCH         1
#define CONFIG_ENABLE_GPS           1
#define CONFIG_ENABLE_CELLULAR      1
#define CONFIG_ENABLE_KEYS          1

/* Screen */
#define CONFIG_SCREEN_CS_PIN        5
#define CONFIG_SCREEN_DC_PIN        27
#define CONFIG_SCREEN_RST_PIN       16
#define CONFIG_SCREEN_SCK_PIN       18
#define CONFIG_SCREEN_MOSI_PIN      23
#define CONFIG_SCREEN_MISO_PIN      19
/* Connect the module LED pin to 3V3 for first bring-up. */
#define CONFIG_SCREEN_BLK_PIN       -1

#define CONFIG_SCREEN_HOR_RES       240
#define CONFIG_SCREEN_VER_RES       320
#define CONFIG_SCREEN_BUFFER_SIZE   (CONFIG_SCREEN_HOR_RES * CONFIG_SCREEN_VER_RES /2)
#define CONFIG_SCREEN_ROTATION      0
#define CONFIG_SCREEN_SELF_TEST     1

#define CONFIG_SOUND_ENABLE_DEFAULT true

/*
 * DFPlayer Mini UART2 connection (pin names are from the ESP32 side):
 * ESP32 TX17 -- 1k resistor --> DFPlayer RX
 * ESP32 RX26 <--------------- DFPlayer TX
 * ESP32 GPIO34 <------------- DFPlayer BUSY (optional)
 */
#define CONFIG_AUDIO_USE_DFPLAYER   1
#define CONFIG_DFPLAYER_RX_PIN      26
#define CONFIG_DFPLAYER_TX_PIN      17
#define CONFIG_DFPLAYER_BUSY_PIN    34
#define CONFIG_DFPLAYER_VOLUME      20

/* GPS: input-only software serial. GPIO36 is connected to GPS TX. */
#define CONFIG_GPS_RX_PIN           36
#define CONFIG_GPS_BAUD             9600
#define CONFIG_GPS_MAX_LINE_LENGTH  160
#define CONFIG_TIMEZONE_OFFSET_MINUTES 480

/* A7670/FS-MCore 4G module on UART1 (ESP32 pin names). */
#define CONFIG_CELLULAR_RX_PIN      14
#define CONFIG_CELLULAR_TX_PIN      13
#define CONFIG_CELLULAR_BAUD        115200
#define CONFIG_CELLULAR_APN         "cmnet"
#define CONFIG_CELLULAR_SERVER      "134.175.109.168"
#define CONFIG_CELLULAR_SERVER_PORT 5000
/* Optional phone hotspot. Empty SSID keeps Wi-Fi disabled and uses 4G only. */
// #define CONFIG_WIFI_SSID            "XiaoHai"
// #define CONFIG_WIFI_PASSWORD        "11112222"
#define CONFIG_WIFI_SSID            ""
#define CONFIG_WIFI_PASSWORD        ""
/* Public toilet HTTP stays disabled. SMS is enabled for the user-authorized SOS contact. */
#define CONFIG_CELLULAR_ENABLE_EXTERNAL_REQUESTS 0
#define CONFIG_CELLULAR_ENABLE_SMS               1
#define CONFIG_CELLULAR_ENABLE_LBS               1
/* GPS must remain unavailable for this long before the first 4G LBS request. */
#define CONFIG_CELLULAR_LBS_GPS_GRACE_MS         30000UL
/* Limit carrier positioning traffic and modem power use while GPS is unavailable. */
#define CONFIG_CELLULAR_LBS_RETRY_MS             60000UL
#define CONFIG_CELLULAR_LBS_VALID_MS             180000UL
#define CONFIG_GPS_FIX_VALID_MS                  10000UL
#define CONFIG_EMERGENCY_PHONE_DEFAULT           "13352934299"

/*
 * Two active-low keys.
 *
 * GPIO32 supports INPUT_PULLUP, so its button only needs to connect GPIO32 to
 * GND.  GPIO35 is reserved for a future IMU interrupt driven by the sensor.
 *
 * The ESP32 Dev Module BOOT key is GPIO0 and can use INPUT_PULLUP.  Do not hold
 * BOOT while resetting/powering the board, otherwise the ROM download mode is
 * selected.
 */
#define CONFIG_KEY_TOILET_PIN       32
#define CONFIG_KEY_TOILET_ENABLED   1
#define CONFIG_KEY_HOME_PIN         0
#define CONFIG_KEY_HOME_ENABLED     1
#define CONFIG_KEY_DEBOUNCE_MS      35
#define CONFIG_KEY_DOUBLE_MS        320
#define CONFIG_KEY_LONG_MS          1500
#define CONFIG_KEY_ARM_RELEASE_MS   800

/* IMU */
#define CONFIG_IMU_INT1_PIN         35
#define CONFIG_IMU_INT2_PIN         39

/* I2C */
#define CONFIG_MCU_SDA_PIN          21
#define CONFIG_MCU_SCL_PIN          22

/* Capacitive touch: silkscreen T_IRQ/T_SDI/T_CS/T_CLK = INT/SDA/RST/SCL */
#define CONFIG_TOUCH_INT_PIN        25
#define CONFIG_TOUCH_RST_PIN        33
#define CONFIG_TOUCH_SWAP_XY        0
#define CONFIG_TOUCH_INVERT_X       0
#define CONFIG_TOUCH_INVERT_Y       0

/* Debug USART */
#define CONFIG_DEBUG_SERIAL         Serial

/* SD CARD */
#define CONFIG_SD_CD_PIN            -1
#define CONFIG_SD_MOSI_PIN          CONFIG_SCREEN_MOSI_PIN
#define CONFIG_SD_MISO_PIN          CONFIG_SCREEN_MISO_PIN
#define CONFIG_SD_SCK_PIN           CONFIG_SCREEN_SCK_PIN
#define CONFIG_SD_CS_PIN            4
#define CONFIG_SD_DET_PIN           -1

/* Stack Info */
#define CONFIG_USE_STACK_INFO       0

#endif
