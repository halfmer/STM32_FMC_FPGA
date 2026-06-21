// fifo.v 的仿真副本：OUT_REG 由宏 OUTREG 控制，用于对比标准模式/FWFT
// 例化名与原始一致，便于层次引用探测 mem。
`timescale 1 ns / 1 ps
`ifndef OUTREG
 `define OUTREG 0
`endif
module fifo
 (
  wr_data, wr_en, wr_clk, wr_rst, full, almost_full,
  rd_data, rd_en, rd_clk, rd_rst, empty, almost_empty
);
    localparam ADDR_WIDTH = 10 ;
    localparam DATA_WIDTH = 16 ;
    localparam OUT_REG = `OUTREG ;          // ★ 关键开关：0=组合(FWFT)  1=寄存(标准)
    localparam RST_TYPE = "ASYNC" ;
    localparam FIFO_TYPE = "ASYNC_FIFO" ;
    localparam ALMOST_FULL_NUM = 1023 ;
    localparam ALMOST_EMPTY_NUM = 4 ;

    input  wire  [DATA_WIDTH-1 : 0]      wr_data ;
    input  wire                          wr_en   ;
    input  wire                          wr_clk  ;
    input  wire                          wr_rst  ;
    output wire                          full    ;
    output wire                          almost_full ;
    output wire  [DATA_WIDTH-1 : 0]      rd_data ;
    input  wire                          rd_en   ;
    input  wire                          rd_clk  ;
    input  wire                          rd_rst  ;
    output wire                          empty   ;
    output wire                          almost_empty ;

ipm_distributed_fifo_v1_4_fifo
 #(
  .ADDR_WIDTH(ADDR_WIDTH), .DATA_WIDTH(DATA_WIDTH), .OUT_REG(OUT_REG),
  .RST_TYPE(RST_TYPE), .FIFO_TYPE(FIFO_TYPE),
  .ALMOST_FULL_NUM(ALMOST_FULL_NUM), .ALMOST_EMPTY_NUM(ALMOST_EMPTY_NUM)
)u_ipm_distributed_fifo_fifo
 (
  .wr_data(wr_data), .wr_en(wr_en), .wr_clk(wr_clk), .wr_rst(wr_rst),
  .full(full), .almost_full(almost_full),
  .rd_data(rd_data), .rd_en(rd_en), .rd_clk(rd_clk), .rd_rst(rd_rst),
  .empty(empty), .almost_empty(almost_empty)
);
endmodule
