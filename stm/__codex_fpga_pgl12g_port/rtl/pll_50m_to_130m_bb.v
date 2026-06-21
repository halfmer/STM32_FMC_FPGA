// Black-box wrapper for the Pango PLL.
//
// Generate a Pango PLL IP with this exact module name and port list:
//   input  clk_in   : 50 MHz
//   input  reset    : active high
//   output clk_out  : 130 MHz
//   output locked   : high after PLL lock
//
// Keep this file while importing/checking hierarchy. Remove or exclude it once
// the generated Pango PLL IP with the same module name is added to the project.

module pll_50m_to_130m (
    input  wire clk_in,
    input  wire reset,
    output wire clk_out,
    output wire locked
);
// synthesis syn_black_box
endmodule
