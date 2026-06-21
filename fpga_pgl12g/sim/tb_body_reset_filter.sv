`timescale 1ns/1ps

module pll (
    input  wire clkin1,
    input  wire pll_rst,
    output wire clkout0,
    output wire pll_lock
);
    assign clkout0 = clkin1;
    assign pll_lock = ~pll_rst;
endmodule

module fifo_and_fmc #(
    parameter ALL_LENGTH = 1024
)(
    input  wire             FIFO_IN_CLK,
    input  wire             FMC_CLK,
    input  wire             rst_n,
    input  wire [15:0]      DEBUG_STAT,
    input  wire             CS,
    input  wire             RS,
    input  wire             WR,
    input  wire             RD,
    inout  wire [15:0]      SD,
    input  wire [15:0]      ADC_A_IN
);
endmodule

module tb_body_reset_filter;
    reg         clk = 1'b0;
    reg         rst_32 = 1'b0;
    reg         cs = 1'b1;
    reg         rs = 1'b0;
    reg         wr = 1'b1;
    reg         rd = 1'b1;
    reg  [15:0] adc_a = 16'h0000;
    reg  [15:0] adc_c = 16'h0000;
    reg  [15:0] adc_d = 16'h0000;
    wire [15:0] sd;
    wire        dac_a_clk;
    wire        dac_b_clk;
    wire        adc_a_clk;
    wire        adc_b_clk;
    wire        adc_c_clk;
    wire        adc_d_clk;
    wire signed [13:0] dac_a_out;
    wire signed [13:0] dac_b_out;

    integer errors = 0;

    always #10 clk = ~clk;

    body dut (
        .clk       (clk),
        .DAC_A_CLK (dac_a_clk),
        .DAC_B_CLK (dac_b_clk),
        .DAC_A_OUT (dac_a_out),
        .DAC_B_OUT (dac_b_out),
        .ADC_A_CLK (adc_a_clk),
        .ADC_B_CLK (adc_b_clk),
        .ADC_C_CLK (adc_c_clk),
        .ADC_D_CLK (adc_d_clk),
        .ADC_A_IN  (adc_a),
        .ADC_C_IN  (adc_c),
        .ADC_D_IN  (adc_d),
        .CS        (cs),
        .RS        (rs),
        .WR        (wr),
        .RD        (rd),
        .SD        (sd),
        .rst_32    (rst_32)
    );

    task check_rst(input expected, input [8*40-1:0] label);
        begin
            if (dut.rst_n !== expected) begin
                $display("ERROR: %0s expected rst_n=%0b got %0b", label, expected, dut.rst_n);
                errors = errors + 1;
            end else begin
                $display("PASS : %0s rst_n=%0b", label, dut.rst_n);
            end
        end
    endtask

    initial begin
        repeat (10) @(posedge clk);
        check_rst(1'b0, "initial reset held");

        rst_32 = 1'b1;
        #(21_000_000);
        check_rst(1'b1, "released after stable high");

        rst_32 = 1'b0;
        #(100_000);
        rst_32 = 1'b1;
        #(200_000);
        check_rst(1'b1, "short low pulse ignored");

        rst_32 = 1'b0;
        #(1_200_000);
        check_rst(1'b0, "long low pulse asserted");

        rst_32 = 1'b1;
        #(21_000_000);
        check_rst(1'b1, "released after second stable high");

        if (errors == 0) begin
            $display("TB_BODY_RESET_FILTER PASS");
        end else begin
            $display("TB_BODY_RESET_FILTER FAIL errors=%0d", errors);
            $fatal(1);
        end
        $finish;
    end
endmodule
