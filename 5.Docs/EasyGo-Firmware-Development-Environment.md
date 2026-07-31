# EasyGo 固件开发环境配置与使用教程

本文用于在 Windows 上从零配置 EasyGo ESP32 固件开发环境，并完成编译、烧录和串口监视。内容以仓库在 2026-07-31 的实际配置为准。

## 1. 当前工程基线

固件工程目录：

```text
E:\Projects\EasyGo\3.Firmware\PlatformIO\EasyGo-ESP32-fw
```

当前工程配置：

| 项目 | 当前值 |
| --- | --- |
| 构建系统 | PlatformIO Core 6.1.19 |
| PlatformIO 环境 | `esp32dev` |
| 平台 | Espressif 32 6.9.0 |
| 开发板 | Espressif ESP32 Dev Module（`board = esp32dev`） |
| 框架 | Arduino-ESP32 2.0.17 |
| 编译器 | Xtensa ESP32 GCC 8.4.0+2021r2-patch5 |
| C/C++ 标准 | GNU99 / GNU++11 |
| 分区表 | `min_spiffs.csv` |
| 上传速度 | 921600 baud |
| 串口监视速度 | 115200 baud |
| UI | LVGL 8.1.0-dev，RGB565，240×320 应用分辨率 |

`platformio.ini` 已锁定 `espressif32@6.9.0`，避免新电脑首次配置时解析到不同平台版本。

## 2. 安装基础软件

### 2.1 安装 Visual Studio Code

安装 VS Code 后启动一次，确认可以正常打开文件夹和终端。

### 2.2 安装 PlatformIO IDE 扩展

在 VS Code 的扩展页面搜索并安装：

```text
platformio.platformio-ide
```

仓库的 `.vscode/extensions.json` 已推荐该扩展。PlatformIO 会自动生成 IntelliSense、编译器路径和调试配置，不要手动编辑 `.vscode/c_cpp_properties.json` 或 `.vscode/launch.json`。

### 2.3 安装 USB 串口驱动

把设备通过支持数据传输的 USB 线连接到电脑，在“设备管理器 → 端口（COM 和 LPT）”中确认出现 COM 端口。

如果没有端口：

1. 更换支持数据传输的 USB 线和 USB 接口。
2. 根据板载 USB 转串口芯片安装对应驱动。
3. 重新插拔设备并记录端口号，例如 `COM5`。

不要仅凭开发板名称猜测 USB 转串口芯片，应以实际器件丝印或设备管理器硬件 ID 为准。

## 3. 正确打开工程

在 VS Code 中选择“文件 → 打开文件夹”，直接打开：

```text
E:\Projects\EasyGo\3.Firmware\PlatformIO\EasyGo-ESP32-fw
```

不要只打开单个 `.cpp` 文件。PlatformIO 必须从包含 `platformio.ini` 的目录识别项目。

首次打开时，PlatformIO 可能下载 ESP32 平台、Arduino 框架和工具链，需要联网并等待安装完成。项目的 LVGL、ArduinoJson、NimBLE、TFT_eSPI 等应用库已放在 `lib/`，不需要逐个从 Library Manager 重装。

需要把已验证的 Windows x64 平台、框架和工具链交给协作同事时，使用 `0.References/PlatformIO-ESP32-6.9.0-Windows-x64.zip`，解压后按包内 `README.md` 安装，不要复制整个个人 `.platformio` 目录。

## 4. 理解 `platformio.ini`

工程的核心配置为：

```ini
[platformio]
default_envs = esp32dev

[env:esp32dev]
platform = espressif32@6.9.0
board = esp32dev
framework = arduino
board_build.partitions = min_spiffs.csv

upload_speed = 921600
monitor_speed = 115200
```

各字段含义：

- `[platformio] default_envs = esp32dev`：不指定环境时默认构建 ESP32 Dev Module。
- `[env:esp32dev]`：定义名为 `esp32dev` 的构建环境。
- `platform = espressif32@6.9.0`：固定使用 PlatformIO Espressif 32 6.9.0。
- `board = esp32dev`：使用 Espressif ESP32 Dev Module 板级定义。
- `framework = arduino`：使用 Arduino 编程框架。
- `board_build.partitions = min_spiffs.csv`：使用平台包自带的最小 SPIFFS 分区表，本工程目录下没有同名文件是正常现象。
- `upload_speed`：烧录串口波特率。
- `monitor_speed`：串口监视器波特率，需与程序中的调试串口一致。

不要把个人电脑的 `COM` 端口写死并提交到公共配置。需要指定端口时，优先在命令中使用 `--upload-port COMx` 或通过本机 PlatformIO 设置处理。

## 5. 编译固件

### 5.1 使用 VS Code

打开左侧 PlatformIO 图标，依次选择：

```text
PROJECT TASKS → esp32dev → General → Build
```

也可以点击 VS Code 底部状态栏的勾号按钮。

### 5.2 使用 PowerShell

```powershell
cd E:\Projects\EasyGo\3.Firmware\PlatformIO\EasyGo-ESP32-fw
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev
```

如果 PlatformIO 已加入 `PATH`，可使用可移植写法：

```powershell
platformio run -e esp32dev
```

编译成功必须同时满足：

- 命令退出码为 `0`；
- 日志末尾出现 `[SUCCESS]`；
- 完成 `Linking`、容量检查和 ESP32 镜像生成。

主要产物：

```text
.pio/build/esp32dev/firmware.bin
.pio/build/esp32dev/firmware.elf
.pio/build/esp32dev/bootloader.bin
.pio/build/esp32dev/partitions.bin
```

其中 `firmware.bin` 用于常规烧录，`firmware.elf` 用于符号分析和调试。

## 6. 清理并完整重编译

只有怀疑依赖、头文件或配置缓存陈旧时才清理：

```powershell
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev --target clean
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev
```

清理会删除 `.pio/build/esp32dev/` 中可重新生成的构建产物，不会删除源码。

## 7. 烧录固件

烧录前确认目标板、USB 连接和供电均正确。指定端口的命令示例：

```powershell
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev --target upload --upload-port COM5
```

把 `COM5` 替换为设备管理器中的实际端口。

在 VS Code 中可选择：

```text
PROJECT TASKS → esp32dev → General → Upload
```

若一直停在 `Connecting...`：

1. 关闭占用该 COM 口的串口监视器和其他程序。
2. 检查 USB 线、供电和端口号。
3. 根据实体板的下载电路，在开始连接时按住 BOOT，再短按 RESET/EN；开始写入后松开 BOOT。
4. 必要时把上传速度临时降为 460800 或 115200，排除串口链路不稳定。

## 8. 打开串口监视器

```powershell
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' device monitor --port COM5 --baud 115200
```

退出串口监视器通常使用 `Ctrl+C`。烧录前先退出监视器，避免端口被占用。

VS Code 中对应任务为：

```text
PROJECT TASKS → esp32dev → General → Monitor
```

## 9. 项目依赖说明

当前主要依赖如下：

| 库 | 版本 | 来源 |
| --- | --- | --- |
| LVGL | 8.1.0-dev | `lib/lvgl/` |
| ArduinoJson | 6.18.0 | `lib/ArduinoJson/` |
| NimBLE-Arduino | 1.3.1 | `lib/NimBLE/` |
| TFT_eSPI | 2.3.58 | `lib/TFT_eSPI/` |
| MPU6050 | 0.0.1 | `lib/MPU6050/` |
| SD / SPI | 1.1 | `lib/SD/`、`lib/SPI/` |
| Wire / FS | Arduino-ESP32 内置 | PlatformIO 框架包 |

不要同时从 Library Manager 安装另一版本并强行加入工程，否则可能出现头文件命中错误版本、API 不兼容或重复符号。

查看 PlatformIO 实际解析到的工具链版本：

```powershell
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' pkg list --environment esp32dev
```

## 10. UI、显示和硬件配置位置

| 配置内容 | 文件 |
| --- | --- |
| PlatformIO 板型、框架、分区、波特率 | `platformio.ini` |
| 应用分辨率、背光、蜂鸣器、I²C、编码器、电源、SD 卡等 | `src/App/Configs/Config.h` |
| LVGL 色深、字体、组件开关 | `lib/lvgl/lv_conf.h` |
| TFT 驱动、屏幕尺寸、SPI 引脚和频率 | `lib/TFT_eSPI/User_Setups/Setup_EasyGo_ESP32_ST7789_240x320.h` |
| TFT_eSPI 当前选择的配置 | `lib/TFT_eSPI/User_Setup_Select.h` |
| 电容触摸控制器适配 | `src/HAL/HAL_Touch.cpp` |
| LVGL 显示刷新适配 | `src/Port/lv_port/lv_port_disp.cpp` |
| 程序入口 | `src/main.cpp` |

修改屏幕或板卡时要同时核对这些文件，不能只改一处。

### 当前显示与触摸基线

应用、LVGL 显示端口和 TFT_eSPI 已统一为 ST7789 240×320。当前引脚、供电注意、触摸丝印映射和真机排查步骤见 [ESP32 Dev Module + ST7789 电容触摸屏接线与点亮教程](ESP32Dev-ST7789-Wiring-and-Bringup.md)。

照片无法确认触摸 IC 的具体型号。固件会有限识别 FT6x36、CST816 和 GT911；实体板仍应通过串口地址、芯片丝印和实际触摸坐标完成验证。

## 11. UI 字体配置

UI 使用某个 Montserrat 字号前，必须在 `lib/lvgl/lv_conf.h` 中启用相应宏，例如：

```c
#define LV_FONT_MONTSERRAT_20 1
```

出现以下错误时：

```text
lv_font_montserrat_20 was not declared in this scope
```

先汇总源码实际使用的字号：

```powershell
rg -o "lv_font_montserrat_[0-9]+" src | Sort-Object -Unique
```

再启用缺失字号并重新编译。不要用相近字号替换 UI 设计来掩盖配置问题。启用更多内置字体会增加 Flash 占用，构建后必须查看容量报告。

中文 UI 使用以下自定义字形子集：

```text
font_easygo_12
font_easygo_14
font_easygo_16
font_easygo_20
font_easygo_28
```

修改中文文案后若出现方框或缺字，使用 `4.Software/UI/tools/build_lvgl_assets.py` 重新生成字形，并确认生成的 `.c` 文件保留在 `src/App/Resources/Font/`。

## 12. 常见构建问题

### `printf was not declared in this scope`

在对应 C++ 文件中包含：

```cpp
#include <cstdio>
```

### 类型不是某命名空间成员

例如：

```text
'Dummy_Info_t' is not a member of 'HAL'
```

检查声明所在头文件、命名空间、包含关系以及声明与实现的类型是否一致。不要只在报错文件中临时定义一个同名类型。

### PlatformIO 无法写入 `platforms.lock` 或 `.cache`

如果日志包含用户目录权限错误，先确认当前进程能访问：

```text
C:\Users\<用户名>\.platformio
```

在沙箱或受限执行环境中，应授予现有 PlatformIO 目录必要访问后重试。不要因为权限报错直接删除缓存、升级 PlatformIO 或修改源码。

### 找不到库或头文件

1. 确认打开的是包含 `platformio.ini` 的固件目录。
2. 确认仓库的 `lib/` 内容完整。
3. 查看依赖扫描日志实际命中了哪个库。
4. 必要时执行 clean 后完整重编译。

### 只有 warning，没有 error

以退出码和 `[SUCCESS]` 为准。警告需要记录和后续处理，但警告本身不等于构建失败。

## 13. 新电脑配置完成检查表

- [ ] VS Code 已安装。
- [ ] PlatformIO IDE 扩展已安装并完成初始化。
- [ ] 直接打开了 `EasyGo-ESP32-fw` 文件夹。
- [ ] `platformio.ini` 中能看到 `esp32dev` 环境。
- [ ] `lib/` 下的本地依赖完整。
- [ ] `platformio run -e esp32dev` 最终显示 `[SUCCESS]`。
- [ ] `.pio/build/esp32dev/firmware.bin` 已生成。
- [ ] 设备管理器能识别目标板 COM 端口。
- [ ] 烧录前已核对实体屏幕分辨率、TFT_eSPI 配置和硬件引脚。
- [ ] 串口监视器使用 115200 baud。
- [ ] 构建日志中的 RAM、Flash 占用和警告已检查。

完成以上检查后，固件开发环境即可用于日常编译。烧录、硬件上电和真实外设测试仍应按目标板接线及供电条件单独确认。
