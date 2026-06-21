# PGL12G Porting Notes

This directory is a Pango/PGL12G port of the original Quartus/Cyclone IV
project under `../fpga`. The `sim` directory and Quartus-generated build
outputs are intentionally not copied.

## Kept Behavior

The STM32 FMC protocol is unchanged:

- `0x0001`: ready flag, returns `16'd255` when one 1024-sample frame is ready
- `0x0002`: sample index
- `0x0003`: sample data

The STM32 code can keep using:

- `FMC_WR_ADDR(0x01); FMC_RD_DATA();`
- `FMC_WR_ADDR(0x02); FMC_RD_DATA();`
- `FMC_WR_ADDR(0x03); FMC_RD_DATA();`

## Vendor Changes

- Quartus `altpll` is replaced by `pll_50m_to_130m`.
- Quartus `dcfifo` is replaced by portable RTL in `rtl/fifo.v`.
- The original gated `WR_ADDRx` event clocks are converted to WR-domain enable
  signals. The STM32 address/read protocol and sample alignment are preserved,
  but Pango only has to time one FMC WR clock domain.
- Quartus `.qsf` pin locations are not migrated because PGL12G pins must come
  from the PGL12G board schematic.

## PLL Requirement

Generate a Pango PLL IP with this wrapper interface:

```verilog
module pll_50m_to_130m (
    input  wire clk_in,   // 50 MHz
    input  wire reset,    // active high
    output wire clk_out,  // 130 MHz
    output wire locked
);
```

Exclude `rtl/pll_50m_to_130m_bb.v` after adding the generated Pango PLL IP with
the same module name.

## Timing

The Pango project imports `prj/project/timing.fdc`; `constraints/timing.sdc` is
kept as the same constraint content in SDC-style form.

- `clk_50m`: 20 ns input board clock.
- `clk_130m`: 7.692 ns clock on `pll_inst.clk_out`.
- `fmc_wr`: 20 ns STM32 FMC write/event clock.
- `fmc_wr` is asynchronous to the ADC/PLL clock domain.

After pin assignment, add FMC bus input/output delays from the STM32 FMC timing
configuration and the board trace budget.
