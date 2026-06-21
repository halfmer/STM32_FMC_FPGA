// Frame FIFO controller.
//
// Writes one 1024-sample frame from the ADC clock domain, then exposes the
// frame to STM32 reads through FMC WR-domain address events. read_over resets the
// FIFO for the next frame after the STM32 side has drained the data.

module my_fifo #(
    parameter FIFO_LENGTH = 1024
) (
    input  wire        Sys_clk_in,
    input  wire        Sys_clk_out,
    input  wire        Sys_rst_n,

    input  wire [15:0] IN_DATA,
    output reg  [15:0] OUT_DATA,

    input  wire        read_over,
    input  wire        WR_ADDR1,
    input  wire        WR_ADDR3,
    output reg         can_read
);

localparam [10:0] WRITE_LAST = FIFO_LENGTH - 1;
localparam [10:0] READ_LAST  = FIFO_LENGTH + 2;

wire sys_rst = Sys_rst_n & ~read_over;

wire        wrfull;
wire        wrempty;
wire [9:0]  wrusedw;
wire        rdfull;
wire        rdempty;
wire [9:0]  rdusedw;
wire [15:0] out_data_i;

reg  [10:0] wr_count;
reg         write_done;
reg  [10:0] rd_count;

wire wrreq = sys_rst && !write_done && !wrfull;
wire rdreq = sys_rst && can_read && WR_ADDR3 && !rdempty && (rd_count < READ_LAST);

fifo fifo_inst (
    .aclr    (~sys_rst),
    .data    (IN_DATA),
    .rdclk   (Sys_clk_out),
    .rdreq   (rdreq),
    .wrclk   (Sys_clk_in),
    .wrreq   (wrreq),
    .q       (out_data_i),
    .rdempty (rdempty),
    .rdfull  (rdfull),
    .rdusedw (rdusedw),
    .wrempty (wrempty),
    .wrfull  (wrfull),
    .wrusedw (wrusedw)
);

always @(posedge Sys_clk_in or negedge sys_rst) begin
    if (!sys_rst) begin
        wr_count   <= 11'd0;
        write_done <= 1'b0;
    end else if (wrreq) begin
        if (wr_count == WRITE_LAST) begin
            write_done <= 1'b1;
        end
        wr_count <= wr_count + 11'd1;
    end
end

reg write_done_d1;
reg write_done_d2;

always @(posedge Sys_clk_out or negedge sys_rst) begin
    if (!sys_rst) begin
        write_done_d1 <= 1'b0;
        write_done_d2 <= 1'b0;
        can_read      <= 1'b0;
    end else if (WR_ADDR1) begin
        write_done_d1 <= write_done;
        write_done_d2 <= write_done_d1;
        can_read      <= write_done_d2;
    end else begin
        write_done_d1 <= write_done_d1;
        write_done_d2 <= write_done_d2;
        can_read      <= can_read;
    end
end

always @(posedge Sys_clk_out or negedge sys_rst) begin
    if (!sys_rst) begin
        rd_count <= 11'd0;
    end else if (!can_read) begin
        rd_count <= 11'd0;
    end else if (rdreq) begin
        rd_count <= rd_count + 11'd1;
    end else begin
        rd_count <= rd_count;
    end
end

always @(posedge Sys_clk_out or negedge sys_rst) begin
    if (!sys_rst) begin
        OUT_DATA <= 16'd0;
    end else if (rdreq) begin
        OUT_DATA <= out_data_i;
    end else begin
        OUT_DATA <= OUT_DATA;
    end
end

endmodule
