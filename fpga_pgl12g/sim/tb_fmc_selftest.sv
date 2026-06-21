`timescale 1ns/1ps

module tb_fmc_selftest;
    reg         clk50 = 1'b0;
    reg         clk130 = 1'b0;
    reg         rst_n = 1'b0;
    reg         cs = 1'b1;
    reg         rs = 1'b0;
    reg         wr = 1'b1;
    reg         rd = 1'b1;
    reg         sd_oe = 1'b0;
    reg  [15:0] sd_drv = 16'h0000;
    wire [15:0] sd;
    wire        wr_addr1;
    wire        wr_addr2;
    wire        wr_addr3;

    integer errors = 0;

    assign sd = sd_oe ? sd_drv : 16'hzzzz;

    always #10 clk50 = ~clk50;
    always #4  clk130 = ~clk130;

    fmc dut (
        .CLK_IN      (clk50),
        .CLK_130M_IN (clk130),
        .Rst_n       (rst_n),
        .CS          (cs),
        .RS          (rs),
        .WR          (wr),
        .RD          (rd),
        .SD          (sd),
        .WR_ADDR1    (wr_addr1),
        .WR_ADDR2    (wr_addr2),
        .WR_ADDR3    (wr_addr3),
        .FMC_DET     (16'h00FF),
        .FMC_IN      (16'h5A5A),
        .FMC_NUM     (16'h0123),
        .DEBUG_STAT  (16'h0007)
    );

    task fmc_write_addr(input [15:0] addr);
        begin
            @(negedge clk50);
            sd_oe = 1'b1;
            sd_drv = addr;
            cs = 1'b0;
            rs = 1'b0;
            wr = 1'b0;
            rd = 1'b1;
            repeat (10) @(posedge clk50);
            wr = 1'b1;
            cs = 1'b1;
            sd_oe = 1'b0;
            repeat (4) @(posedge clk50);
        end
    endtask

    task fmc_write_data(input [15:0] data);
        begin
            @(negedge clk50);
            sd_oe = 1'b1;
            sd_drv = data;
            cs = 1'b0;
            rs = 1'b1;
            wr = 1'b0;
            rd = 1'b1;
            repeat (10) @(posedge clk50);
            wr = 1'b1;
            cs = 1'b1;
            sd_oe = 1'b0;
            repeat (4) @(posedge clk50);
        end
    endtask

    task fmc_read_data(output [15:0] data);
        begin
            @(negedge clk50);
            sd_oe = 1'b0;
            cs = 1'b0;
            rs = 1'b1;
            wr = 1'b1;
            rd = 1'b0;
            repeat (10) @(posedge clk50);
            data = sd;
            rd = 1'b1;
            cs = 1'b1;
            repeat (4) @(posedge clk50);
        end
    endtask

    task read_reg(input [15:0] addr, output [15:0] data);
        begin
            fmc_write_addr(addr);
            fmc_read_data(data);
        end
    endtask

    task expect_reg(input [15:0] addr, input [15:0] expected);
        reg [15:0] got;
        begin
            read_reg(addr, got);
            if (got !== expected) begin
                $display("ERROR: addr %04h expected %04h got %04h", addr, expected, got);
                errors = errors + 1;
            end else begin
                $display("PASS : addr %04h got %04h", addr, got);
            end
        end
    endtask

    initial begin
        repeat (5) @(posedge clk50);
        rst_n = 1'b1;
        repeat (10) @(posedge clk50);

        expect_reg(16'h0000, 16'hA55A);
        expect_reg(16'h0001, 16'h1234);
        expect_reg(16'h0005, 16'h0007);

        fmc_write_addr(16'h0003);
        fmc_write_data(16'h55AA);
        expect_reg(16'h0003, 16'h55AA);

        force dut.div50 = 26'd49_999_999;
        @(posedge clk50);
        release dut.div50;
        repeat (4) @(posedge clk50);
        expect_reg(16'h0002, 16'h0001);

        dut.div130 = 27'd129_999_998;
        dut.hb130_tick = 1'b0;
        repeat (10) @(posedge clk130);
        repeat (10) @(posedge clk50);
        expect_reg(16'h0004, 16'h0001);

        if (errors == 0) begin
            $display("TB_FMC_SELFTEST PASS");
        end else begin
            $display("TB_FMC_SELFTEST FAIL errors=%0d", errors);
            $fatal(1);
        end
        $finish;
    end
endmodule
