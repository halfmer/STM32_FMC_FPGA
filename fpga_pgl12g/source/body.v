/*
 * body.v 鈥斺€?椤跺眰妯″潡锛堢传鍏夊悓鍒?Pango Logos PGL12G-6 LPG144 绉绘鐗堬級
 *
 * 鐢?Intel Quartus(Cyclone IV E) 宸ョ▼绉绘鑰屾潵銆傛敼鍔ㄧ偣锛? *   1. PLL 鐢?Intel altpll 鏀逛负 Pango Logos PLL IP锛圥DS Clock Wizard 鐢熸垚锛?0MHz鈫?30MHz锛? *   2. 鍒犻櫎 ADC_B 姝婚€昏緫锛堝師 ADC_B_IN 閲囨牱鍚庝粠鏈弬涓庝换浣曡緭鍑猴級锛屼粎淇濈暀 ADC_B_CLK 鏃堕挓杈撳嚭
 *   3. 甯告暟琛ヤ綅瀹姐€佸垹闄ゆ湭浣跨敤鐨?CLK_10M锛屽叾浣欏姛鑳介€昏緫淇濇寔涓嶅彉
 */
module body(
    input       wire                        clk         ,    //50Mhz
    //input       wire                        rst_n       ,

    output      wire                        DAC_A_CLK   ,
    output      wire                        DAC_B_CLK   ,

    output      reg     signed   [13:0]     DAC_A_OUT   ,
    output      reg     signed   [13:0]     DAC_B_OUT   ,

    output      wire                        ADC_A_CLK   ,
    output      wire                        ADC_B_CLK   ,
    output      wire                        ADC_C_CLK   ,
    output      wire                        ADC_D_CLK   ,

    input       wire    [15:0]              ADC_A_IN    ,
    input       wire    [15:0]              ADC_C_IN    ,
    input       wire    [15:0]              ADC_D_IN    ,

    input       wire                        CS          ,   // 鐗囬€変俊鍙?浣庣數骞虫湁鏁?
    input       wire                        RS          ,   // 鍦板潃/鏁版嵁鏍囧織(0: 鍦板潃   1锛?鏁版嵁)
    input       wire                        WR          ,   // 鍐欎娇鑳戒俊鍙?浣庣數骞虫湁鏁?
    input       wire                        RD          ,   // 璇讳娇鑳戒俊鍙?浣庣數骞虫湁鏁?

    inout       wire    [15:0]              SD          ,   // FMC鏁版嵁鎬荤嚎

    input       wire                        rst_32         //32浼犻€掔殑澶嶄綅淇″彿
);

reg  [2:0]  rst32_sync   = 3'b000;
reg  [19:0] rst_high_cnt = 20'd0;
reg  [15:0] rst_low_cnt  = 16'd0;
reg         rst_n        = 1'b0;
wire CLK_130M;
wire pll_lock;

localparam [19:0] RST_RELEASE_CNT = 20'd999_999;  // 20 ms @ 50 MHz
localparam [15:0] RST_ASSERT_CNT  = 16'd49_999;   // 1 ms @ 50 MHz

// Filter STM32 NRST before it becomes the FPGA internal reset. Short low
// glitches on NRST must not clear the FMC heartbeat counters.
always @(posedge clk) begin
    rst32_sync <= {rst32_sync[1:0], rst_32};

    if (!rst32_sync[2]) begin
        rst_high_cnt <= 20'd0;
        if (rst_low_cnt < RST_ASSERT_CNT) begin
            rst_low_cnt <= rst_low_cnt + 16'd1;
        end else begin
            rst_n <= 1'b0;
        end
    end else begin
        rst_low_cnt <= 16'd0;
        if (!rst_n) begin
            if (rst_high_cnt < RST_RELEASE_CNT) begin
                rst_high_cnt <= rst_high_cnt + 20'd1;
            end else begin
                rst_n <= 1'b1;
            end
        end else begin
            rst_high_cnt <= RST_RELEASE_CNT;
        end
    end
end

//============================================================
// PLL: 50 MHz input to 130 MHz output.
// pll_rst is active high; pll_lock is exported to self-test status.
//============================================================
pll u_pll
(
    .clkin1   (clk)       ,
    .pll_rst  (~rst_n)    ,
    .clkout0  (CLK_130M)  ,
    .pll_lock (pll_lock)

);

assign  DAC_A_CLK = CLK_130M;
assign  DAC_B_CLK = CLK_130M;
assign  ADC_A_CLK = CLK_130M;
assign  ADC_B_CLK = CLK_130M;   // ADC_B 鏁版嵁鏈噰闆嗭紝浠呬繚鐣欐椂閽熻緭鍑轰緵鏉夸笂 ADC_B 鑺墖
assign  ADC_C_CLK = CLK_130M;
assign  ADC_D_CLK = CLK_130M;

reg      [15:0]   ADC_A_DATA;
reg      [15:0]   ADC_C_DATA;
reg      [15:0]   ADC_D_DATA;

always @(posedge CLK_130M or negedge rst_n)
begin
	if(!rst_n)begin
		ADC_A_DATA <= 16'd0;
	end else begin
		ADC_A_DATA <= ADC_A_IN;
	end
end

always @(posedge CLK_130M or negedge rst_n)
begin
	if(!rst_n)begin
		ADC_C_DATA <= 16'd0;
	end else begin
		ADC_C_DATA <= ADC_C_IN;
	end
end

always @(posedge CLK_130M or negedge rst_n)
begin
	if(!rst_n)begin
		ADC_D_DATA <= 16'd0;
	end else begin
		ADC_D_DATA <= ADC_D_IN;
	end
end


always @(posedge CLK_130M or negedge rst_n)
begin
	if(!rst_n)begin
		DAC_A_OUT <= 14'd0;
	end else begin
		DAC_A_OUT <= ADC_C_DATA[15:2]+14'd8192;
	end
end

always @(posedge CLK_130M or negedge rst_n)
begin
	if(!rst_n)begin
		DAC_B_OUT <= 14'd0;
	end else begin
		DAC_B_OUT <= ADC_D_DATA[15:2]+14'd8192;
	end
end

fifo_and_fmc #(
    .ALL_LENGTH (1024)
)fifo_and_fmc_inst
(
    .FIFO_IN_CLK(CLK_130M)      ,
    .FMC_CLK    (clk)           ,
    .rst_n      (rst_n)         ,
    .DEBUG_STAT ({13'd0, pll_lock, rst_n, rst32_sync[2]}) ,

    .ADC_A_IN   (ADC_A_IN)      ,
    .CS         (CS)            ,
    .RS         (RS)            ,
    .WR         (WR)            ,
    .RD         (RD)            ,

    .SD         (SD)

);

endmodule

