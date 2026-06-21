/*
 * FIFO and STM32 FMC bridge.
 *
 * Existing STM32 firmware reads:
 *   ADDR1 -> ready flag 255
 *   ADDR2 -> sample index, valid range 4..1027
 *   ADDR3 -> sample data
 *
 * ADDR2 prefetches the next FIFO word. ADDR3 then latches that prefetched word
 * into the FMC read register.
 */
module fifo_and_fmc #(
    parameter ALL_LENGTH = 1024
)(
    input  wire             FIFO_IN_CLK ,
    input  wire             FMC_CLK     ,
    input  wire             rst_n       ,
    input  wire    [15:0]   DEBUG_STAT  ,

    input  wire             CS          ,
    input  wire             RS          ,
    input  wire             WR          ,
    input  wire             RD          ,

    inout  wire    [15:0]   SD          ,

    input  wire    [15:0]   ADC_A_IN
);

localparam [10:0] FIRST_SAMPLE_NUM = 11'd4;
localparam [10:0] LAST_SAMPLE_NUM  = ALL_LENGTH + 3;
localparam [10:0] INVALID_NUM      = ALL_LENGTH + 4;
localparam [10:0] LAST_READ_COUNT  = ALL_LENGTH - 1;

wire WR_ADDR1;
wire WR_ADDR2;
wire WR_ADDR3;

reg  [15:0] FIFO_IN;
wire [15:0] FIFO_OUT;

reg         read_over;
wire        can_read;
reg         can_read_d;
wire        can_read_rise;

reg  [10:0] read_count;
reg  [10:0] sample_num;

wire [15:0] fmc_det = can_read ? 16'd255 : 16'd0;
wire [15:0] fmc_num = {5'd0, sample_num};

assign can_read_rise = can_read && !can_read_d;

always @(posedge FIFO_IN_CLK or negedge rst_n) begin
    if (!rst_n) begin
        FIFO_IN <= 16'd0;
    end else begin
        FIFO_IN <= ADC_A_IN;
    end
end

my_fifo #(
    .FIFO_LENGTH (ALL_LENGTH)
) my_fifo_inst (
    .Sys_clk_in  (FIFO_IN_CLK),
    .Sys_clk_out (WR),
    .Sys_rst_n   (rst_n),
    .IN_DATA     (FIFO_IN),
    .OUT_DATA    (FIFO_OUT),
    .read_over   (read_over),
    .WR_ADDR1    (WR_ADDR1),
    .WR_ADDR2    (WR_ADDR2),
    .can_read    (can_read)
);

fmc fmc_inst (
    .CLK_IN      (FMC_CLK),
    .CLK_130M_IN (FIFO_IN_CLK),
    .Rst_n    (rst_n),
    .CS       (CS),
    .RS       (RS),
    .WR       (WR),
    .RD       (RD),
    .SD       (SD),
    .WR_ADDR1 (WR_ADDR1),
    .WR_ADDR2 (WR_ADDR2),
    .WR_ADDR3 (WR_ADDR3),
    .FMC_DET  (fmc_det),
    .FMC_IN   (FIFO_OUT),
    .FMC_NUM  (fmc_num),
    .DEBUG_STAT(DEBUG_STAT)
);

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        can_read_d <= 1'b0;
    end else begin
        can_read_d <= can_read;
    end
end

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        sample_num <= 11'd0;
    end else if (can_read_rise) begin
        sample_num <= FIRST_SAMPLE_NUM;
    end else if (!can_read) begin
        sample_num <= 11'd0;
    end else if (WR_ADDR2 && (sample_num < LAST_SAMPLE_NUM)) begin
        sample_num <= sample_num + 11'd1;
    end else if (WR_ADDR2 && (sample_num == LAST_SAMPLE_NUM)) begin
        sample_num <= INVALID_NUM;
    end else begin
        sample_num <= sample_num;
    end
end

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        read_count <= 11'd0;
    end else if (can_read_rise) begin
        read_count <= 11'd0;
    end else if (!can_read) begin
        read_count <= 11'd0;
    end else if (WR_ADDR3 && (read_count < ALL_LENGTH)) begin
        read_count <= read_count + 11'd1;
    end else begin
        read_count <= read_count;
    end
end

always @(posedge WR or negedge rst_n) begin
    if (!rst_n) begin
        read_over <= 1'b0;
    end else if (WR_ADDR3 && can_read && (read_count == LAST_READ_COUNT)) begin
        read_over <= 1'b1;
    end else begin
        read_over <= 1'b0;
    end
end

endmodule
