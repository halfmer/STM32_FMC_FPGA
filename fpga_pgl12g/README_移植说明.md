# Quartus → Pango 移植说明

STM32-FMC 数据采集 FPGA 工程，由 Intel Quartus（Cyclone IV E）移植到
**紫光同创 Pango Logos PGL12G-6 LPG144**（PDS 2022.2-SP6.1）。

本目录下的 RTL 与时序约束已改写完成。你需要在 PDS 里**生成 2 个 IP** + **加入工程** + **编译验证**。

---

## 一、目录结构

```
fpga_pgl12g/
├── prj/project/project.pds      已有的 PDS 空工程（PGL12G/-6/LPG144）
├── source/                      ★已改写的 RTL（直接加入工程）
│   ├── body.v                   顶层（设为 Top）
│   ├── fifo_and_fmc.v
│   ├── fmc.v
│   └── my_fifo.v
├── ip/                          ★你在 PDS 里生成的 IP 放这里
│   ├── pll/                     PLL IP
│   └── fifo/                    异步 FIFO IP
├── constraint/
│   └── timing.fdc               ★时序约束（直接加入工程）
└── README_移植说明.md           本文件
```

---

## 二、生成 PLL IP（PDS Clock Wizard / IP Compiler）

| 配置项 | 取值 |
|---|---|
| IP 类型 | Logos PLL |
| 输入时钟 clkin1 | **50 MHz** |
| 输出 clkout0 | **130 MHz**，相位 0 |
| 其余输出 clkout1~3 | 关闭 |
| 复位 | 勾选 **Enable Port pll_rst**（高有效） |
| 锁定 | 勾选 **pll_lock** |
| 模块名（建议） | `pll` |
| 输出目录 | `fpga_pgl12g/ip/pll/` |

> 原 Intel altpll 参数：50MHz ×13 ÷5 = 130MHz，仅此一路有效。

---

## 三、生成异步 FIFO IP（PDS IP Compiler）

| 配置项 | 取值 |
|---|---|
| IP 类型 | **异步 FIFO**（独立读/写时钟） |
| 数据宽度 | **16 bit** |
| 深度 | **1024** |
| 读模式 | **标准模式 Standard / Normal**（**不要选 Show-ahead / FWFT**） |
| 复位 | **异步复位，高有效** |
| 输出标志 | **full、empty**（必需；其余水位信号可不勾） |
| 模块名（建议） | `fifo` |
| 输出目录 | `fpga_pgl12g/ip/fifo/` |

> 读模式必须与原 Intel dcfifo 一致（标准模式）：数据 `dout` 在 `rd_en` 有效后**下一拍**出现。
> 若误选 FWFT，会导致 `my_fifo.v` 里 `out_cnt` 数据对齐错位。

---

## 四、★IP 端口校准（重要）★

PDS 各版本生成的 IP 端口名可能与下方略有差异。生成后请打开 IP 的**例化模板**，
核对端口名，必要时修改对应 RTL 的例化连接。

### PLL —— 见 `source/body.v` 的 `u_pll`
| 代码中端口 | 含义 | 若你的 IP 端口名不同 → 改这里 |
|---|---|---|
| `.clkin1(clk)` | 50MHz 输入 | 改成实际输入端口名 |
| `.pll_rst(~rst_n)` | 高有效复位 | 若为低有效，改 `.xxx(rst_n)` |
| `.clkout0(CLK_130M)` | 130MHz 输出 | 改成实际输出端口名 |
| `.pll_lock()` | 锁定（悬空） | 改成实际名 |

### FIFO —— 见 `source/my_fifo.v` 的 `u_fifo`
| 代码中端口 | 含义 | 对应原 dcfifo |
|---|---|---|
| `.a_rst(~sys_rst)` | 异步复位高有效 | aclr；**若 IP 为低有效复位，改接 `sys_rst`** |
| `.wr_clk(Sys_clk_in)` | 写时钟 | wrclk |
| `.wr_en(wrreq)` | 写使能 | wrreq |
| `.din(in_data_i)` | 写数据[15:0] | data |
| `.full(wrfull)` | 写满 | wrfull |
| `.rd_clk(Sys_clk_out)` | 读时钟 | rdclk |
| `.rd_en(rdreq)` | 读使能 | rdreq |
| `.dout(out_data_i)` | 读数据[15:0] | q |
| `.empty(rdempty)` | 读空 | rdempty |

---

## 五、加入 PDS 工程

在 PDS 打开 `prj/project/project.pds`，按 GUI 操作（**不要手改 .pds 文件**）：

1. **Add Source**：加入 `source/` 下 4 个 `.v` + `ip/` 下生成的 IP 文件（`.v`/`.vqs`/qxp 等）
2. **Add Constraint**：加入 `constraint/timing.fdc`
3. **设置顶层**：Top Module = `body`
4. **管脚约束**：由你自行编写（位置 LOC + 电平 IOSTANDARD），单独放一个 `.fdc`
5. 依次运行 Compile → Synthesis → Map → PnR → Gen Bitstream

---

## 六、验证清单

1. **综合**：无 latch / multiple-driver / 位宽错误（规范化后应仅剩可接受 warning）
2. **资源/IO**：Map 后查 IO 利用率 ≤ LPG144 可用 IO（你已确认板子 IO 足够）
3. **时序**：PnR 后看 Timing Report，确认 `set_clock_groups` 生效，系统域(50M/130M)与
   FMC 域之间无虚假违例；130MHz 域内部收敛
4. **功能联调**：STM32 运行 `FMC_DET_DATA()`，轮询地址 0x01 读到 `can_read==255` 后，
   循环读地址 0x02（序号）/ 0x03（数据），确认能取回 1024 点 ADC_A 数据且序号对齐
5. **仿真（可选）**：联合 Modelsim 时，testbench 需例化 `GTP_GRS`（`.GRS_N(1'b1)`），
   否则报 `GRS_INST` 未解析

---

## 七、改动摘要（相对原 Quartus 代码）

- `body.v`：altpll → Pango PLL IP 例化；**删除 ADC_B 死逻辑**（原 ADC_B_IN 采样后从未使用，
  仅保留 ADC_B_CLK 时钟输出）；常数补位宽；删未用的 CLK_10M
- `my_fifo.v`：dcfifo → Pango 异步 FIFO IP 例化；删未用的 wrempty/rdfull/wrusedw/rdusedw；
  内部读写/CDC 逻辑不变
- `fmc.v`：组合 `always@(*)` 的 `<=` 改 `=`；三态总线、门控时钟、寄存器逻辑不变
- `fifo_and_fmc.v`：计数器常数位宽对齐（`12'd`→`11'd`）、比较常数显式化；逻辑不变
- `timing.fdc`：新增，定义 50M/130M/FMC 时钟与异步分组

## 八、已知风险

- **门控时钟**：`WR / WR_ADDR1~3` 用信号沿当时钟，原样保留。若 PnR 时钟资源紧张或
  时钟偏斜超标，需后续改为时钟使能(CE)结构（功能等价重构，本次未做）。
- **`.fdc` 对象语法 / PLL 时钟对象路径**：依赖 PDS 版本，建议用 UCE 工具拾取校准。
- **IO 预算**：已按"全保留"实现；若 Map 后实际 IO 超限，回退裁剪一对 ADC→DAC 转发即可。
