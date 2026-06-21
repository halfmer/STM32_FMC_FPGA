// Portable 16-bit by 1024-word asynchronous FIFO.
//
// This module keeps the same port names as the Quartus dcfifo-generated
// module used in the original project, but it is written as vendor-neutral RTL
// for the PGL12G port.

module fifo (
    input  wire        aclr,
    input  wire [15:0] data,
    input  wire        rdclk,
    input  wire        rdreq,
    input  wire        wrclk,
    input  wire        wrreq,
    output reg  [15:0] q,
    output wire        rdempty,
    output wire        rdfull,
    output wire [9:0]  rdusedw,
    output wire        wrempty,
    output wire        wrfull,
    output wire [9:0]  wrusedw
);

localparam ADDR_WIDTH = 10;
localparam DEPTH      = 1024;
localparam [ADDR_WIDTH:0] DEPTH_WORDS = 11'd1024;

reg [15:0] mem [0:DEPTH-1];

reg [ADDR_WIDTH:0] wr_bin;
reg [ADDR_WIDTH:0] wr_gray;
reg [ADDR_WIDTH:0] rd_bin;
reg [ADDR_WIDTH:0] rd_gray;

reg [ADDR_WIDTH:0] rd_gray_wr1;
reg [ADDR_WIDTH:0] rd_gray_wr2;
reg [ADDR_WIDTH:0] wr_gray_rd1;
reg [ADDR_WIDTH:0] wr_gray_rd2;

reg wrfull_reg;
reg rdempty_reg;

wire [ADDR_WIDTH:0] wr_bin_next;
wire [ADDR_WIDTH:0] rd_bin_next;
wire [ADDR_WIDTH:0] wr_gray_next;
wire [ADDR_WIDTH:0] rd_gray_next;
wire wrfull_next;
wire rdempty_next;

wire wr_allow = wrreq && !wrfull_reg;
wire rd_allow = rdreq && !rdempty_reg;

assign wr_bin_next  = wr_bin + {{ADDR_WIDTH{1'b0}}, wr_allow};
assign rd_bin_next  = rd_bin + {{ADDR_WIDTH{1'b0}}, rd_allow};
assign wr_gray_next = (wr_bin_next >> 1) ^ wr_bin_next;
assign rd_gray_next = (rd_bin_next >> 1) ^ rd_bin_next;

function [ADDR_WIDTH:0] gray_to_bin;
    input [ADDR_WIDTH:0] gray;
    integer i;
    begin
        gray_to_bin[ADDR_WIDTH] = gray[ADDR_WIDTH];
        for (i = ADDR_WIDTH - 1; i >= 0; i = i - 1) begin
            gray_to_bin[i] = gray_to_bin[i + 1] ^ gray[i];
        end
    end
endfunction

wire [ADDR_WIDTH:0] rd_bin_sync_wr = gray_to_bin(rd_gray_wr2);
wire [ADDR_WIDTH:0] wr_bin_sync_rd = gray_to_bin(wr_gray_rd2);
wire [ADDR_WIDTH:0] wr_used_full = wr_bin - rd_bin_sync_wr;
wire [ADDR_WIDTH:0] rd_used_full = wr_bin_sync_rd - rd_bin;

assign wrfull_next = (wr_gray_next == {~rd_gray_wr2[ADDR_WIDTH:ADDR_WIDTH-1],
                                       rd_gray_wr2[ADDR_WIDTH-2:0]});
assign rdempty_next = (rd_gray_next == wr_gray_rd2);

assign wrfull = wrfull_reg;
assign rdempty = rdempty_reg;
assign wrusedw = wr_used_full[ADDR_WIDTH-1:0];
assign rdusedw = rd_used_full[ADDR_WIDTH-1:0];
assign wrempty = (wr_bin == rd_bin_sync_wr);
assign rdfull  = (rd_used_full == DEPTH_WORDS);

always @(posedge wrclk or posedge aclr) begin
    if (aclr) begin
        wr_bin      <= {(ADDR_WIDTH+1){1'b0}};
        wr_gray     <= {(ADDR_WIDTH+1){1'b0}};
        rd_gray_wr1 <= {(ADDR_WIDTH+1){1'b0}};
        rd_gray_wr2 <= {(ADDR_WIDTH+1){1'b0}};
        wrfull_reg  <= 1'b0;
    end else begin
        rd_gray_wr1 <= rd_gray;
        rd_gray_wr2 <= rd_gray_wr1;
        if (wr_allow) begin
            mem[wr_bin[ADDR_WIDTH-1:0]] <= data;
        end
        wr_bin  <= wr_bin_next;
        wr_gray <= wr_gray_next;
        wrfull_reg <= wrfull_next;
    end
end

always @(posedge rdclk or posedge aclr) begin
    if (aclr) begin
        rd_bin      <= {(ADDR_WIDTH+1){1'b0}};
        rd_gray     <= {(ADDR_WIDTH+1){1'b0}};
        wr_gray_rd1 <= {(ADDR_WIDTH+1){1'b0}};
        wr_gray_rd2 <= {(ADDR_WIDTH+1){1'b0}};
        q           <= 16'd0;
        rdempty_reg <= 1'b1;
    end else begin
        wr_gray_rd1 <= wr_gray;
        wr_gray_rd2 <= wr_gray_rd1;
        if (rd_allow) begin
            q <= mem[rd_bin[ADDR_WIDTH-1:0]];
        end
        rd_bin  <= rd_bin_next;
        rd_gray <= rd_gray_next;
        rdempty_reg <= rdempty_next;
    end
end

endmodule
