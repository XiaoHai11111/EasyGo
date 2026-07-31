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
#define CONFIG_ENABLE_POWER         0
#define CONFIG_ENABLE_ENCODER       0
#define CONFIG_ENABLE_BUZZER        0
#define CONFIG_ENABLE_AUDIO         1
#define CONFIG_ENABLE_SD_CARD       0
#define CONFIG_ENABLE_IMU           0
#define CONFIG_ENABLE_TOUCH         1

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

/* Battery */
#define CONFIG_BAT_DET_PIN          37
#define CONFIG_BAT_CHG_DET_PIN      38

/* Buzzer */
#define CONFIG_BUZZ_PIN             25
#define CONFIG_BUZZ_CHANNEL         2
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

/* IMU */
#define CONFIG_IMU_INT1_PIN         32
#define CONFIG_IMU_INT2_PIN         33

/* I2C */
#define CONFIG_MCU_SDA_PIN          21
#define CONFIG_MCU_SCL_PIN          22

/* Capacitive touch: silkscreen T_IRQ/T_SDI/T_CS/T_CLK = INT/SDA/RST/SCL */
#define CONFIG_TOUCH_INT_PIN        25
#define CONFIG_TOUCH_RST_PIN        33
#define CONFIG_TOUCH_SWAP_XY        0
#define CONFIG_TOUCH_INVERT_X       0
#define CONFIG_TOUCH_INVERT_Y       0

/* Encoder */
#define CONFIG_ENCODER_B_PIN        34
#define CONFIG_ENCODER_A_PIN        35
#define CONFIG_ENCODER_PUSH_PIN     27

/* Power */
#define CONFIG_POWER_EN_PIN         21

/* Debug USART */
#define CONFIG_DEBUG_SERIAL         Serial

/* SD CARD */
#define CONFIG_SD_SPI               SPI_SD
#define CONFIG_SD_CD_PIN            -1
#define CONFIG_SD_MOSI_PIN          PB15
#define CONFIG_SD_MISO_PIN          PB14
#define CONFIG_SD_SCK_PIN           PB13
#define CONFIG_SD_CS_PIN            15
#define CONFIG_SD_DET_PIN           22

/* Stack Info */
#define CONFIG_USE_STACK_INFO       0

#endif
