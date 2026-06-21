/*
 * STM32 FMC 接口 —— 同步采样版(抗杜邦飞线振铃)。
 *
 * 关键:不再用 posedge WR 作时钟(飞线上 WR 一振铃就有多个假边沿→地址锁存乱)。
 *       改用 CLK_IN(50MHz)对 CS/RS/WR/RD/SD 打拍同步,在写脉冲(WR 低)
 *       期间“电平捕获”地址/数据 —— 对 WR/CS/RS 的边沿毛刺免疫。
 *       读出仍组合驱动(STM32 用很慢的时序,采样点在总线稳定之后)。
 *
 * 顶部 `define SELFTEST_MODE 切换:
 *   ★ 定义   = 上板自检寄存器(0x00=A55A 签名, 0x01=1234 常量,
 *              0x02=50M心跳, 0x03=回环, 0x04=130M(PLL)心跳, 其它=FEFE)
 *   ☆ 注释掉 = 正式 FIFO 采集协议(0x01 ready / 0x02 num / 0x03 data)
 */

`define SELFTEST_MODE      // ★ 注释本行 = 恢复正式 FIFO 协议版

module fmc
(
    input  wire         CLK_IN      ,   // 50MHz  (FMC_CLK)
    input  wire         CLK_130M_IN ,   // 130MHz (PLL 输出,自检 PLL 心跳用)
    input  wire         Rst_n       ,

    input  wire         CS          ,
    input  wire         RS          ,
    input  wire         WR          ,
    input  wire         RD          ,

    inout  wire  [15:0] SD          ,

    output wire         WR_ADDR1    ,
    output wire         WR_ADDR2    ,
    output wire         WR_ADDR3    ,

    input  wire  [15:0] FMC_DET     ,
    input  wire  [15:0] FMC_IN      ,
    input  wire  [15:0] FMC_NUM     ,
    input  wire  [15:0] DEBUG_STAT
);

//============================================================
// 同步器:把异步 FMC 信号打 3 拍到 CLK_IN 域(抗亚稳态/毛刺)
//============================================================
reg  [2:0]  cs_r, rs_r, wr_r, rd_r;
reg  [15:0] sd_r0, sd_r1;
always @(posedge CLK_IN or negedge Rst_n) begin
    if (!Rst_n) begin
        cs_r <= 3'b111; rs_r <= 3'b000; wr_r <= 3'b111; rd_r <= 3'b111;
        sd_r0 <= 16'd0; sd_r1 <= 16'd0;
    end else begin
        cs_r <= {cs_r[1:0], CS};
        rs_r <= {rs_r[1:0], RS};
        wr_r <= {wr_r[1:0], WR};
        rd_r <= {rd_r[1:0], RD};
        sd_r0 <= SD;
        sd_r1 <= sd_r0;
    end
end
wire        cs_s = cs_r[2];
wire        rs_s = rs_r[2];
wire        wr_s = wr_r[2];
wire        rd_s = rd_r[2];
wire [15:0] sd_s = sd_r1;

// Capture address/data while the synchronized FMC write strobe is active.
wire        sd_stable  = (sd_r0 == sd_r1);
wire        addr_write = (cs_s == 1'b0) && (rs_s == 1'b0) && (wr_s == 1'b0);
wire        data_write = (cs_s == 1'b0) && (rs_s == 1'b1) && (wr_s == 1'b0);

reg [15:0] ADDR;
always @(posedge CLK_IN or negedge Rst_n) begin
    if (!Rst_n)                          ADDR <= 16'd0;
    else if (addr_write && sd_stable)    ADDR <= sd_s;
end

// 给 fifo 连接用的译码(同步版用不到,保留以兼容连线,无害)
assign WR_ADDR1 = addr_write && (sd_s == 16'h0001);
assign WR_ADDR2 = addr_write && (sd_s == 16'h0002);
assign WR_ADDR3 = addr_write && (sd_s == 16'h0003);

reg [15:0] RD_DATA;

`ifdef SELFTEST_MODE
//================= 上板自检版(同步) =================
reg [15:0] loopreg;
reg [15:0] hb50;   reg [25:0] div50;
reg [15:0] hb130;  reg [26:0] div130;
reg        hb130_tick;
reg [2:0]  hb130_tick_sync;

// 回环寄存器:数据写(RS=1)且当前地址=0x03
always @(posedge CLK_IN or negedge Rst_n) begin
    if (!Rst_n)                                                loopreg <= 16'd0;
    else if (data_write && sd_stable && (ADDR == 16'h0003))    loopreg <= sd_s;
end

// 50MHz 域心跳(每 50,000,000 拍 +1 -> ~1Hz)
always @(posedge CLK_IN or negedge Rst_n) begin
    if (!Rst_n) begin div50 <= 26'd0; hb50 <= 16'd0; end
    else if (div50 >= 26'd49_999_999) begin div50 <= 26'd0; hb50 <= hb50 + 16'd1; end
    else div50 <= div50 + 26'd1;
end

// 130MHz(PLL)域心跳(每 130,000,000 拍 +1 -> ~1Hz)
always @(posedge CLK_130M_IN or negedge Rst_n) begin
    if (!Rst_n) begin div130 <= 27'd0; hb130_tick <= 1'b0; end
    else if (div130 >= 27'd129_999_999) begin div130 <= 27'd0; hb130_tick <= ~hb130_tick; end
    else div130 <= div130 + 27'd1;
end

// Bring the 130MHz heartbeat event into the 50MHz FMC readout domain.
always @(posedge CLK_IN or negedge Rst_n) begin
    if (!Rst_n) begin
        hb130_tick_sync <= 3'd0;
        hb130 <= 16'd0;
    end else begin
        hb130_tick_sync <= {hb130_tick_sync[1:0], hb130_tick};
        if (hb130_tick_sync[2] ^ hb130_tick_sync[1])
            hb130 <= hb130 + 16'd1;
    end
end

always @(*) begin
    case (ADDR)
        16'h0000: RD_DATA = 16'hA55A;
        16'h0001: RD_DATA = 16'h1234;
        16'h0002: RD_DATA = hb50;
        16'h0003: RD_DATA = loopreg;
        16'h0004: RD_DATA = hb130;
        16'h0005: RD_DATA = DEBUG_STAT;
        default:  RD_DATA = 16'hFEFE;
    endcase
end

`else
//================= 正式 FIFO 采集协议版(同步快照) =================
reg [15:0] ADDR0_DATA, ADDR1_DATA, ADDR2_DATA, ADDR3_DATA;

// 写地址 0x01/0x02/0x03 时,快照 FIFO 的 ready/num/data
always @(posedge CLK_IN or negedge Rst_n) begin
    if (!Rst_n) begin
        ADDR1_DATA <= 16'd0; ADDR2_DATA <= 16'd0; ADDR3_DATA <= 16'd0;
    end else if (addr_write && sd_stable) begin
        if (sd_s == 16'h0001) ADDR1_DATA <= FMC_DET;
        if (sd_s == 16'h0002) ADDR2_DATA <= FMC_NUM;
        if (sd_s == 16'h0003) ADDR3_DATA <= FMC_IN;
    end
end

always @(posedge CLK_IN or negedge Rst_n) begin
    if (!Rst_n)                                                ADDR0_DATA <= 16'd0;
    else if (data_write && sd_stable && (ADDR == 16'h0000))    ADDR0_DATA <= sd_s;
end

always @(*) begin
    case (ADDR)
        16'h0000: RD_DATA = ADDR0_DATA;
        16'h0001: RD_DATA = ADDR1_DATA;
        16'h0002: RD_DATA = ADDR2_DATA;
        16'h0003: RD_DATA = ADDR3_DATA;
        default:  RD_DATA = 16'hFEFE;
    endcase
end
`endif

// 读出三态驱动:用同步后的 cs/rd 选通(慢时序下其延迟无关紧要,且避免写期间误驱动)
assign SD = ((cs_s == 1'b0) && (rd_s == 1'b0)) ? RD_DATA : 16'hZZZZ;

endmodule
