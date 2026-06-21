// STM32 FMC asynchronous 16-bit register interface.
//
// Bus mapping:
//   0x0000: writable control register, currently reserved
//   0x0001: read-only ready flag
//   0x0002: read-only sample index
//   0x0003: read-only sample data

module fmc (
    input  wire        CLK_IN,
    input  wire        Rst_n,

    input  wire        CS,
    input  wire        RS,
    input  wire        WR,
    input  wire        RD,
    inout  wire [15:0] SD,

    output wire        WR_ADDR1,
    output wire        WR_ADDR2,
    output wire        WR_ADDR3,

    input  wire [15:0] FMC_DET,
    input  wire [15:0] FMC_IN,
    input  wire [15:0] FMC_NUM
);

reg [15:0] addr;
reg [15:0] rd_data;

reg [15:0] addr0_data;
reg [15:0] addr1_data;
reg [15:0] addr2_data;
reg [15:0] addr3_data;

// Address phase: STM32 writes the selected register address when RS=0.
always @(posedge WR or negedge Rst_n) begin
    if (!Rst_n) begin
        addr <= 16'd0;
    end else if (!CS && !RS) begin
        addr <= SD;
    end
end

// Legacy event enables. They intentionally decode the previous address value
// at the next WR edge, matching the original STM32 read sequence alignment.
assign WR_ADDR1 = (!CS && (addr == 16'h0001));
assign WR_ADDR2 = (!CS && (addr == 16'h0002));
assign WR_ADDR3 = (!CS && (addr == 16'h0003));

always @(posedge WR or negedge Rst_n) begin
    if (!Rst_n) begin
        addr1_data <= 16'd0;
    end else if (WR_ADDR1) begin
        addr1_data <= FMC_DET;
    end else begin
        addr1_data <= addr1_data;
    end
end

always @(posedge WR or negedge Rst_n) begin
    if (!Rst_n) begin
        addr2_data <= 16'd0;
    end else if (WR_ADDR2) begin
        addr2_data <= FMC_NUM;
    end else begin
        addr2_data <= addr2_data;
    end
end

always @(posedge WR or negedge Rst_n) begin
    if (!Rst_n) begin
        addr3_data <= 16'd0;
    end else if (WR_ADDR3) begin
        addr3_data <= FMC_IN;
    end else begin
        addr3_data <= addr3_data;
    end
end

// Optional write register space. Only address 0 is currently writable.
always @(posedge WR or negedge Rst_n) begin
    if (!Rst_n) begin
        addr0_data <= 16'd0;
    end else if (!CS && RS && (addr == 16'h0000)) begin
        addr0_data <= SD;
    end
end

always @(*) begin
    case (addr)
        16'h0000: rd_data = addr0_data;
        16'h0001: rd_data = addr1_data;
        16'h0002: rd_data = addr2_data;
        16'h0003: rd_data = addr3_data;
        default:  rd_data = 16'hFEFE;
    endcase
end

assign SD = (!CS && !RD) ? rd_data : 16'hZZZZ;

// Prevent some tools from trimming the unused FMC clock input while keeping
// the external port compatible with the original module.
wire unused_clk_in = CLK_IN;

endmodule
