# Migration Check List

## RTL

- [x] `body` is the top-level module.
- [x] `fifo.v` is included before `my_fifo.v`.
- [ ] Pango PLL IP is generated as `pll_50m_to_130m`.
- [ ] `pll_50m_to_130m_bb.v` is excluded after real PLL IP is added.
- [x] `FMC_NUM` is explicitly zero-extended to 16 bits.
- [x] Constants use explicit widths.
- [x] FMC register events use WR-domain enables instead of gated clocks.

## Pango/PGL12G

- [x] Project device is `PGL12G`.
- [ ] Package and speed grade match the physical board.
- [ ] All top-level ports are assigned to package pins.
- [ ] IO standards match STM32/ADC/DAC electrical levels.
- [x] Pango compile/synthesis runs with the temporary PLL black box.

## Timing

- [x] `clk_50m` is constrained to 20 ns.
- [x] PLL output is constrained to 130 MHz.
- [ ] FMC bus delays are added after STM32 FMC timing is finalized.
- [x] Clock groups between FMC WR clock and ADC clock are reviewed.

## Bring-Up

- [ ] STM32 can read `0x0001 == 255`.
- [ ] STM32 reads sample index from `0x0002`.
- [ ] STM32 reads sample data from `0x0003`.
- [ ] One frame contains 1024 valid samples.
