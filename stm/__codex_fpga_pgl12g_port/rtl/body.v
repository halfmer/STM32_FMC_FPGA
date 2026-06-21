// Top level for the PGL12G port.
//
// The STM32 FMC register protocol is kept compatible with the original
// Cyclone IV project:
//   address 0x0001 -> ready flag, 16'd255 when one frame is ready
//   address 0x0002 -> sample index
//   address 0x0003 -> ADC sample data

module body (
    input  wire              clk,        // 50 MHz board clock
    input  wire              rst_32,     // active-high reset from STM32/board

    output wire              DAC_A_CLK,
    output wire              DAC_B_CLK,
    output reg signed [13:0] DAC_A_OUT,
    output reg signed [13:0] DAC_B_OUT,

    output wire              ADC_A_CLK,
    output wire              ADC_B_CLK,
    output wire              ADC_C_CLK,
    output wire              ADC_D_CLK,

    input  wire [15:0]       ADC_A_IN,
    input  wire [15:0]       ADC_B_IN,
    input  wire [15:0]       ADC_C_IN,
    input  wire [15:0]       ADC_D_IN,

    input  wire              CS,         // FMC chip select, active low
    input  wire              RS,         // FMC address/data select: 0 address, 1 data
    input  wire              WR,         // FMC write strobe, active low
    input  wire              RD,         // FMC read strobe, active low
    inout  wire [15:0]       SD,         // FMC data bus

    output wire              CLK_130M
);

wire pll_locked;
wire rst_n;

assign rst_n = rst_32 & pll_locked;

pll_50m_to_130m pll_inst (
    .clk_in  (clk),
    .reset   (~rst_32),
    .clk_out (CLK_130M),
    .locked  (pll_locked)
);

assign DAC_A_CLK = CLK_130M;
assign DAC_B_CLK = CLK_130M;
assign ADC_A_CLK = CLK_130M;
assign ADC_B_CLK = CLK_130M;
assign ADC_C_CLK = CLK_130M;
assign ADC_D_CLK = CLK_130M;

reg [15:0] adc_a_data;
reg [15:0] adc_c_data;
reg [15:0] adc_d_data;

always @(posedge CLK_130M or negedge rst_n) begin
    if (!rst_n) begin
        adc_a_data <= 16'd0;
        adc_c_data <= 16'd0;
        adc_d_data <= 16'd0;
    end else begin
        adc_a_data <= ADC_A_IN;
        adc_c_data <= ADC_C_IN;
        adc_d_data <= ADC_D_IN;
    end
end

// Keep ADC_B_IN intentionally unused for protocol compatibility with the
// existing board interface. Add channel selection here if the STM32 must read B.
wire [15:0] unused_adc_b = ADC_B_IN;

// The original design added 8192 to a 14-bit DAC word. That is equivalent to
// toggling bit 13 modulo 14 bits and avoids width-truncation warnings.
always @(posedge CLK_130M or negedge rst_n) begin
    if (!rst_n) begin
        DAC_A_OUT <= 14'd0;
        DAC_B_OUT <= 14'd0;
    end else begin
        DAC_A_OUT <= adc_c_data[15:2] ^ 14'h2000;
        DAC_B_OUT <= adc_d_data[15:2] ^ 14'h2000;
    end
end

fifo_and_fmc #(
    .ALL_LENGTH (1024)
) fifo_and_fmc_inst (
    .FIFO_IN_CLK (CLK_130M),
    .FMC_CLK     (clk),
    .rst_n       (rst_n),
    .ADC_A_IN    (adc_a_data),
    .CS          (CS),
    .RS          (RS),
    .WR          (WR),
    .RD          (RD),
    .SD          (SD)
);

endmodule
