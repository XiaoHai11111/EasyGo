# EasyGo

EasyGo 是一个面向老年人出行辅助场景的软硬件项目，当前重点验证便携式设备上的适老交互、附近厕所展示与简单导航、亲情联系人和紧急求助入口。

项目目前处于 **240×320 LVGL UI 与 Windows PC 仿真联调阶段**。仓库中的地图、路线、距离、联系人和求助行为主要用于界面与数据链路演示，不代表生产环境已经接入真实定位、导航、拨号或告警服务。

## 当前进展

- [x] ESP32 Dev Module PlatformIO 固件和 `esp32dev` 构建链路
- [x] ST7789 240×320 显示、颜色与 FT6x36 电容触摸真机联调
- [x] DFPlayer Mini UART2 音频和 microSD 提示音映射（已编译，待实体模块验证）
- [x] 240×320 启动页、首页、导航、亲情联系人和设置页面
- [x] 固件页面源码与 Windows LVGL 模拟器共用
- [x] 中文字体子集及资源生成脚本
- [x] 模拟 SD 卡联系人数据加载
- [x] 深圳宝安区离线地图、七级缩放和厕所 POI 演示数据
- [x] Windows x64 PlatformIO 协作依赖包
- [ ] 真实厕所 POI 与道路路线服务
- [ ] GNSS、蜂窝、Wi-Fi 或手机辅助定位方案
- [ ] 电子围栏完整判定与告警闭环
- [ ] 真实拨号、消息和位置共享
- [ ] 最终硬件、结构、电源、天线和量产方案

## 目录结构

```text
EasyGo/
├─ 0.References/   器件资料、地图来源和开发环境离线包
├─ 1.Model/        产品结构、外壳与装配模型
├─ 2.Hardware/     原理图、PCB、BOM 与硬件测试资料
├─ 3.Firmware/     ESP32 PlatformIO 固件工程
├─ 4.Software/     Windows LVGL 模拟器、模拟 SD 卡和 UI 工具
├─ 5.Docs/         开发教程、项目状态、AI 技能和专项说明
└─ README.md       项目入口文档
```

主要工程入口：

- 固件：`3.Firmware/PlatformIO/EasyGo-ESP32-fw/`
- Windows 模拟器：`4.Software/Simulator/LVGL.Simulator.sln`
- 模拟 SD 卡：`4.Software/Simulator/SD/`
- UI 资源工具：`4.Software/UI/tools/build_lvgl_assets.py`

## 开发环境版本

下表记录的是已在本项目完成构建验证的版本，不应把“可以安装更新版”理解为“更新版已经兼容”。

| 组件 | 已验证版本或配置 |
| --- | --- |
| 操作系统 | Windows x64 |
| Visual Studio Code | 安装 PlatformIO IDE 扩展 |
| PlatformIO IDE 扩展 | `platformio.platformio-ide` |
| PlatformIO Core | 6.1.19 |
| PlatformIO 环境 | `esp32dev` |
| PlatformIO 平台 | Espressif 32 6.9.0 |
| 开发板定义 | Espressif ESP32 Dev Module（`board = esp32dev`） |
| Arduino-ESP32 | 2.0.17 / `framework-arduinoespressif32` 3.20017.0 |
| Xtensa 工具链 | 8.4.0+2021r2-patch5 |
| esptool.py | 4.5.1 / PlatformIO 包 1.40501.0 |
| C / C++ 标准 | GNU99 / GNU++11 |
| 分区表 | `min_spiffs.csv` |
| 固件串口监视 | 115200 baud |
| 固件上传速度 | 921600 baud |
| Windows 模拟器 | Visual Studio 2019，`Debug|x64` |
| LVGL | 8.1.0-dev |
| 屏幕应用分辨率 | 240×320，RGB565 |

主要本地库：

| 库 | 版本 |
| --- | --- |
| ArduinoJson | 6.18.0 |
| NimBLE-Arduino | 1.3.1 |
| TFT_eSPI | 2.3.58 |
| MPU6050 | 0.0.1 |
| SD / SPI | 1.1 |

这些库已随固件工程保存在 `lib/`，不要再从 Library Manager 混装另一版本。

`platformio.ini` 已锁定 `espressif32@6.9.0`。TFT_eSPI 使用项目专用的 ST7789 240×320 配置；ESP32 Dev Module 的接线、烧录和真机排查见 [ESP32 Dev Module + ST7789 接线与点亮教程](5.Docs/ESP32Dev-ST7789-Wiring-and-Bringup.md)。

## 快速开始

### 编译 ESP32 固件

```powershell
cd .\3.Firmware\PlatformIO\EasyGo-ESP32-fw
platformio run --environment esp32dev
```

本机未把 PlatformIO 加入 `PATH` 时，可使用：

```powershell
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run --environment esp32dev
```

成功构建应出现 `[SUCCESS]`，主要产物为：

```text
3.Firmware/PlatformIO/EasyGo-ESP32-fw/.pio/build/esp32dev/firmware.bin
3.Firmware/PlatformIO/EasyGo-ESP32-fw/.pio/build/esp32dev/firmware.elf
```

完整安装、清理、烧录和串口监视步骤见 [EasyGo 固件开发环境配置与使用教程](5.Docs/EasyGo-Firmware-Development-Environment.md)。

### 运行 Windows LVGL 模拟器

1. 使用 Visual Studio 2019 打开 `4.Software/Simulator/LVGL.Simulator.sln`。
2. 选择 `Debug|x64`。
3. 将 `LVGL.Simulator` 设为启动项目并构建运行。
4. 验证启动页、页面跳转、中文显示、地图缩放/拖动和联系人列表。

输出程序位于：

```text
4.Software/Simulator/Output/Debug/x64/LVGL.Simulator.exe
```

模拟器直接引用固件 `src/App/` 下的页面与资源。修改页面时不要复制第二套代码；新增或删除源文件后，应同步更新 `.vcxproj` 和 `.vcxproj.filters`。

## 协作环境包

Windows x64 同事可使用：

[PlatformIO ESP32 6.9.0 Windows x64 依赖包](0.References/PlatformIO-ESP32-6.9.0-Windows-x64.zip)

压缩包约 349 MB，包含：

- Espressif 32 Platform 6.9.0
- Arduino-ESP32 2.0.17
- Xtensa ESP32 工具链
- esptoolpy、SCons 和文件系统工具
- 离线安装说明与 SHA-256

该压缩包通过 Git LFS 管理，不包含 PlatformIO Core、VS Code 或 USB 串口驱动。同事应先安装 PlatformIO Core 6.1.19，再按压缩包内 `README.md` 安装依赖。

## 软件结构

固件 UI 保持 `Page / View / Model` 分层：

- `Page`：生命周期、事件绑定和页面跳转
- `View`：LVGL 控件创建、样式和显示更新
- `Model`：文件读取、业务数据和平台差异
- `AppFactory`：页面类型创建
- `PageManager`：页面注册、路由、缓存和动画
- `ResourcePool`：字体与图片资源注册
- `HAL`：屏幕、按键、IMU、电源、SD、蓝牙等硬件接口

当前页面链路：

```text
Startup
   └─ Home
      ├─ Navigation
      ├─ Family
      └─ Setting
```

## AI 协作提示词

仓库提供了 `develop-easygo` 项目技能，用于约束 AI 读取事实来源、保护现有改动、执行验证并维护项目状态。支持技能的 Codex 环境应优先使用 `$develop-easygo`；其他 AI 工具应先读取相同文件再开始修改。

### 通用任务提示词

```text
请使用 $develop-easygo 处理 EasyGo 适老导航硬件项目任务，并先读取：
E:\Projects\EasyGo\5.Docs\develop-easygo\SKILL.md
E:\Projects\EasyGo\5.Docs\develop-easygo\references\project-state.md

本次目标：[需要分析、设计、修改或验证的内容]
涉及范围：[产品/结构/硬件/固件/软件/定位/POI/导航/围栏/告警/隐私/测试]
已知条件：[硬件版本、固件版本、目标地区、网络、定位方式和数据源；未知项写“未知”]
预期结果：[可验证的现象或指标]
允许的外部动作：[无/编译/运行模拟器/烧录/联网/发送测试告警/上传测试位置]
版本操作：[不涉及/仅生成提交信息/提交/提交并推送]

请先检查仓库现状和未提交改动，保留无关修改；区分已确认事实、合理推断、候选方案和待验证事项。
修改后执行与风险相称的构建或测试，并报告实际命令、结果、改动文件、未验证内容和已知风险。
只有我明确授权后，才能烧录硬件、连接真实蜂窝网络、发送真实告警、上传真实位置、提交或推送 Git。
完成后检查项目状态文档和技能是否需要同步更新。
```

### 固件编译报错提示词

```text
请读取附件中的完整 PlatformIO 编译日志，定位第一个真正导致失败的错误，并结合当前仓库源码修复。

固件目录：
E:\Projects\EasyGo\3.Firmware\PlatformIO\EasyGo-ESP32-fw

要求：
1. 不修改无关代码，不覆盖现有未提交改动。
2. 不用替换相近字体、屏蔽源文件或删除功能来掩盖错误。
3. 修复后执行：
   C:\Users\24302\.platformio\penv\Scripts\platformio.exe run --environment esp32dev
4. 只有退出码为 0、出现 [SUCCESS]、完成链接和镜像生成，才算验证通过。
5. 报告根因、修改文件、RAM/Flash 占用和仍存在的 warning。
6. 不执行烧录。
```

### LVGL 页面开发提示词

```text
请基于当前 EasyGo 的 240×320 适老 UI，完成：[页面或交互目标]。

开始前读取：
- 5.Docs/develop-easygo/SKILL.md
- 5.Docs/develop-easygo/references/lvgl-simulator.md
- 5.Docs/develop-easygo/references/project-state.md

要求：
1. 页面源码只放在固件 src/App/ 中，Windows 模拟器直接引用，不复制第二套。
2. 保持 Page / View / Model / ResourcePool 架构。
3. 新增或删除源文件与资源时，同步更新 AppFactory、页面注册、ResourcePool、vcxproj 和 vcxproj.filters。
4. 中文文案必须检查 font_easygo 字形覆盖；地图和联系人使用模拟或脱敏数据。
5. 同时验证 Windows 模拟器 Debug|x64 和 ESP32 `esp32dev` 构建。
6. 分别报告编译、启动、视觉和交互验证结果，不用“进程存活”代替界面验收。
```

### 生成 Git 提交信息提示词

```text
请根据当前 Git diff 生成 EasyGo 项目的提交信息，但不要执行 git add、commit 或 push。

要求：
1. 只描述 diff 中实际存在的修改，不复用其他项目内容。
2. 第一行使用 Conventional Commits：
   <type>(<scope>): <中文摘要>
3. 正文按以下结构输出：
   [背景]
   [改动]
   [影响评估]
   [验证]
   [未验证/风险]
4. 明确区分已执行验证和建议验证。
5. 如果变更包含大文件、离线包、地图数据或生成资源，单独说明体积、来源和是否建议使用 Git LFS。
```

## 项目文档

| 文档 | 内容 |
| --- | --- |
| [固件开发环境教程](5.Docs/EasyGo-Firmware-Development-Environment.md) | VS Code、PlatformIO、依赖版本、编译、烧录、串口和故障排查 |
| [ESP32 Dev Module + ST7789 点亮教程](5.Docs/ESP32Dev-ST7789-Wiring-and-Bringup.md) | 显示与电容触摸接线、固件配置、烧录、方向修正和真机排查 |
| [DFPlayer Mini + microSD 音频教程](5.Docs/DFPlayer-Mini-SD-Audio-Bringup.md) | UART 接线、音频文件编号、供电、喇叭连接和串口排查 |
| [EasyGo 项目技能](5.Docs/develop-easygo/SKILL.md) | AI 工作流程、项目约束、安全边界和稳定开发方法 |
| [当前项目状态](5.Docs/develop-easygo/references/project-state.md) | 已确认事实、阶段、事实边界、待确认项和近期产出 |
| [LVGL 模拟器说明](5.Docs/develop-easygo/references/lvgl-simulator.md) | 页面共用、项目文件同步、模拟 SD、构建与实际验证记录 |
| [模拟器基础 README](4.Software/Simulator/README.md) | 上游 Visual Studio LVGL 模拟器的基础使用说明 |
| [离线地图格式](4.Software/Simulator/SD/MAP/MAP_FORMAT.md) | RGB565 地图、层级索引、坐标边界和厕所 POI 文件格式 |
| [地图数据来源](4.Software/Simulator/SD/MAP/ATTRIBUTION.md) | OSM 数据来源、许可与归属说明 |
| [联系人样例](4.Software/Simulator/SD/CONTACTS/contacts.csv) | 脱敏联系人 CSV 格式与模拟数据 |
| [PlatformIO 协作依赖包](0.References/PlatformIO-ESP32-6.9.0-Windows-x64.zip) | Windows x64 已验证平台、框架和工具链 |

## 数据与安全边界

- 地图和厕所数据当前仅用于离线 UI/算法原型，不承诺实时开放状态、无障碍设施或路线安全性。
- 联系人、位置和轨迹开发数据必须使用合成或脱敏内容。
- 未经明确授权，不触发真实拨号、短信、推送、蜂窝连接或位置上传。
- 不把实验室单次成功描述为老人真实场景下的可靠性结论。
- A7670 系列资料目前只是候选参考，不代表具体通信模块已经定型。

## 已知问题

- ESP32 Dev Module + ST7789 已完成真机点亮、颜色修正和页面点击验证；串口确认触摸控制器地址为 `0x38`，与 FT6x36 兼容，但全屏坐标边界和长期稳定性仍需继续测试。
- 设置页曾因 LVGL 8.1 日志回调签名不匹配而触发 `LoadProhibited`；代码已修复并重新编译，仍需烧录最新固件完成回归验证。
- DFPlayer Mini 当前没有 UART 响应，microSD 文件、RX/TX 交叉、串联电阻和模块供电仍需在实体模块上排查。
- 当前导航页面仍以演示数据为主，生产 POI、道路路线和定位链路尚未接入。
- 电子围栏的精度、迟滞、连续判定、接收人、告警渠道和失败恢复策略尚未确定。
- 根项目尚未声明统一许可证；引用第三方库、字体和 OSM 数据时必须分别遵守其许可证和归属要求。

## 协作约定

1. 修改前检查 `git status`，保留他人的未提交改动。
2. 页面和资源以固件 `src/App/` 为唯一来源。
3. 修改 UI 后同时检查模拟器工程项和 ESP32 构建。
4. 只把实际执行过的测试标记为通过。
5. 大型生成文件和离线依赖包提交前先确认仓库策略，必要时使用 Git LFS 或外部制品存储。
6. 只有得到明确授权后才执行 Git 提交、推送、固件烧录或真实外部通信。
