# ESP32 Dev Module + ST7789 电容触摸屏接线与点亮教程

本文对应 EasyGo 当前固件配置：ESP32 Dev Module、ST7789 240×320 SPI 屏，以及屏幕转接板丝印所示的 I²C 电容触摸接口。

> 当前结论分为两部分：固件已在 `esp32dev` 环境完整编译通过；实体接线、烧录、屏幕方向和触摸坐标仍需在你的开发板上验证。

## 1. 接线前注意

- 断电接线，确认无短路后再插 USB。
- ESP32 GPIO 是 3.3V 逻辑，不能接入 5V 信号。
- 屏幕 `VCC` 首次使用接 ESP32 `3V3`，不要在没有模块规格书的情况下改接 `VIN/5V`。
- `LED` 是背光脚。首次点亮建议通过 100–220Ω 串联电阻接 `3V3`；确认模块自带限流或背光驱动后，可按模块资料简化。
- 图片中的电容触摸定义以右栏为准：`T_IRQ/T_SDO/T_SDI/T_CS/T_CLK` 分别作为 `INT/NC/SDA/RST/SCL`，不是电阻触摸 SPI 接口。
- 不使用 GPIO0、GPIO2、GPIO12、GPIO15，避免外设电平干扰 ESP32 启动绑带。

## 2. 完整接线表

### 2.1 ST7789 显示

| 屏幕转接板 | ESP32 Dev Module | 作用 |
| --- | --- | --- |
| `VCC` | `3V3` | 屏幕与触摸供电 |
| `GND` | `GND` | 公共地 |
| `CS` | `GPIO5` | TFT 片选 |
| `RESET` | `GPIO16` | TFT 复位 |
| `DC` | `GPIO27` | 数据/命令选择 |
| `SDI(MOSI)` | `GPIO23` | VSPI MOSI |
| `SCK` | `GPIO18` | VSPI 时钟 |
| `SDO(MISO)` | `GPIO19` | VSPI MISO；显示写入时通常不使用，但建议连接 |
| `LED` | `3V3`，建议串 100–220Ω | 背光常亮；当前固件不对该脚输出 PWM |

### 2.2 电容触摸

| 屏幕转接板丝印 | 电容触摸含义 | ESP32 Dev Module |
| --- | --- | --- |
| `T_IRQ` | `INT` | `GPIO25` |
| `T_SDO` | `NC` | 不连接 |
| `T_SDI` | `SDA` | `GPIO21` |
| `T_CS` | `RST` | `GPIO33` |
| `T_CLK` | `SCL` | `GPIO22` |

所有地线必须共地。杜邦线尽量短，SPI 线过长会导致白屏、花屏或随机闪烁。

## 3. 固件中已完成的适配

| 项目 | 当前配置 |
| --- | --- |
| PlatformIO 环境 | `esp32dev` |
| PlatformIO 平台 | `espressif32@6.9.0` |
| 开发板 | `board = esp32dev` |
| 显示驱动 | ST7789，240×320，VSPI 40MHz |
| LVGL 分辨率 | 240×320，RGB565 |
| 屏幕方向 | `CONFIG_SCREEN_ROTATION = 0` |
| 触摸总线 | I²C，SDA 21 / SCL 22 |
| 触摸兼容 | FT6x36、CST816、GT911 有限自动识别 |
| 首次显示诊断 | 红、绿、蓝各显示约 120ms，然后启动 LVGL |

主要配置文件：

- `3.Firmware/PlatformIO/EasyGo-ESP32-fw/platformio.ini`
- `3.Firmware/PlatformIO/EasyGo-ESP32-fw/src/App/Configs/Config.h`
- `3.Firmware/PlatformIO/EasyGo-ESP32-fw/lib/TFT_eSPI/User_Setups/Setup_EasyGo_ESP32_ST7789_240x320.h`
- `3.Firmware/PlatformIO/EasyGo-ESP32-fw/src/HAL/HAL_Touch.cpp`
- `3.Firmware/PlatformIO/EasyGo-ESP32-fw/src/Port/lv_port/lv_port_indev.cpp`

为避免引脚冲突，首次点屏配置暂时关闭了旧板卡的编码器、电源检测、蜂鸣器、音频、SD、IMU 和蓝牙初始化。待这些外设确定新接线后，再逐项恢复。

## 4. 编译

```powershell
cd E:\Projects\EasyGo\3.Firmware\PlatformIO\EasyGo-ESP32-fw
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev
```

2026-07-31 的实际验证结果：

```text
RAM:   31484 / 327680 bytes (9.6%)
Flash: 1221313 / 1966080 bytes (62.1%)
[SUCCESS]
```

固件输出：

```text
.pio/build/esp32dev/firmware.bin
.pio/build/esp32dev/firmware.elf
```

## 5. 烧录与串口

先在设备管理器确认实际 COM 口，再执行：

```powershell
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev --target upload --upload-port COM5
```

将 `COM5` 替换为真实端口。若停在 `Connecting...`，按住 `BOOT`，短按一次 `EN/RESET`，开始写入后松开 `BOOT`。

烧录完成后打开串口：

```powershell
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' device monitor --port COM5 --baud 115200
```

## 6. 正常现象

1. 背光点亮。
2. 屏幕依次出现红、绿、蓝。
3. 显示 EasyGo Startup 页面。
4. 约 2.2 秒后自动进入 Home 页面。
5. 串口出现类似输出：

```text
Display: ST7789 240 x 320, rotation=0
Touch: FT6x36-compatible controller at 0x38
```

触摸控制器也可能显示 `CST816 ... 0x15` 或 `GT911 ... 0x5D/0x14`。

## 7. 屏幕或触摸方向修正

屏幕上下颠倒时，在 `Config.h` 中把：

```cpp
#define CONFIG_SCREEN_ROTATION 0
```

改为 `2`。如果需要横屏，使用 `1` 或 `3`，并同步调整 LVGL 分辨率；当前 EasyGo UI 是竖屏 240×320，不建议只旋转 TFT 而不改 UI。

触摸位置与画面不一致时，逐项修改：

```cpp
#define CONFIG_TOUCH_SWAP_XY  0
#define CONFIG_TOUCH_INVERT_X 0
#define CONFIG_TOUCH_INVERT_Y 0
```

每个值只能是 `0` 或 `1`。先测试四角，再确定是否交换 X/Y 或反向某一轴。

## 8. 故障排查

### 背光不亮

- 先断电检查 `LED`、`VCC`、`GND`。
- 确认 `LED` 的模块定义和限流方式。
- 用万用表确认屏幕端确实得到约 3.3V。

### 背光亮但全黑

- 检查 `CS=5`、`RESET=16`、`DC=27`、`MOSI=23`、`SCK=18`。
- 确认屏幕控制器确实是 ST7789，分辨率确实是 240×320。
- 降低 `SPI_FREQUENCY` 到 `27000000` 排除线长和信号质量问题。

### 有图像但颜色异常

本项目当前这块 ST7789 实屏使用：

```cpp
#define TFT_RGB_ORDER TFT_BGR
#define TFT_INVERSION_OFF
```

判断方法：

- 米白背景变为近黑、蓝色变为黄橙色，说明整屏颜色被取反，应使用 `TFT_INVERSION_OFF`。
- 背景明暗正确、但纯红显示为蓝色且纯蓝显示为红色，才把 `TFT_BGR` 改为 `TFT_RGB`。
- 一次只改一项。当前实拍符合第一种情况，因此仅关闭反相，保留 `TFT_BGR`。

例如，启动页的设计背景 `#FFF9EF` 取反后约为 `#000610`，设计蓝色 `#2F80ED` 取反后约为 `#D07F12`，与实拍的黑色背景和橙黄色卡片一致。

### UI 正常但不能触摸

- 核对图片右栏映射，尤其 `T_CS` 在电容模式下是 `RST`，不是 SPI 片选。
- 查看串口是否检测到 `0x38`、`0x15`、`0x5D` 或 `0x14`。
- 如果串口只发现其他地址，记录地址并拍清楚触摸芯片丝印；不要继续猜寄存器协议。
- 如果控制器识别成功但点位错误，调整三个触摸方向宏。

### 随机复位或无法下载

- 确认没有接 GPIO0、GPIO2、GPIO12、GPIO15。
- 检查供电能力、杜邦线短路和 USB 线。
- 烧录时断开非必要外设，先只保留 USB；成功后再接屏幕。

## 9. 当前尚未验证

- 未在本次代码任务中实际烧录你的实体开发板。
- 照片无法辨认触摸 IC 的具体型号；自动识别覆盖三个常见系列，但仍需以串口探测和芯片丝印为准。
- `LED` 引脚的板载限流/驱动电路未从照片确认，首次上电应使用串联电阻并观察电流和温升。
- 旧板卡的 SD、IMU、编码器、电源、蜂鸣器和蓝牙尚未为这套新接线重新分配并做硬件验证。
