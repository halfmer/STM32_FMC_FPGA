# STM32 FMC 与 FPGA 通信自检工程说明

## 1. 工程作用

本工程用于验证 STM32H723 与 Pango Logos PGL12G FPGA 之间的 16 位 FMC 并口通信是否稳定可靠。

当前工程的核心功能是：

- STM32 通过 FMC 总线访问 FPGA 内部寄存器。
- FPGA 在自检模式下提供固定签名、固定常量、回环寄存器、50 MHz 心跳、130 MHz PLL 心跳和调试状态寄存器。
- STM32 周期性读取这些寄存器，并在 4.3 寸 LCD 上显示通信状态。
- LCD UI 使用“按变化刷新”的方式，只刷新变化的数字和状态，减少整行重绘导致的闪屏。

这个自检功能的意义在于：在正式传输 ADC/FIFO 数据之前，先确认硬件连线、FMC 时序、FPGA 复位、PLL 状态、跨时钟域心跳和 STM32 读写路径都是可用的。只要自检稳定通过，后续 FIFO 数据采集问题就可以更集中地排查数据路径本身，而不是先怀疑总线基础通信。

## 2. 目录结构

关键目录如下（部分关键文件）：

```text
fpga_stm/
├── fpga_pgl12g/                 # PGL12G FPGA 工程
│   ├── source/
│   │   ├── body.v               # FPGA 顶层，PLL、复位滤波、FMC 模块连接
│   │   ├── fmc.v                # STM32 FMC 总线接口与自检寄存器
│   │   ├── fifo_and_fmc.v       # FIFO 数据采集与 FMC 接口封装
│   │   └── my_fifo.v            # FIFO 相关逻辑
│   └── constraint/
│       └── timing.fdc           # FPGA 时序约束
├── stm/                         # STM32H723 工程
│   ├── Core/Src/main.c          # 主循环，初始化 LCD/FMC，并调用自检显示
│   ├── Hardware/FMC_FPGA.c      # STM32 侧 FMC 读写、自检判断和 LCD UI
│   ├── Hardware/FMC_FPGA.h      # FPGA FMC 地址窗口和函数声明
│   └── LCD/Show/                # LCD 驱动
└── sim_check/                   # 仿真检查相关文件
```

## 3. 整体通信流程

当前自检链路如下：

```text
STM32 main loop
    ↓
FMC_SelfTest_Display()
    ↓
FMC_WR_ADDR(addr) 选择 FPGA 寄存器地址
    ↓
FMC_RD_DATA() 或 FMC_WR_DATA(data)
    ↓
STM32 FMC 总线引脚 CS/RS/WR/RD/SD[15:0]
    ↓
FPGA fmc.v 同步采样总线控制信号和数据
    ↓
FPGA 根据 ADDR 返回 RD_DATA
    ↓
STM32 判断 PASS/FAIL 并刷新 LCD
```

STM32 侧不是直接访问普通变量，而是通过 `FMC_FPGA.h` 中的地址映射访问 FPGA：

```c
typedef struct
{
    volatile uint16_t FPGA_REG;
    volatile uint16_t FPGA_RAM;
} FPGA_TypeDef;

#define FPGA_FSMC_NEX 2
#define FPGA_FSMC_AX  12
#define FPGA_BASE     ...
#define FPGA          ((FPGA_TypeDef *) FPGA_BASE)
```

其中：

- `FPGA_REG` 用于写 FPGA 内部地址。
- `FPGA_RAM` 用于读写该地址对应的数据。
- `RS=0` 时 FPGA 认为 STM32 在写地址。
- `RS=1` 时 FPGA 认为 STM32 在写数据或读数据。

## 4. FPGA 自检寄存器说明

当前 `fpga_pgl12g/source/fmc.v` 中打开了：

```verilog
`define SELFTEST_MODE
```

因此 FPGA 运行在上板自检寄存器模式，而不是正式 FIFO 采集协议模式。

STM32 读取的寄存器表如下：

| 地址 | 名称 | 期望/含义 | 用途 |
|---|---|---|---|
| `0x0000` | SIGN | `0xA55A` | FPGA 签名，确认 STM32 能正确读到 FPGA 固定值 |
| `0x0001` | CONST | `0x1234` | 固定常量，辅助确认数据位宽和字节顺序没有错 |
| `0x0002` | HB50 | 递增计数 | 50 MHz FMC 时钟域心跳，确认 FPGA 主时钟和 FMC 逻辑在运行 |
| `0x0003` | LOOP | 写什么读什么 | 回环寄存器，确认 STM32 对 FPGA 的写数据路径可用 |
| `0x0004` | HB130 | 递增计数 | 130 MHz PLL 时钟域心跳，确认 PLL 输出和跨时钟域同步可用 |
| `0x0005` | STAT | `R0/1 S1 P1` | 调试状态，包含 STM32 复位输入、FPGA 内部复位释放、PLL lock |

`fmc.v` 中对应的读出逻辑：

```verilog
always @(*) begin
    case (ADDR)
        16'h0000: RD_DATA = 16'hA55A;
        16'h0001: RD_DATA = 16'h1234;
        16'h0002: RD_DATA = hb50;
        16'h0003: RD_DATA = loopreg;
        16'h0004: RD_DATA = hb130;
        16'h0005: RD_DATA = DEBUG_STAT;
        default:  RD_DATA = 16'hFEFE;
    endcase
end
```

如果 `0x0000/0x0001` 都不正确，优先检查 FMC 地址线、数据线、片选、读时序和 FPGA 是否完成配置。

如果固定值正确但 `LOOP` 不正确，优先检查 STM32 写数据路径、`RS/WR` 连接和 FPGA `data_write` 判断。

如果固定值和回环都正确但 `HB50/HB130` 不动，说明总线能读写，但 FPGA 时钟、复位或 PLL 相关逻辑可能存在问题。

## 5. FPGA 侧关键代码解读

### 5.1 `body.v`: 顶层连接

文件：`fpga_pgl12g/source/body.v`

`body.v` 是 FPGA 顶层，主要负责：

- 接入板载 50 MHz `clk`。
- 通过 PLL 生成 130 MHz `CLK_130M`。
- 将 `CLK_130M` 输出给 ADC/DAC 相关时钟。
- 对 STM32 的 `rst_32` 进行滤波，生成 FPGA 内部复位 `rst_n`。
- 实例化 `fifo_and_fmc`，把 FMC 信号接入 FPGA 内部。

复位滤波逻辑的目的：

```verilog
localparam [19:0] RST_RELEASE_CNT = 20'd999_999;  // 20 ms @ 50 MHz
localparam [15:0] RST_ASSERT_CNT  = 16'd49_999;   // 1 ms @ 50 MHz
```

- `rst_32` 必须持续高电平约 20 ms，FPGA 内部复位才释放。
- `rst_32` 必须持续低电平约 1 ms，FPGA 内部才重新复位。
- 这样可以过滤 STM32 NRST 引脚上的短毛刺，避免心跳计数器被偶发清零。

调试状态寄存器来自：

```verilog
.DEBUG_STAT ({13'd0, pll_lock, rst_n, rst32_sync[2]})
```

低 3 位含义：

| 位 | 名称 | 含义 |
|---|---|---|
| bit0 | raw rst | 同步后的 STM32 `rst_32` 输入状态 |
| bit1 | rst_n | FPGA 内部复位是否释放 |
| bit2 | pll_lock | PLL 是否锁定 |

STM32 侧当前期望 bit1 和 bit2 为 1，即内部复位已释放且 PLL 已锁定。

### 5.2 `fmc.v`: FMC 总线同步与寄存器

文件：`fpga_pgl12g/source/fmc.v`

FMC 的 `CS/RS/WR/RD/SD` 对 FPGA 来说不是严格同步信号，因此 `fmc.v` 没有直接使用 `posedge WR` 当时钟，而是用 50 MHz `CLK_IN` 对这些信号打拍：

```verilog
reg [2:0] cs_r, rs_r, wr_r, rd_r;
reg [15:0] sd_r0, sd_r1;
```

这样做的原因：

- 外部 FMC 控制线可能有边沿毛刺。
- 飞线或转接板连接会让 WR 边沿质量变差。
- 把控制信号同步到 FPGA 内部时钟域后，再用电平条件判断写地址/写数据，更稳。

地址写判断：

```verilog
wire addr_write = (cs_s == 1'b0) && (rs_s == 1'b0) && (wr_s == 1'b0);
```

数据写判断：

```verilog
wire data_write = (cs_s == 1'b0) && (rs_s == 1'b1) && (wr_s == 1'b0);
```

数据稳定判断：

```verilog
wire sd_stable = (sd_r0 == sd_r1);
```

只有在 `SD` 连续两拍一致时才锁存地址或数据，可以降低总线转换瞬间采错的概率。

读出三态驱动：

```verilog
assign SD = ((cs_s == 1'b0) && (rd_s == 1'b0)) ? RD_DATA : 16'hZZZZ;
```

也就是说，只有 STM32 发起读周期时 FPGA 才驱动 `SD[15:0]`，否则保持高阻，避免和 STM32 写周期冲突。

### 5.3 心跳计数

50 MHz 心跳：

```verilog
if (div50 >= 26'd49_999_999) begin
    div50 <= 26'd0;
    hb50 <= hb50 + 16'd1;
end
```

该计数约 1 秒加 1，用来确认 50 MHz FMC 域还在运行。

130 MHz 心跳先在 PLL 域翻转 `hb130_tick`，再同步到 50 MHz 读出域：

```verilog
if (hb130_tick_sync[2] ^ hb130_tick_sync[1])
    hb130 <= hb130 + 16'd1;
```

这说明 LCD 上看到的 `HB130` 不是直接跨域读取 130 MHz 计数器，而是在 50 MHz 域检测 130 MHz 域事件后递增。这样比直接跨时钟域读多位计数器可靠。

## 6. STM32 侧关键代码解读

### 6.1 `main.c`: 初始化和周期刷新

文件：`stm/Core/Src/main.c`

初始化阶段完成：

- `MX_GPIO_Init()`
- `MX_FMC_Init()`
- `MX_USART1_UART_Init()`
- `LCD_Init()`
- `tp_dev.init()`
- `my_fft_init()`

之后进入主循环：

```c
while (1)
{
    FMC_SelfTest_Display();   /* Read FPGA test registers and refresh the LCD UI. */
    HAL_Delay(100);
    ...
    HAL_Delay(100);
}
```

因此 LCD 自检界面大约每 200 ms 执行一次读取和刷新判断。注意：现在不是每 200 ms 全屏重绘，而是每 200 ms 读取一次数据，然后只刷新变化的 LCD 区域。

### 6.2 `FMC_FPGA.h`: FPGA 地址窗口

文件：`stm/Hardware/FMC_FPGA.h`

`FPGA_TypeDef` 把外部 FMC 地址空间抽象成两个 16 位寄存器：

```c
typedef struct
{
    volatile uint16_t FPGA_REG;
    volatile uint16_t FPGA_RAM;
} FPGA_TypeDef;
```

STM32 先写 `FPGA_REG` 选择 FPGA 内部地址，再通过 `FPGA_RAM` 读写数据。

### 6.3 `FMC_FPGA.c`: 基础读写函数

文件：`stm/Hardware/FMC_FPGA.c`

写地址：

```c
void FMC_WR_ADDR(volatile uint16_t addr)
{
    FPGA->FPGA_REG = addr;
    __DSB();
    FPGA_opt_delay(300);
}
```

写数据：

```c
void FMC_WR_DATA(volatile uint16_t data)
{
    FPGA->FPGA_RAM = data;
    __DSB();
    FPGA_opt_delay(300);
}
```

读数据：

```c
uint16_t FMC_RD_DATA(void)
{
    volatile uint16_t ram;

    __DSB();
    FPGA_opt_delay(120);
    ram = FPGA->FPGA_RAM;
    __DSB();
    return ram;
}
```

这里的 `__DSB()` 和短延时用于让 FMC 写地址、写数据和读数据之间有足够的总线稳定时间。当前项目为了调试可靠性，选择了偏保守的延时。

### 6.4 `FMC_SelfTest_Display()`: 自检判断

该函数完成三件事：

1. 读取 FPGA 自检寄存器。
2. 判断每一项是否通过。
3. 把结果刷新到 LCD。

读取顺序：

```c
FMC_WR_ADDR(0x0000); sig = FMC_RD_DATA();
FMC_WR_ADDR(0x0001); cst = FMC_RD_DATA();
FMC_WR_ADDR(0x0002); hb50 = FMC_RD_DATA();
FMC_WR_ADDR(0x0004); hb130 = FMC_RD_DATA();
FMC_WR_ADDR(0x0005); stat = FMC_RD_DATA();
```

回环测试：

```c
FMC_WR_ADDR(0x0003);
FMC_WR_DATA(0x55AA);
FMC_WR_ADDR(0x0003);
loop = FMC_RD_DATA();
```

判定条件：

```c
ok_sig  = (sig == 0xA55A);
ok_cst  = (cst == 0x1234);
ok_loop = (loop == 0x55AA);
ok_hb50 = (!hb50_back) && ((now - hb50_last_ms) < HEARTBEAT_TIMEOUT_MS);
ok_pll  = (!hb130_back) && ((now - hb130_last_ms) < HEARTBEAT_TIMEOUT_MS);
ok_stat = ((stat & 0xFFF8U) == 0U) && ((stat & 0x0006U) == 0x0006U);
```

其中：

- `ok_hb50` 要求 HB50 不能回退，并且不能超过 `HEARTBEAT_TIMEOUT_MS` 没变化。
- `ok_pll` 对 HB130 做同样判断。
- `ok_stat` 要求高位无异常，且 `rst_n=1`、`pll_lock=1`。

## 7. LCD UI 与低闪烁刷新机制

LCD UI 主要在 `stm/Hardware/FMC_FPGA.c` 内实现。

### 7.1 自适应布局

`UI_BuildLayout()` 根据 `lcddev.width` 和 `lcddev.height` 自动判断横屏/竖屏：

```c
ui->screen_w = (lcddev.width > 0U) ? lcddev.width : 480U;
ui->screen_h = (lcddev.height > 0U) ? lcddev.height : 800U;
portrait = (ui->screen_h > ui->screen_w);
```

因此当前 UI 可以适配：

- 480 x 800 竖屏
- 800 x 480 横屏

### 7.2 静态内容只画一次

以下内容只在 UI 初始化或分辨率变化时绘制：

- 背景色
- 顶部标题
- 状态区边框
- 表格边框
- 表头 `ITEM / EXPECT / READ / STATE`
- 每行固定标签和期望值

对应函数：

```c
UI_DrawStaticFrame(&ui);
UI_DrawStaticRow(&ui, 0U, "SIGN", "A55A");
UI_DrawStaticRow(&ui, 1U, "CONST", "1234");
...
```

### 7.3 动态内容按变化刷新

动态缓存结构：

```c
typedef struct
{
    uint8_t valid;
    uint8_t ok;
    char read[UI_TEXT_LEN];
} UiRowCache;
```

每一行都缓存上一次显示的读数和 OK/ERR 状态。

`UI_UpdateRowValue()` 会先比较：

```c
read_changed = ...
ok_changed = ...
```

如果读数和状态都没变，直接返回，不访问 LCD。

如果只有读数变化，只擦写 `READ` 数字区域。

如果只有 OK/ERR 变化，只刷新状态块。

这样可以显著减少闪屏，因为 LCD 不再每轮擦整行、整块状态区或全屏。

### 7.4 为什么不显示刷新时间

之前 UI 显示过类似 `Refresh xxx ms` 的实时文本，这会导致状态区每轮都变化，从而触发重绘。现在已经改成固定文案 `FPGA status`，避免无意义刷新。

## 8. 正常 LCD 现象

自检通过时，LCD 应显示：

- 顶部标题：`FMC FPGA LINK MONITOR`
- 大状态块：`LINK PASS`
- 表格中：
  - `SIGN` 读到 `A55A`
  - `CONST` 读到 `1234`
  - `LOOP` 读到 `55AA`
  - `HB50` 数字约每秒递增
  - `HB130` 数字约每秒递增
  - `STAT` 显示类似 `R1 S1 P1` 或至少 `S1 P1`

如果 `LINK FAIL`，优先看哪一行变红：

| 行 | 可能原因 |
|---|---|
| SIGN/CONST 错 | FMC 读路径、地址线、数据线、片选、FPGA 未配置 |
| LOOP 错 | FMC 写路径、RS/WR 信号、FPGA data_write 判断 |
| HB50 错 | FPGA 50 MHz 时钟、内部复位、FMC 域逻辑异常 |
| HB130 错 | PLL 未锁定、130 MHz 时钟未运行、跨域同步异常 |
| STAT 错 | `rst_32`、内部 `rst_n` 或 `pll_lock` 异常 |

## 9. 正式 FIFO 模式说明

当前 `fmc.v` 使用 `SELFTEST_MODE`。

如果注释掉：

```verilog
`define SELFTEST_MODE
```

则 FPGA 会切换到正式 FIFO 采集协议：

| 地址 | 含义 |
|---|---|
| `0x0001` | ready / detect |
| `0x0002` | sample number |
| `0x0003` | sample data |

STM32 侧已有 `FMC_DET_DATA()`，用于按旧协议读取 FIFO 数据：

```c
FMC_WR_ADDR(0x01);
addr1_data = FMC_RD_DATA();
...
FMC_WR_ADDR(0x02);
addr2_data = FMC_RD_DATA();
FMC_WR_ADDR(0x03);
addr3_data = FMC_RD_DATA();
```

建议先确保自检模式长期稳定，再切换回正式 FIFO 模式。这样可以把“总线基础通信问题”和“采集数据路径问题”分开排查。

## 10. 调试建议

### 10.1 固定值错误

先排查：

- STM32 FMC 初始化是否正确。
- FPGA 是否完成下载/配置。
- `CS/RS/WR/RD/SD[15:0]` 引脚约束是否对应。
- STM32 与 FPGA 是否共地。
- FMC 时序是否过快。

### 10.2 HB50 不动

先排查：

- FPGA 50 MHz `clk` 是否输入。
- FPGA 内部 `rst_n` 是否释放。
- `rst_32` 是否长期有效。

### 10.3 HB130 不动

先排查：

- PLL IP 是否正常生成 130 MHz。
- `pll_lock` 是否为 1。
- `CLK_130M` 是否被正确接入 `fmc.v` 的 `CLK_130M_IN`。

### 10.4 LCD 闪屏

当前代码已经采用按变化刷新。如果仍有明显闪屏，优先确认：

- `UI_DrawStaticFrame()` 是否被频繁触发。正常情况下只有首次进入或分辨率变化时触发。
- `lcddev.width/height` 是否在运行中异常变化。
- 是否又加入了实时变化的文字，例如刷新时间、计数时间戳等。

## 11. 构建与验证记录

当前工作区中存在：

- STM32 Keil 工程：`stm/MDK-ARM/Project.uvprojx`
- FPGA 工程：`fpga_pgl12g/prj/project`
- ModelSim/仿真检查目录：`sim_check/`

此前自检相关仿真重点包括：

- FMC 自检寄存器读写。
- 回环寄存器。
- 复位滤波。
- 50 MHz 与 130 MHz 心跳逻辑。

在本机命令行环境中，如果 `UV4/UV5/armclang` 不在 PATH，无法直接通过终端编译 Keil 工程，需要在 Keil MDK 内打开 `stm/MDK-ARM/Project.uvprojx` 编译。

