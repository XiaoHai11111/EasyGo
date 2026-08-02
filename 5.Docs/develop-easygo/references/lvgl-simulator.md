# LVGL 页面与 Windows 模拟器

## 事实来源

- 页面与资源源码：`3.Firmware/PlatformIO/EasyGo-ESP32-fw/src/App/`
- 模拟器解决方案：`4.Software/Simulator/LVGL.Simulator.sln`
- 模拟器项目：`4.Software/Simulator/LVGL.Simulator/LVGL.Simulator.vcxproj`
- Visual Studio 筛选器：`4.Software/Simulator/LVGL.Simulator/LVGL.Simulator.vcxproj.filters`
- 当前验证配置：Visual Studio 2019、`Debug|x64`

模拟器直接编译固件目录中的页面、账户、资源和通用组件。不要复制一份页面源码到模拟器中，也不要恢复对旧 `Peak-ESP32-fw` 目录或个人绝对路径的引用。

## 同步页面变更

1. 把页面逻辑和资源修改保留在 `EasyGo-ESP32-fw/src/App/`。
2. 新增或删除参与 PC 仿真的 `.c`、`.cpp` 或 `.h` 文件时，同步修改 `.vcxproj`。
3. 同步修改 `.vcxproj.filters`，使 Visual Studio 中的文件分组与工程项一致。
4. 在 `ResourcePool.cpp` 新增 `IMPORT_IMG` 或 `IMPORT_FONT` 时，把对应生成源文件加入 `ClCompile`；否则编译能通过但链接会出现无法解析的资源符号。
5. 使用相对于模拟器项目的路径 `..\..\..\3.Firmware\PlatformIO\EasyGo-ESP32-fw\...`，避免机器相关绝对路径。
6. 修改后搜索 `Peak-ESP32-fw`、`2.Firmware` 和旧绝对路径，确认模拟器工程中没有遗留引用。

## 页面接入检查

新增或迁移页面时，需要同时完成以下接入点；只把 `.cpp` 加入模拟器工程并不会让页面自动出现：

1. 在 `App.cpp` 中用 `manager.Install()` 注册页面名称与路径。
2. 在 `Pages/AppFactory.cpp` 中包含页面头文件并添加 `APP_CLASS_MATCH()`。
3. 从启动页或上级页面使用已注册的完整路径执行 `Manager->Push()`。
4. 在 `ResourcePool.cpp` 中注册页面使用的全部图片和字体资源。
5. 确认资源源文件及页面源文件同时存在于 `.vcxproj` 和 `.vcxproj.filters`。

当前启动链路为 `Startup -> Home`。Home 的三个入口分别进入 `Navigation`、`Family` 和 `Setting`，Setting 可进入 `DeviceStatus`。这些页面继续使用 `Page / View / Model / ResourcePool` 架构；页面 Model 通过 `CareGoClient` 订阅 `CareGo` 固定大小状态，不直接读取硬件 HAL。

## 分辨率、字体与模拟 SD 卡

- 固件 `CONFIG_SCREEN_HOR_RES/VER_RES` 和 Windows 窗口均为 `240×320`。
- EasyGo 中文字体子集由 `4.Software/UI/tools/build_lvgl_assets.py` 从本机 Microsoft YaHei 生成，输出为 `font_easygo_12/14/16/20/28.c`。
- 字体脚本必须使用与 `getbbox(anchor="ls")` 相同的基线锚点绘制字形；否则生成的位图会透明而只留下页面图标。
- 模拟 SD 卡根目录为 `4.Software/Simulator/SD/`。启动器从 EXE 所在目录向上查找 `SD/MAP/shenzhen.bin`，找到后设置工作目录，再由 LVGL 路径 `/SD/MAP/shenzhen.bin` 加载。
- 当前地图是深圳市宝安区 OSM 原始数据的本地渲染，来源、范围、日期、许可和用途限制见 `4.Software/Simulator/SD/MAP/ATTRIBUTION.md`；二进制格式见 `4.Software/Simulator/SD/MAP/MAP_FORMAT.md`。
- 联系人仿真文件为 `4.Software/Simulator/SD/CONTACTS/contacts.csv`，每行格式是 `姓名,脱敏号码,头像样式编号`。PC 使用 `/SD/CONTACTS/contacts.csv`，实体 SD 卡放在 `/CONTACTS/contacts.csv` 并通过 `S:/CONTACTS/contacts.csv` 读取。
- FamilyModel 最多读取 16 条联系人；空行和 `#` 注释会被忽略，文件不存在或没有有效记录时使用内置的 3 条脱敏演示联系人。

### SD 路径映射

| 数据 | PC 宿主文件 | PC LVGL 路径 | 实体 SD 卡位置 | 固件 LVGL 路径 |
| --- | --- | --- | --- | --- |
| 宝安区地图 | `4.Software/Simulator/SD/MAP/shenzhen.bin` | `/SD/MAP/shenzhen.bin` | `/MAP/shenzhen.bin` | `S:/MAP/shenzhen.bin` |
| 地图层级索引 | `4.Software/Simulator/SD/MAP/shenzhen_levels.bin` | `/SD/MAP/shenzhen_levels.bin` | `/MAP/shenzhen_levels.bin` | `S:/MAP/shenzhen_levels.bin` |
| 宝安区厕所 POI | `4.Software/Simulator/SD/MAP/shenzhen_toilets.bin` | `/SD/MAP/shenzhen_toilets.bin` | `/MAP/shenzhen_toilets.bin` | `S:/MAP/shenzhen_toilets.bin` |
| 联系人 | `4.Software/Simulator/SD/CONTACTS/contacts.csv` | `/SD/CONTACTS/contacts.csv` | `/CONTACTS/contacts.csv` | `S:/CONTACTS/contacts.csv` |

不要把 PC 宿主目录中的 `SD` 文件夹名称复制到实体卡根目录。使用 `_WIN32` 或统一存储适配层选择路径，避免在 View 中拼接平台路径。

### 地图与厕所 POI 数据边界

- `shenzhen.bin` 是供 LVGL 显示的 RGB565 像素数据，不包含可供运行时搜索的 POI 结构。
- `shenzhen_baoan.osm.json` 是当前宝安区地图与 POI 的 OSM 原始快照，包含主要道路以及 89 个 `amenity=toilets` 对象。
- `shenzhen_toilets.bin` 是适合 MCU 顺序读取的定长 POI 索引，保存 OSM 标识、WGS84 经纬度、对象类型、名称和有限属性；头部、记录、比例尺与 CRC32 见 `MAP_FORMAT.md`。
- `ACT_Network` 已接入 POI 索引顺序扫描，按 WGS84 Haversine 直线距离选择最近记录；道路路线、开放时间过滤和可达性判断尚未实现。
- 地图缩放使用 1×、2×、4×、8×、16×、32×、64× 七个 720×492 RGB565 大画布层级；`shenzhen.bin` 是默认 8×，`shenzhen_z3.bin` 是其相同内容别名。按钮通过 Model 选择平台路径并让 View 切换图片源。
- 地图显示区是 240×164 的双向滚动视口，初始滚动到 `(240, 164)`。底图、路线、当前位置和厕所目标位于同一个 720×492 内容层并随触摸拖动；右侧缩放按钮、行政区标签和 OSM 署名位于根层并保持固定。
- `shenzhen_levels.bin` 使用 `EGMAPL2` 版本 2 头部，记录画布/视口尺寸、24 像素画布内容边距、初始滚动位置、七级 WGS84 边界、米/像素和 CRC32。当前模拟焦点是 `(22.5533410, 113.8782710)`；最大 64× 层级约为横向 2.1 米/像素、纵向 4.0 米/像素。
- 不要对当前文件源调用 `lv_img_set_zoom()`。内置解码器按行读取 `.bin`，变换代码却需要完整图片缓冲，会导致地图透明或消失；若未来改为内存图片，先验证约 692 KiB 单级图片数据及变换开销在目标硬件上的连续内存预算。
- PC 鼠标拖动由 Windows 指针输入模拟触摸。真机要获得相同行为，必须在 LVGL 中注册并校准 `LV_INDEV_TYPE_POINTER` 触摸设备；只有编码器输入时页面不会响应触摸拖动。
- 查询结果必须区分直线距离、道路距离和步行时间。当前固定的 `120 米`、`约 2 分钟` 与路线仅用于界面演示。
- 保留 `ATTRIBUTION.md`，更新地图或 POI 数据时同步记录范围、来源、许可、抓取日期和生成方式。

## 账户初始化

- 把 `App/Accounts/_ACT_LIST.inc` 作为运行时账户清单；`Accounts_Init()` 只会创建并初始化该清单中的账户。
- 在 `AccountSystem::*_Commit()` 包装层检查缓存的账户指针和输入参数，再调用 `Account::Commit()`。
- 不要尝试在 `Account::Commit()` 内通过判断 `this` 修复空对象调用；调用空对象成员函数本身已是未定义行为。
- 迁移旧 HAL 模拟数据源时，先确认对应账户仍在清单中且存在订阅者。没有账户或消费者的旧更新通道应从 `HAL_Update()` 移除。

## 验证

1. 检查 `.vcxproj` 和 `.vcxproj.filters` 是合法 XML。
2. 检查所有指向 `EasyGo-ESP32-fw` 的工程项均存在。
3. 构建 `LVGL.Simulator.sln` 的 `Debug|x64` 配置。
4. 启动生成的 `LVGL.Simulator.exe`，等待页面初始化并确认进程持续响应。
5. 把编译通过、启动通过和视觉/交互验证分开记录；进程存活不能替代页面视觉与交互验收。

若解决方案正被 Visual Studio 打开，外部 MSBuild 可能因中间文件被占用而无法清理默认 `Output/`。优先在当前 Visual Studio 实例中构建，或关闭占用后再从命令行构建。

## 2026-07-29 验证记录

- `Debug|x64` 构建通过，输出为 `4.Software/Simulator/Output/Debug/x64/LVGL.Simulator.exe`。
- 启动页、Home、Navigation、Family、Setting 均完成实际窗口截图检查，中文、背景和图标正常显示。
- Navigation 已验证模拟 SD 地图、路线、厕所目标、当前位置、放大/缩小和返回。
- 宝安区地图已验证为 240×174 RGB565 LVGL 文件；厕所索引已验证包含 89 条记录，文件尺寸、WGS84 边界和 CRC32 均正确。
- Family 已验证三个演示联系人、模拟呼叫与演示求助；不会拨号、发消息或传输位置。
- Family 已进一步验证从 SD CSV 动态加载 8 条联系人、自动生成卡片以及纵向滚动条；顶部标题和底部 SOS 固定不滚动。
- Setting 已验证语音开关和返回首页。
- 地图中的距离、预计时间和线路为固定演示数据，不作为路线正确性或安全性验证。
- 运行期间未再触发 `Account::Commit()` 的空 `this` 异常。

## 2026-07-30 验证记录

- 修复 SD 文件源调用 `lv_img_set_zoom()` 后地图消失的问题，改为五级离线地图文件切换。
- `Debug|x64` 重新构建通过。
- 在实际模拟器窗口中从默认层级连续放大到最大、缩小到最小，并在最小层级额外点击缩小；各层级地图均持续显示且比例变化可见。
- 原五级 0.84×–1.36× 方案因总跨度仅约 1.62 倍而废止；已替换为 1×–64× 七级方案，并再次完成实际窗口逐级放大、逐级缩小验证。
- `shenzhen_levels.bin` 已验证头部、七条记录、文件长度和 CRC32；固件 Model 提供同边界的 WGS84 E7 到屏幕像素投影方法。
- 地图资源已扩展为 720×492 大画布，地图视口支持双向拖动且边界受限；实际窗口已验证拖动时底图、路线和标记整体移动，固定缩放按钮不随地图移动。
- 拖动后点击放大仍能正常切换文件并保持地图显示；`Debug|x64` 与 ESP32 `esp32dev` 构建均通过。2026-07-31 的 ESP32 Dev Module 构建资源占用为 RAM 9.6%、Flash 62.1%。

## 2026-08-01 验证记录

- 新增 Location、Network、Input、CareGo 发布订阅账户以及 `CareGoClient` 页面适配层；模拟器工程已同步新账户、服务和 DeviceStatus 页面，并移除旧 Power、Buzz、Encoder 和 StatusBar 工程项。
- `Debug|x64` 生成成功，0 个错误；启动实际窗口后依次点击并截图检查 Home、Navigation、Setting 和 DeviceStatus，中文、地图和实时状态字段正常显示。
- 仿真位置触发离线 `shenzhen_toilets.bin` 最近厕所查询；导航页显示由账户状态提供的目标和距离。
- ESP32 `esp32dev` 最终构建成功，RAM 12.8%（41,988 / 327,680），Flash 81.4%（1,600,469 / 1,966,080）。
- 真实 GPS、4G、短信、按键、SD、IMU 和地磁计均未在本次验证中连接；真实外部请求与 SMS 配置仍为关闭。
