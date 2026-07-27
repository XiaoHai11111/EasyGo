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

当前启动链路为 `Startup -> DummyHome`。DummyHome 的三个入口分别进入 `Ctrl`、`Teach` 和 `SystemInfos`。

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

## 2026-07-26 验证记录

- `Debug|x64` 独立输出构建通过。
- 启动动画结束后进入 DummyHome，三个按钮及其图标正常显示。
- Ctrl、Teach、SystemInfos 三个入口均完成实际点击验证。
- Ctrl 返回按钮与 Teach 画布返回操作可以回到 DummyHome。
- 运行期间未再触发 `Account::Commit()` 的空 `this` 异常。
