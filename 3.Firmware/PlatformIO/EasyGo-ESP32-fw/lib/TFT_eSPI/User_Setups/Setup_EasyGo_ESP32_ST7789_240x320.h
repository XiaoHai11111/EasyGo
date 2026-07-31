#ifndef USER_SETUP_LOADED
#define USER_SETUP_LOADED

#define USER_SETUP_ID 900

#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

/* Verified on the EasyGo ST7789 module: BGR order, normal (non-inverted) colors. */
#define TFT_RGB_ORDER TFT_BGR
#define TFT_INVERSION_OFF

/* ESP32 Dev Module VSPI wiring. */
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC    27
#define TFT_RST   16

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000

#endif
