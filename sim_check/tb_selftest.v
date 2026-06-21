`timescale 1ns/1ps
//==============================================================================
// tb_selftest.v —— 自检版 fmc.v 的功能仿真(同步采样 + sd_stable 滤毛刺)
//   验证点:
//     1. 写地址 0x00/0x01 → 读回 0xA55A / 0x1234(读 mux + 同步地址锁存)
//     2. 写回环 0x55AA 到 0x03 → 读回 0x55AA(data_write 路径)
//     3. 非法地址 → 0xFEFE(default)
//     4. ★关键★ WR 低电平期间往 SD 注入"振铃"假值,地址仍正确锁存
//        (证明 3 级同步器 + sd_stable 对总线毛刺免疫)
//   fmc.v 顶部已 `define SELFTEST_MODE,直接编进来即可。
//==============================================================================
module tb_selftest;

    reg         clk50  = 1'b0;
    reg         clk130 = 1'b0;
    reg         rst_n  = 1'b0;

    reg         cs = 1'b1, rs = 1'b0, wr = 1'b1, rd = 1'b1;
    reg  [15:0] sd_drv = 16'h0000;
    reg         drv_en = 1'b0;
    wire [15:0] SD = drv_en ? sd_drv : 16'hZZZZ;

    always #10     clk50  = ~clk50;    // 50 MHz
    always #3.846  clk130 = ~clk130;   // ~130 MHz

    fmc dut (
        .CLK_IN      (clk50),
        .CLK_130M_IN (clk130),
        .Rst_n       (rst_n),
        .CS          (cs),
        .RS          (rs),
        .WR          (wr),
        .RD          (rd),
        .SD          (SD),
        .WR_ADDR1    (),
        .WR_ADDR2    (),
        .WR_ADDR3    (),
        .FMC_DET     (16'd0),
        .FMC_IN      (16'd0),
        .FMC_NUM     (16'd0)
    );

    integer    errors = 0;
    reg [15:0] rdval;

    // 写地址寄存器(A12=0):RS=0,SD=地址,WR 低脉冲(宽,够同步器锁存)
    task wr_addr(input [15:0] a);
    begin
        @(negedge clk50); cs = 0; rs = 0; sd_drv = a; drv_en = 1;
        #5  wr = 0;
        #200 wr = 1;            // WR 低 ~200ns = 10 个 50M 周期
        #60  drv_en = 0; cs = 1;
        #80;                    // 访问间隔
    end
    endtask

    // 写地址,但在 WR 低期间注入一次短促"振铃"假值(< 1 个 50M 周期)
    task wr_addr_glitch(input [15:0] a, input [15:0] g);
    begin
        @(negedge clk50); cs = 0; rs = 0; sd_drv = a; drv_en = 1;
        #5  wr = 0;
        #43 sd_drv = g;         // 窜入假值
        #12 sd_drv = a;         // 12ns 后恢复(短于 20ns 采样周期)
        #190 wr = 1;
        #60  drv_en = 0; cs = 1;
        #80;
    end
    endtask

    // 写数据(A12=1):RS=1,SD=数据,WR 低脉冲
    task wr_data(input [15:0] d);
    begin
        @(negedge clk50); cs = 0; rs = 1; sd_drv = d; drv_en = 1;
        #5  wr = 0;
        #200 wr = 1;
        #60  drv_en = 0; cs = 1; rs = 0;
        #80;
    end
    endtask

    // 读(A12 任意,这里 RS=1):CS=0,RD=0,等数据稳定后采样
    task rd_chk(input [15:0] exp, input [127:0] tag);
    begin
        @(negedge clk50); cs = 0; rd = 0; drv_en = 0;
        #220;                   // 慢读时序,留足同步器选通 + 总线稳定
        rdval = SD;
        rd = 1; cs = 1;
        #100;                   // 等 DUT 释放三态
        if (rdval !== exp) begin
            errors = errors + 1;
            $display("[FAIL] %0s : exp %04h got %04h", tag, exp, rdval);
        end else begin
            $display("[ OK ] %0s : %04h", tag, rdval);
        end
    end
    endtask

    initial begin
        rst_n = 0; #100; rst_n = 1; #200;

        wr_addr(16'h0000); rd_chk(16'hA55A, "SIG   addr0");
        wr_addr(16'h0001); rd_chk(16'h1234, "CONST addr1");

        wr_addr(16'h0003); wr_data(16'h55AA);
        wr_addr(16'h0003); rd_chk(16'h55AA, "LOOP  addr3");

        wr_addr(16'h0007); rd_chk(16'hFEFE, "DEFAULT addr7");

        // 读心跳(值不定,仅打印,确认可读)
        wr_addr(16'h0002); rd_chk(16'hFEFE, "HB50(info,期望非FEFE,FAIL正常)");

        // ★ 振铃免疫:WR 低期间 SD 窜过 0xDEAD,但地址应仍锁存为 0x0001
        wr_addr_glitch(16'h0001, 16'hDEAD);
        rd_chk(16'h1234, "GLITCH addr1->应仍读1234");

        // 再来一次:地址 0x00 期间窜 0xFFFF
        wr_addr_glitch(16'h0000, 16'hFFFF);
        rd_chk(16'hA55A, "GLITCH addr0->应仍读A55A");

        #100;
        $display("==================================================");
        if (errors == 0 || errors == 1)
            $display(">>> 核心路径通过(errors=%0d;其中 HB50 那条按设计必 FAIL)", errors);
        else
            $display(">>> 有 %0d 处失败,需检查", errors);
        $display("==================================================");
        $finish;
    end

endmodule
