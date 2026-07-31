# DFPlayer Mini + microSD 音频接线与验证

本文用于把 DFPlayer Mini 接入当前 EasyGo ESP32 Dev Module 固件，通过 UART2 播放 microSD 卡中的提示音。

> DFPlayer 插槽中的 microSD 卡只供 DFPlayer 解码音频。ESP32 不能把它挂载成 EasyGo 的 `S:/` 文件系统，因此地图、联系人等数据仍需要独立的 SPI microSD 模块。

## 1. 接线

断电接线：

| DFPlayer Mini | ESP32 Dev Module | 说明 |
| --- | --- | --- |
| `VCC` | `5V/VIN` | 使用 USB 供电时取开发板 5V；DFPlayer 支持 3.2–5V，带喇叭时建议稳定 5V |
| `GND` | `GND` | 必须与 ESP32 共地 |
| `RX` | `GPIO17`，中间串联 1kΩ | ESP32 UART2 TX → DFPlayer RX |
| `TX` | `GPIO26` | DFPlayer TX → ESP32 UART2 RX |
| `BUSY` | `GPIO34` | 可选；低电平表示正在播放 |
| `SPK_1` | 喇叭一端 | 直推单声道喇叭 |
| `SPK_2` | 喇叭另一端 | 不要接 GND |

其余 `USB+/-`、`IO_1/2`、`ADKEY_1/2` 暂不连接。

### 音频输出二选一

- 小于等于 3W 的无源喇叭：接 `SPK_1` 与 `SPK_2`。
- 外置功放或耳机输入：使用 `DAC_L`、`DAC_R` 和 `GND`。

不要把 `SPK_1/SPK_2` 接到功放的线路输入，也不要把其中一根喇叭线接地。

如果使用独立 5V 电源给 DFPlayer 供电，电源负极必须与 ESP32 GND 相连。首次测试把音量保持在当前默认值 20/30，确认无过热、异常电流和严重噪声后再调整。

## 2. microSD 卡准备

1. 使用不超过 32GB 的 microSD/TF 卡。
2. 格式化为 FAT16 或 FAT32。
3. 在卡根目录创建小写文件夹：

   ```text
   /mp3
   ```

4. 文件名使用四位数字：

   ```text
   /mp3/0001.mp3
   /mp3/0002.mp3
   ...
   /mp3/0011.mp3
   ```

5. 断电插卡，再给 DFPlayer 上电；不建议热插拔。

当前 EasyGo 名称与文件编号：

| 文件 | 固件名称 | 用途 |
| --- | --- | --- |
| `0001.mp3` | `Startup` | 开机提示 |
| `0002.mp3` | `Shutdown` | 关机提示 |
| `0003.mp3` | `Error` | 错误提示 |
| `0004.mp3` | `Connect` | 连接成功 |
| `0005.mp3` | `Disconnect` | 连接断开 |
| `0006.mp3` | `UnstableConnect` | 连接不稳定 |
| `0007.mp3` | `BattChargeStart` | 开始充电 |
| `0008.mp3` | `BattChargeEnd` | 停止充电 |
| `0009.mp3` | `DeviceInsert` | 设备插入 |
| `0010.mp3` | `DevicePullout` | 设备拔出 |
| `0011.mp3` | `NoOperationWarning` | 长时间无操作 |

首次测试只准备 `0001.mp3` 即可。

## 3. 固件配置

配置位于：

```text
3.Firmware/PlatformIO/EasyGo-ESP32-fw/src/App/Configs/Config.h
```

当前值：

```cpp
#define CONFIG_ENABLE_AUDIO         1
#define CONFIG_AUDIO_USE_DFPLAYER   1
#define CONFIG_DFPLAYER_RX_PIN      26
#define CONFIG_DFPLAYER_TX_PIN      17
#define CONFIG_DFPLAYER_BUSY_PIN    34
#define CONFIG_DFPLAYER_VOLUME      20
```

驱动位于：

```text
3.Firmware/PlatformIO/EasyGo-ESP32-fw/src/HAL/HAL_Audio.cpp
```

驱动直接实现 DFPlayer UART 帧协议，不需要安装额外 Arduino 库。初始化是非阻塞的，不会在 DFPlayer 启动期间暂停 LVGL 页面。

## 4. 编译与烧录

```powershell
cd E:\Projects\EasyGo\3.Firmware\PlatformIO\EasyGo-ESP32-fw
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev
& 'C:\Users\24302\.platformio\penv\Scripts\platformio.exe' run -e esp32dev --target upload --upload-port COM5
```

把 `COM5` 替换成实际端口。

## 5. 正常串口输出

```text
DFPlayer: waiting for module and SD card...
DFPlayer: initialization commands sent, volume=20
DFPlayer: play /mp3/0001.mp3
```

模块有反馈时还可能看到：

```text
DFPlayer: SD card inserted
DFPlayer: storage online, mask=0x0002
DFPlayer: track 1 finished
```

## 6. 故障排查

### `no UART response`

检查顺序：

1. 卡是否已插入且为 FAT16/FAT32。
2. `ESP32 GPIO17 → 1kΩ → DFPlayer RX`。
3. `DFPlayer TX → ESP32 GPIO26`。
4. RX/TX 必须交叉连接，不能 RX 对 RX。
5. VCC 是否为稳定的约 5V，GND 是否共地。
6. 文件是否位于 `/mp3/0001.mp3`。

### 有反馈但没有声音

- 检查喇叭是否接在 `SPK_1` 和 `SPK_2` 之间。
- 检查文件能否在电脑正常播放。
- 尝试音量 15–25，不要一开始设为 30。
- 使用 DAC 输出时必须连接功放或有源音箱。

### 播放时重启、爆音或底噪

- 使用短线并让电源线和喇叭线远离 SPI、I²C 和 UART。
- 确认 ESP32 TX 到 DFPlayer RX 之间有 1kΩ 串联电阻。
- 检查 5V 电源在播放峰值时是否明显下降。
- 必要时使用独立稳定 5V 供电，并保持共地。

## 7. 已验证与待验证

- 已在 `esp32dev` 环境完成固件编译。
- 已避免与 ST7789、触摸 I²C 和调试串口的引脚冲突。
- 尚未在用户实体 DFPlayer、microSD 和喇叭上完成播放验证。
- 不同兼容版 DFPlayer 的上电时间和反馈行为可能存在差异，应以串口输出和实际播放为准。
