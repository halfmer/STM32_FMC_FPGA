// Bridge between the 130 MHz ADC sample stream and the STM32 FMC register bus.

module fifo_and_fmc #(
    parameter ALL_LENGTH = 1024
) (
    input  wire        FIFO_IN_CLK,
    input  wire        FMC_CLK,
    input  wire        rst_n,

    input  wire        CS,
    input  wire        RS,
    input  wire        WR,
    input  wire        RD,
    inout  wire [15:0] SD,

    input  wire [15:0] ADC_A_IN
);

localparam [10:0] READ_LIMIT = ALL_LENGTH + 3;

wire        wr_addr1;
wire        wr_addr2;
wire        wr_addr3;

reg  [15:0] fifo_in;
wire [15:0] fifo_out;

reg  [15:0] fmc_in;
reg         read_over;
wire        can_read;
reg         can_read_d;
reg  [10:0] out_cnt;
reg  [10:0] cnt;

wire [15:0] fmc_det = can_read ? 16'd255 : 16'd0;

always @(posedge FIFO_IN_CLK or negedge rst_n) begin
    if (!rst_n) begin
        fifo_in <= 16'd0;
    end else begin
        fifo_in <= ADC_A_IN;
    end
end

my_fifo #(
    .FIFO_LENGTH (ALL_LENGTH)
) my_fifo_inst (
    .Sys_clk_in  (FIFO_IN_CLK),
    .Sys_clk_out (WR),
    .Sys_rst_n   (rst_n),
    .IN_DATA     (fifo_in),
    .OUT_DATA    (fifo_out),
    .read_over   (read_over),
    .WR_ADDR1    (wr_addr1),
    .WR_ADDR3    (wr_addr3),
    .can_read    (can_read)
);

fmc fmc_inst (
    .CLK_IN   (FMC_CLK),
    .Rst_n    (rst_n),
    .CS       (CS),
    .RS       (RS),
    .WR       (WR),
    .RD       (RD),
    .SD       (SD),
    .WR_ADDR1 (wr_addr1),
    .WR_ADDR2 (wr_addr2),
    .WR_ADDR3 (wr_addr3),
    .FMC_DET  (fmc_det),
    .FMC_IN   (fmc_in),
    .FMC_NUM  ({5'd0, cnt})
);

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        can_read_d <= 1'b0;
    end else if (wr_addr3) begin
        can_read_d <= can_read;
    end else begin
        can_read_d <= can_read_d;
    end
end

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        out_cnt <= 11'd0;
        fmc_in  <= 16'd0;
    end else if (!wr_addr3) begin
        out_cnt <= out_cnt;
        fmc_in  <= fmc_in;
    end else if (out_cnt == READ_LIMIT) begin
        out_cnt <= 11'd0;
    end else if (can_read_d) begin
        out_cnt <= out_cnt + 11'd1;
        fmc_in  <= fifo_out;
    end else begin
        out_cnt <= 11'd0;
    end
end

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        read_over <= 1'b0;
    end else if (wr_addr3 && (out_cnt == READ_LIMIT)) begin
        read_over <= 1'b1;
    end else begin
        read_over <= 1'b0;
    end
end

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        cnt <= 11'd0;
    end else if (!wr_addr2) begin
        cnt <= cnt;
    end else if (cnt == READ_LIMIT) begin
        cnt <= 11'd0;
    end else if (can_read) begin
        cnt <= cnt + 11'd1;
    end else begin
        cnt <= 11'd0;
    end
end

endmodule
