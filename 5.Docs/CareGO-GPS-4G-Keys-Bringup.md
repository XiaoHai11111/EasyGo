# CareGO GPS、4G、双按键与离线厕所功能说明

## 1. 当前实现边界

本版本已经把 GPS、网络、双按键和 CareGO 业务状态接入项目的发布订阅框架，并由固件与 Windows LVGL 仿真器共用页面代码。SD、IMU 和地磁计的接口位置已经保留，但用户尚未连接这些硬件，因此默认不初始化。

真实公网厕所查询和短信发送默认关闭：

```cpp
#define CONFIG_CELLULAR_ENABLE_EXTERNAL_REQUESTS 0
#define CONFIG_CELLULAR_ENABLE_SMS               0
```

修改这两个开关前，先使用测试 SIM、测试号码和测试服务器完成小范围验收。不要把私人号码、家庭位置或真实轨迹提交到 Git。

## 2. 接线

所有模块必须共地。ESP32 GPIO 只能承受 3.3V 逻辑电平；如果 4G/GPS 模块 UART 为更高电平，需要加电平转换。

| 外设端 | ESP32 Dev Module | 说明 |
| --- | --- | --- |
| GPS TX | GPIO36 | GPS 只发数据给 ESP32；默认 9600 baud |
| 4G TX | GPIO14 / UART1 RX | 交叉连接 |
| 4G RX | GPIO13 / UART1 TX | 交叉连接 |
| DFPlayer TX | GPIO26 / UART2 RX | 交叉连接 |
| DFPlayer RX | GPIO17 / UART2 TX | 建议串联 1kΩ |
| DFPlayer BUSY | GPIO34 | 可选，低电平表示播放中 |
| 寻厕/语音键 | GPIO32 到 GND | 固件启用 `INPUT_PULLUP`，无需外接电阻 |
| 回家/SOS 键 | 板载 BOOT（GPIO0） | 固件启用 `INPUT_PULLUP`；上电或复位时不要按住 |

原 GPIO35 按键方案因 GPIO34–39 没有内部上拉，在没有外接电阻时会悬空并产生随机点击，已改用 GPIO32。GPIO35 留给后续由传感器主动驱动的 IMU 中断输出。

4G 模块发射瞬间电流通常明显高于 ESP32，按模块资料使用独立稳定电源，并与 ESP32 共地；不要默认从开发板 3.3V 脚直接供电。

真机联调建议使用满足各模块输入规格的外置开关电源，并在 DFPlayer/4G 模块附近布置足够的去耦与储能电容。USB 可只承担串口通信；不要把两个不同 5V 电源直接并联到同一 5V 母线。

## 3. 按键交互

| 操作 | 动作事件 | 当前行为 |
| --- | --- | --- |
| 寻厕键单击 | `INPUT_TOILET_CLICK` | 查找最近厕所并进入导航页 |
| 寻厕键双击 | `INPUT_HELP_AUDIO_DOUBLE_CLICK` | 播放普通话求助语音 |
| 寻厕键长按 | `INPUT_TOILET_LONG_PRESS` | 保留，不触发外部动作 |
| 回家键单击 | `INPUT_HOME_CLICK` | 导航到设置中保存的家庭位置 |
| 回家键双击 | `INPUT_SOS_DOUBLE_CLICK` | 请求发送 SOS 短信并进入亲情守护页 |
| 回家键长按 | `INPUT_HOME_LONG_PRESS` | 保留，不触发外部动作 |

按键扫描只发布 `Input` 消息。`CareGo` 账户处理业务状态，`AppInputRouter` 只在 LVGL 定时器中处理页面跳转，硬件回调不会直接修改 LVGL 对象。

## 4. 数据链路

```text
GPS NMEA ──> Location ─┐
双按键 ───> Input ────┼──> CareGo ──> Navigation / Family / Setting / DeviceStatus
离线 POI/4G ─> Network ┘       ├──> MusicPlayer ──> DFPlayer
                               └──> Network ──> SMS / 厕所查询
```

GPS 驱动校验 NMEA 校验和并解析 GGA/RMC。厕所查询首先尝试 `/MAP/shenzhen_toilets.bin`，逐条计算 WGS84 Haversine 直线距离并选择最近记录；文件不可用时才尝试热点或 4G。当前离线索引是宝安区 OSM 快照，不表示厕所实时开放或数据完整。

## 5. 热点与 4G 配置

在 `src/App/Configs/Config.h` 中填写可选手机热点：

```cpp
#define CONFIG_WIFI_SSID     "测试热点名"
#define CONFIG_WIFI_PASSWORD "测试密码"
```

SSID 为空时不启动 Wi-Fi。SSID 非空时系统自动连接热点；查询时热点已连接则优先使用 Wi-Fi，否则回退 UART1 4G 模块。APN、服务器和端口也在同一文件中配置。

当前测试 API：

```http
GET /api/nearby?lat=22.551365&lng=113.881238
```

响应字段：

```json
{
  "distance_m": 120,
  "target_lat": 22.551650,
  "target_lng": 113.880720,
  "name": "公共厕所",
  "address": "测试地址"
}
```

当前参考服务是明文 HTTP，只适合受控实验，不适合生产位置数据。生产版本必须定义认证、TLS、超时、重试、限流和隐私策略。

## 6. SOS 配置

默认紧急号码为空，未配置时只在 UI 显示“未配置可发送短信的紧急联系人”。可在 `CONFIG_EMERGENCY_PHONE_DEFAULT` 设置测试号码，或让联系人 CSV 提供未脱敏号码；真实号码不得提交到仓库。

短信为英文 ASCII，降低模块字符集差异：

```text
[CareGO SOS] Need help. Location: <lat>,<lng>. Please contact me.
```

启用 `CONFIG_CELLULAR_ENABLE_SMS=1` 后，固件使用 `AT+CMGF=1` 和 `AT+CMGS`。日志出现“模块已受理”只代表 AT 返回成功，不代表接收端最终收到，仍需做端到端验收。

## 7. DFPlayer 文件

除原有 `0001.mp3`～`0011.mp3` 外，本版本新增：

| 文件 | 用途 |
| --- | --- |
| `0012.mp3` | 普通话求助语音 |
| `0013.mp3` | 附近有厕所提醒 |
| `0014.mp3` | GPS 暂不可用 |
| `0015.mp3` | 正在发送 SOS |
| `0016.mp3` | SOS 已受理 |
| `0017.mp3` | SOS 失败 |
| `0018.mp3` | 开始回家导航 |

文件放在 DFPlayer 卡根目录的 `/mp3/`，使用四位数字文件名。

## 8. 后续 SD、IMU 与地磁计

- 独立数据 SD 与屏幕复用 VSPI 的 SCK18/MISO19/MOSI23，SD 独占 CS4；驱动已去除旧项目 HSPI/CS15 硬编码。
- 实体 SD 接入后，将 `CONFIG_ENABLE_SD_CARD` 改为 `1`，把 `4.Software/Simulator/SD/MAP/` 内容复制到卡根目录 `/MAP/`，再验证屏幕与 SD 的 CS 互斥。
- 触摸已占用 I²C 21/22；IMU/地磁计应复用同一总线，地址不能冲突。IMU INT1 预留 GPIO35，INT2 预留 GPIO39。
- `HAL_Buzz`、`HAL_Encoder`、`HAL_Power` 及旧 Power/StatusBar 账户已从当前产品链路移除。

## 9. 验收顺序

1. 不连接 4G 和 GPS，运行 LVGL 仿真器，检查首页、导航、亲情守护、设置和设备状态页。
2. 接 GPS，观察设备状态页卫星数和精度；室外等待有效定位。
3. 把独立按键接到 GPIO32 与 GND，使用板载 BOOT 作为 GPIO0 按键；确认串口出现 `Keys: armed after stable release` 后逐项验证单击与双击。
4. 接 DFPlayer，先只验证 `0001.mp3`，再补齐 `0012.mp3`～`0018.mp3`。
5. 接 4G，仅验证 `AT`、`CPIN`、`CREG` 和信号状态，不开启外部请求。
6. 使用测试热点/测试服务器开启厕所查询。
7. 最后使用测试号码开启 SMS，核对发送受理和实际到达。

每一步均保留串口日志；如果出现重启，记录完整 `Guru Meditation`、Backtrace 和对应 `firmware.elf`，不要只截最后一行。
