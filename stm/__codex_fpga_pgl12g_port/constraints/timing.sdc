# PGL12G timing constraints for the STM32-FMC bridge design.
#
# Pin constraints are intentionally not included here. Add board pins in PDS
# after confirming the PGL12G package and board schematic.

create_clock -name clk_50m -period 20.000 -waveform {0.000 10.000} [get_ports clk]
create_clock -name fmc_wr -period 20.000 -waveform {0.000 10.000} [get_ports WR]
create_clock -name clk_130m -period 7.692 -waveform {0.000 3.846} [get_pins pll_inst.clk_out]

# The Pango PLL IP must keep wrapper instance/port names:
#   body.pll_inst.clk_in  -> 50 MHz
#   body.pll_inst.clk_out -> 130 MHz
# The 130 MHz domain is constrained directly on the PLL output pin so timing
# remains valid while the PLL is still a black box. If the generated Pango PLL
# emits its own generated clock later, remove or reconcile the explicit
# clk_130m create_clock above.

# The FMC WR domain and ADC/PLL clock domain are asynchronous to each other.
# The design uses an async FIFO for the ADC data crossing. Register-control
# signals from FMC are WR-domain enables, not separate clocks.
set_clock_groups -asynchronous \
    -group [get_clocks clk_50m] \
    -group [get_clocks fmc_wr]
set_clock_groups -asynchronous \
    -group [get_clocks clk_130m] \
    -group [get_clocks fmc_wr]

# FMC data/control input timing must be finalized after board pin assignment and
# STM32 FMC timing are fixed. Start with the STM32 FMC write/read waveform and
# add set_input_delay/set_output_delay for CS/RS/WR/RD/SD as needed.
