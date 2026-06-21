/*
 * ADC-to-FMC frame FIFO.
 *
 * The ADC clock domain writes one 1024-sample frame. The STM32 FMC WR domain
 * then drains that frame by issuing an ADDR2 write before each ADDR3 read.
 */
module my_fifo #(
    parameter FIFO_LENGTH = 1024
)(
    input  wire             Sys_clk_in      ,
    input  wire             Sys_clk_out     ,
    input  wire             Sys_rst_n       ,

    input  wire    [15:0]   IN_DATA         ,
    output wire    [15:0]   OUT_DATA        ,

    input  wire             read_over       ,
    input  wire             WR_ADDR1        ,
    input  wire             WR_ADDR2        ,
    output reg              can_read
);

localparam [10:0] WRITE_LAST = FIFO_LENGTH - 1;

wire sys_rst = Sys_rst_n & ~read_over;

reg  [10:0] wr_count;
reg         write_done;

wire        wrfull;
wire        almost_full;
wire        rdempty;
wire        almost_empty;
wire [15:0] out_data_i;

wire wrreq = sys_rst && !write_done && !wrfull;

always @(posedge Sys_clk_in or negedge sys_rst) begin
    if (!sys_rst) begin
        wr_count   <= 11'd0;
        write_done <= 1'b0;
    end else if (wrreq) begin
        if (wr_count == WRITE_LAST) begin
            write_done <= 1'b1;
        end else begin
            write_done <= write_done;
        end
        wr_count <= wr_count + 11'd1;
    end else begin
        wr_count   <= wr_count;
        write_done <= write_done;
    end
end

reg write_done_d1;
reg write_done_d2;

always @(posedge Sys_clk_out or negedge sys_rst) begin
    if (!sys_rst) begin
        write_done_d1 <= 1'b0;
        write_done_d2 <= 1'b0;
        can_read      <= 1'b0;
    end else begin
        write_done_d1 <= write_done;
        write_done_d2 <= write_done_d1;
        if (!can_read) begin
            can_read <= write_done_d2 && !rdempty;
        end else begin
            can_read <= 1'b1;
        end
    end
end

reg [10:0] rd_count;

wire rdreq = sys_rst && can_read && WR_ADDR2 && !rdempty && (rd_count < FIFO_LENGTH);

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

fifo fifo_inst
(
    .wr_data      ( IN_DATA )          ,
    .wr_en        ( wrreq )            ,
    .wr_clk       ( Sys_clk_in )       ,
    .wr_rst       ( ~sys_rst )         ,
    .full         ( wrfull )           ,
    .almost_full  ( almost_full )      ,
    .rd_data      ( out_data_i )       ,
    .rd_en        ( rdreq )            ,
    .rd_clk       ( Sys_clk_out )      ,
    .rd_rst       ( ~sys_rst )         ,
    .empty        ( rdempty )          ,
    .almost_empty ( almost_empty )
);

assign OUT_DATA = out_data_i;

wire unused_wr_addr1 = WR_ADDR1;
wire unused_almost_full = almost_full;
wire unused_almost_empty = almost_empty;

endmodule
