# 自平衡小车（STM32F103 + HAL 库 + FreeRTOS）

两轮自平衡小车。STM32F103C8T6 上用 MPU6050 测姿态，三环级联 PID 控制两个带编码器的直流减速电机，让车身保持竖直并能用蓝牙遥控。

> **关于本项目的来历**
>
> 本项目以 B 站 UP 主**江协科技（江科大）**的 STM32 标准库平衡车项目为原型，教学视频见 https://www.bilibili.com/video/BV1kBanzfExg/ 。他那一版基于**标准库**、手写工程；这一版是我在此基础上做的**移植与重构**：改用 **HAL 库 + FreeRTOS**，工程用 **CubeMX 生成 + CLion 开发**。
>
> 硬件清单、控制方案和代码逻辑的实现思路**均以江协科技官方为准**，本仓库主要做的是外设驱动的 HAL 化改写，以及把原来的裸机主循环拆成 FreeRTOS 任务。学习时建议对照官方视频一起看。

固件架构：**PID 控制环跑在 TIM1 中断里**（硬实时，不受调度器影响），**应用逻辑拆成三个 FreeRTOS 任务**（按键 / 通信 / 显示）。

---

## 硬件

| 部件 | 型号 / 规格 |
|---|---|
| 主控 | STM32F103C8T6（Cortex-M3，72 MHz，64 KB Flash / 20 KB RAM，LQFP48） |
| 姿态传感器 | MPU6050 六轴（三轴加速度计 + 三轴陀螺仪） |
| 显示 | 0.96" OLED，128×64，SSD1306 兼容，I2C 地址 `0x78` |
| 电机驱动 | TB6612 类模块（`AIN1/AIN2/BIN1/BIN2` + `PWMA/PWMB`） |
| 电机 | 直流减速电机 ×2，带霍尔编码器，减速比 9.27666，磁铁每转 44 计数 |
| 蓝牙 | JDY 系列串口透传模块，接 USART2，**9600 8N1** |

MPU6050 和 OLED 都走**软件 I2C**（GPIO 位翻转），没有用硬件 I2C 外设。

## 引脚分配

| 引脚 | 功能 | 说明 |
|---|---|---|
| PA0 | `PWMA` | 左电机 PWM，TIM2_CH1 |
| PA1 | `PWMB` | 右电机 PWM，TIM2_CH2 |
| PA2 / PA3 | USART2 TX / RX | 蓝牙模块 |
| PA4 / PA5 | Key4 / Key3 | 未使用 |
| PA6 / PA7 | E1A / E1B | 左编码器，TIM3 |
| PA9 / PA10 | USART1 TX / RX | 有线串口（`printf` 重定向到这里） |
| PA13 / PA14 | SWDIO / SWCLK | 下载调试 |
| PB0 / PB1 | Key2 / Key1 | K2 = 提交参数，K1 = 启动/停止 |
| PB6 / PB7 | E2A / E2B | 右编码器，TIM4 |
| PB8 / PB9 | `O_SCL` / `O_SDA` | OLED 软件 I2C |
| PB10 / PB11 | `M_SCL` / `M_SDA` | MPU6050 软件 I2C |
| PB12 / PB13 | `AIN1` / `AIN2` | 左电机方向 |
| PB14 / PB15 | `BIN1` / `BIN2` | 右电机方向 |
| PC13 | LED | 板载 LED，**低电平点亮**，指示 PID 是否在运行 |

## 定时器分配

只有 TIM1~4，且全部占满：

| 定时器 | 用途 | 配置 | 频率 |
|---|---|---|---|
| TIM1 | 控制中断（更新中断） | PSC = 71，ARR = 999 | **1 kHz（1 ms）** |
| TIM2 | 双路电机 PWM | PSC = 35，ARR = 99 | **20 kHz** |
| TIM3 | 左编码器 | 编码器模式 | — |
| TIM4 | 右编码器 | 编码器模式 | — |

因为四个定时器都用完了，**HAL 时基和 FreeRTOS tick 共用 SysTick**（两者都是 1 kHz，可以共存）。工程里没有用到 `HAL_Delay`。

---

## 控制结构

三环级联 PID，全部在 TIM1 中断里执行：

```
                 ┌──────────────── 10 ms ────────────────┐
  速度环 (50ms)   │                                        │
  AveSpeed ──►[SpeedPID]──► Target ──►[AnglePID]──► -Out ──┼──► AvePWM ──┐
                                          ▲                │            │
  姿态解算 ──► Angle ─────────────────────┘                │            ├─► LeftPWM  = AvePWM + DifPWM/2
  (互补滤波)                                               │            └─► RightPWM = AvePWM - DifPWM/2
                                                           │
  转向环 (50ms)                                             │
  DifSpeed ──►[TurnPID]──► DifPWM ─────────────────────────┘
```

**时间分频**：TIM1 每 1 ms 进一次中断，内部用两个计数器分频 —— 每 10 次（**10 ms**）跑一次角度环，每 50 次（**50 ms**）跑一次速度环和转向环。

**姿态解算**（10 ms 一次）：

1. 读 MPU6050 原始值
2. `GY = GYraw - 16` —— 减去陀螺仪 Y 轴零漂（**每台设备不同，需实测**）
3. `AngleAcc = -atan2(AX, AZ) / π × 180 + 0.5` —— 加速度计算角度，`+0.5` 是中心角度校准（**同样需实测**）
4. `AngleGyro = Angle + GY / 32768 × 2000 × 0.01` —— 陀螺仪积分（满量程 ±2000°/s）
5. `Angle = 0.01 × AngleAcc + 0.99 × AngleGyro` —— 互补滤波，系数 0.01 偏重陀螺仪

**串级关系**：`AnglePID.Target = SpeedPID.Out`。速度环的输出单位是**角度**，所以速度环和角度环的增益数值不能直接比较。

**输出链路**：

```c
AvePWM   = -AnglePID.Out;          // 角度环输出（取负，因为车倒的方向和电机驱动的方向相反）
LeftPWM  = AvePWM + DifPWM / 2;
RightPWM = AvePWM - DifPWM / 2;
// 限幅到 ±100 后输出给电机
```

**保护**：`|Angle| > 50°` 时判定倒地，`RunFlag` 清 0，电机停转。这是中断里写的，优先级最高的安全逻辑。

### 默认 PID 参数

| 环 | Kp | Ki | Kd | 输出限幅 | 积分限幅 | 备注 |
|---|---|---|---|---|---|---|
| 角度环 | 5 | 0.1 | 5 | ±100 | ±600 | 另有 `OutOffset = 3`（输出偏移，克服静摩擦死区） |
| 速度环 | 2 | 0.05 | 0 | ±20 | ±150 | 输出单位是角度（度） |
| 转向环 | 4 | 3 | 0 | ±50 | ±20 | |

参数是**手工整定**出来的，不是算出来的。角度环的物理含义比较直观：1° 误差 → 5% 的 PWM 输出（输出 ±100 对应 PWM ±100%）。

---

## 软件架构

### FreeRTOS 任务

| 任务 | 职责 | 优先级 | 栈 |
|---|---|---|---|
| `keyTask` | 每 10 ms 读一次键码：K1 启停，K2 提交蓝牙参数 | `AboveNormal` | 384 B |
| `commTask` | 阻塞等蓝牙队列，解析数据包 | `Normal` | 2048 B |
| `displayTask` | 每 50 ms 刷新 OLED，并控制 LED 指示运行状态 | `BelowNormal` | 2560 B |
|（IDLE） | 内核空闲任务 | 0 | — |

优先级次序 key > comm > display：按键是输入响应，显示是纯装饰性的，可以被牺牲。三个任务绝大多数时间都睡在 `osDelay` 上，所以不存在饿死。

### 为什么 PID 不放进任务

控制环留在 **TIM1 中断**里，一行都没搬进任务。理由是硬实时：PID 周期必须是确定的 10 ms / 50 ms，放进任务就会被调度器、被其他任务的执行时间影响，抖动不可控。中断优先级高于所有任务，抢占是确定的。

### 共享数据怎么保护

原则：**能不加锁就不加锁，只修真正会坏的地方。**

Cortex-M3 上对齐的 32 位读写本身是**单次原子**的，所以：

| 数据 | 谁写 / 谁读 | 保护方式 |
|---|---|---|
| `RunFlag`、`Angle`、`AveSpeed`、`DifSpeed`、`GY` | ISR 写，任务读 | 只加 `volatile`，**不加锁** |
| PID 的 `Kp/Ki/Kd/Offset` | 任务写，ISR 读 | **不加锁**，单个 float 写入是原子的 |
| `PID_Init()` 一次清零多个字段 | — | **挪进 ISR 执行**（见下） |
| 蓝牙数据包 | UART ISR 写，任务读 | **队列**（拷贝传递，不共享内存） |

**临界区一个都没有**，这是刻意的：TIM1 中断优先级是 5，`taskENTER_CRITICAL()` 会把控制环一起屏蔽掉。所以任何"任务侧加锁"的方案都会拖慢甚至停摆控制环。

**两个请求标志位的设计**（`StartReq` / `ApplyReq`）：

任务的职责只是**发请求**，真正改结构体的动作交给 TIM1 中断去做 —— 中断里本来就没有并发。

- **启动**（K1）：`PID_Init()` 一次清零 7 个状态字段。如果在 `keyTask` 里做，ISR 可能读到半新半旧的参数，控制周期里电机踹一脚。改成 `keyTask` 置 `StartReq`，中断消费 → 代价是最多 10 ms 的启动延迟，人感觉不到。
- **提交参数**（K2）：蓝牙收到的 PID 参数先落在**暂存区**（`AnglePID_Stage` 等），按 K2 后才由中断整体拷贝给实时 PID。所以手机上误拖滑杆不会立刻让车抽风。

注意 `PID_Init()` 只清零状态量（`Target`/`Actual`/`Out`/`Error*`/`ErrorInt`），**增益和限幅会保留**，所以重启不会丢掉你整定好的参数。

### 显示快照

`displayTask` 每轮开头把共享变量拷到任务本地副本，之后**只用副本渲染**：

```c
PID_t ang = AnglePID;   // 结构体拷贝，-O0 下编译成 ldmia/stmia，约 70 条指令
float angle = Angle;
...
```

作用是把"读到不一致数据"的时间窗口从"22 次 `OLED_Printf` 的耗时"（带软浮点的 `vsprintf`，好几毫秒）压缩到约 1 µs，碰撞概率约 0.01%。**临界区绝对不能跨 `OLED_Printf`。**

---

## 蓝牙协议

数据包用 `[` 开头、`]` 结尾，中间用逗号分隔：

```
[标签,数据1,数据2,...]
```

接收在 USART2 中断里用状态机组包，攒够一包后投进 FreeRTOS 队列，`commTask` 阻塞取出并解析。

| 包 | 格式 | 行为 |
|---|---|---|
| 滑杆 | `[slider,AngleKp,8]` | 写**暂存区**，按 K2 后才生效 |
| 摇杆 | `[joystick,LH,LV,RH,RV]` | `SpeedPID.Target = LV/25`、`TurnPID.Target = RH/25`，**直接生效**（不经过暂存区） |
| 按键 | `[key,...]` | 预留，未实现 |

滑杆名称支持：`AngleKp` `AngleKi` `AngleKd` `SpeedKp` `SpeedKi` `SpeedKd` `TurnKp` `TurnKi` `TurnKd` `Offset`。

> 注意波特率是 **9600**，不是常见的 115200。手机端 app 和串口助手都要对上。

---

## 编译与烧录

本工程用 **CubeMX + CLion** 开发，所以编译和烧录也都在 CLion 里完成，不需要敲命令行。

### 前置条件

本工程的开发环境（`arm-none-eabi-gcc` 工具链、OpenOCD、烧录器配置）是照着 **B 站 UP 主 keysking** 的视频搭起来的，跟着做一遍即可，这里不赘述：

**《CLion 开发 STM32 轻松搞定！》** —— https://www.bilibili.com/video/BV1ren2zMEaS/

### 编译和烧录

用 CLion **打开工程根目录** `Hal+FreeRTOS/`（不是里面的 `build/`），然后：

1. **选预设** —— CLion 会自动读取 `CMakePresets.json`。在 CMake 工具窗里选 `Debug`，点 **Reload CMake Project**。（之后只要改过 `CMakeLists.txt`，都得重新 Reload 一次）
2. **编译** —— **Build → Build Project**（快捷键 `Ctrl+F9`）
3. **烧录** —— 配置好烧录器之后点运行，就下载进芯片了

### 资源占用（Debug 构建）

```
FLASH  61956 B / 65536 B  = 94.6%
RAM    15976 B / 20480 B  = 78.0%
```

**Flash 已经比较紧**。主要开销是 `CMakeLists.txt:110` 的 `-u _printf_float` —— 它强制链接浮点版 `printf`，OLED 上直接格式化浮点数就是靠它。想去掉的话，需要改成先转成整数再打印。

> 仓库里没有 `Drivers/CMSIS/Lib/` 和 `Drivers/CMSIS/DSP/`（约 49 MB）。那是 CubeMX 整包带过来的 ARM 预编译库和 DSP 包，本工程用 GCC 编译、一行都没引用（`CMakeLists.txt` 里是**逐个列出源文件**，不是 GLOB），删掉不影响构建。

---

## 操作说明

| 操作 | 效果 |
|---|---|
| **K1** 按下 | PID 未运行时 → 启动（LED 亮）；PID 运行中 → 停止（LED 灭） |
| **K2** 按下 | 把蓝牙暂存的 PID 参数提交给实时 PID |
| 手机蓝牙发 `[slider,AngleKp,8]` | OLED 上的 `AngleKp` 变成 `8.00`，按 K2 后实际生效 |

**上电后 PID 默认是停止的**，必须按 K1 才启动。OLED 分三栏显示三个环的参数（最左角度环、中间速度环、最右转向环），每栏从上到下是 `Kp / Ki / Kd / Target / Actual / Out`。角度环那栏最底下还有 `GY`（陀螺仪实时值）和 `Offset`。

**整定时看 OLED**：让车静止，`GY` 的读数就是陀螺仪零漂，把它填进 `main.c` 的 `GY = GYraw - 16`；让车绝对竖直，看角度环的 `Actual`（就是 `Angle`），那个值就是中心角度偏移，填进 `AngleAcc += 0.5`。

**注意**：车必须大致竖直才能启动。如果歪超过 50°，倒地的保护会立刻把 `RunFlag` 清零 —— 表现出来就是"按了 K1 没反应"。

---

## 目录结构

```
Core/
├── App/                      # 应用层，CubeMX 不生成、不会覆盖
│   ├── Tasks/
│   │   ├── app_tasks.c       # 汇总建任务
│   │   ├── KeyTask.c         # 按键任务
│   │   ├── CommTask.c        # 蓝牙解析任务 + 队列
│   │   ├── DisplayTask.c     # OLED 刷新任务
│   │   └── AppState.c        # 共享状态的唯一归属文件
│   └── Types/
│       ├── AppState.h        # 共享变量声明（extern）
│       └── ...               # CommTask.h / KeyTask.h / DisplayTask.h
├── Inc/                      # 驱动层头文件
├── Src/
│   ├── main.c                # CubeMX 生成，含 TIM1 中断里的 PID 控制环
│   ├── freertos.c            # CubeMX 生成，只在 USER CODE 区加了 App_TasksCreate()
│   ├── usart.c               # 蓝牙组包状态机 + 投队列
│   ├── MPU6050.c / OLED.c / Motor.c / Encoder.c / PID.c / Key.c ...
│   └── ...
Middlewares/                  # FreeRTOS 内核 + CMSIS-RTOS2 包装层
Drivers/                      # STM32 HAL + CMSIS
cmake/                        # 工具链文件
Hal+FreeRTOS.ioc              # CubeMX 工程文件
```

**用 CubeMX 改配置时的约束**：`main.c`、`freertos.c`、`usart.c`、`FreeRTOSConfig.h` 都是 CubeMX 生成的，**只能在 `USER CODE BEGIN/END` 区内改**，否则重新生成代码会被覆盖。`Core/App/` 整个目录 CubeMX 不认识，任务代码放这里最安全。`FreeRTOSConfig.h` 是纯生成文件、没有 USER CODE 区，所以它的参数（堆大小、tick 频率）只能在 CubeMX 的 GUI 里改。

---

## 待改进

- Flash 占用 94.6%，逼近上限。`-u _printf_float` 是主要开销。
- 转向环目前只有 `DifSpeed`（左右轮速差）反馈，没有陀螺仪 Z 轴参与，抗扭转扰动的能力有限。

---

## 声明

本项目的原型来自 B 站 UP 主 **江协科技（江科大）** 的 STM32 标准库平衡车项目，**仅供学习与参考，禁止用于任何商业项目**。

- 原项目教学视频：https://www.bilibili.com/video/BV1kBanzfExg/
- 硬件清单、控制方案与代码逻辑的实现**均以江协科技官方为准**。本仓库所做的是把它移植到 HAL 库 + FreeRTOS，并调整工程结构，不涉及控制方案本身的原创。
- 若本仓库的描述与官方内容有出入，**一律以官方为准**。
