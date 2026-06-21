// tb.v —— 模拟 STM32 FMC 访问，验证 fpga_pgl12g 通信逻辑自洽性
//
// 例化 fifo_and_fmc（核心），接真实紫光 FIFO RTL。ADC_A_IN 用连续递增计数器，
// 这样写入 FIFO 的是已知连续序列，读出后即可检查 序号(num) 与 数据(data) 的对齐。
//
// 输出经 $fdisplay 写入 sim_result.txt（绕过 ModelSim transcript 缓冲）。
`timescale 1 ns / 1 ps

module tb;

  localparam ALL_LENGTH = 16;          // 用小帧长加速仿真；逻辑与 1024 同构

  // ---------- 时钟 / 复位 ----------
  reg clk130 = 1'b0;                   // FIFO 写时钟（对应 130MHz）
  reg fmc_clk = 1'b0;
  reg rst_n  = 1'b0;
  always #3.846 clk130  = ~clk130;     // ~130MHz
  always #10    fmc_clk = ~fmc_clk;

  // ---------- ADC 数据源：连续递增 ----------
  reg [15:0] adc = 16'h1000;
  always @(posedge clk130 or negedge rst_n)
    if (!rst_n) adc <= 16'h1000;
    else        adc <= adc + 16'd1;

  // ---------- FMC 总线建模 ----------
  reg        cs = 1'b1;
  reg        rs = 1'b0;
  reg        wr = 1'b1;                // 写(低有效，上升沿锁存)
  reg        rd = 1'b1;
  reg [15:0] sd_drive = 16'd0;
  reg        sd_oe = 1'b0;
  wire [15:0] SD = sd_oe ? sd_drive : 16'hzzzz;

  // ---------- DUT ----------
  fifo_and_fmc #(.ALL_LENGTH(ALL_LENGTH)) dut (
    .FIFO_IN_CLK (clk130),
    .FMC_CLK     (fmc_clk),
    .rst_n       (rst_n),
    .CS          (cs),
    .RS          (rs),
    .WR          (wr),
    .RD          (rd),
    .SD          (SD),
    .ADC_A_IN    (adc)
  );

  // ---------- STM32 访问 task ----------
  task fmc_wr_addr(input [15:0] val);  // 写地址寄存器：CS=0,RS=0，产生 WR 上升沿
    begin
      cs = 1'b0; rs = 1'b0;
      sd_drive = val; sd_oe = 1'b1;
      #15; wr = 1'b0; #20; wr = 1'b1;
      #15; sd_oe = 1'b0; cs = 1'b1;
      #10;
    end
  endtask

  task fmc_rd_data(output [15:0] data); // 读数据：CS=0,RS=1，RD 拉低采样
    begin
      cs = 1'b0; rs = 1'b1; sd_oe = 1'b0;
      rd = 1'b0; #30;
      data = SD;
      rd = 1'b1; #10; cs = 1'b1;
      #10;
    end
  endtask

  // ---------- 主流程：复刻 FMC_DET_DATA ----------
  integer i, state, got, k, errors, fd;
  reg [15:0] addr1, a2, a3;
  reg [15:0] data_arr [0:ALL_LENGTH-1];
  reg [15:0] num_arr  [0:ALL_LENGTH-1];
  reg [15:0] mem0, mem1;
  reg [15:0] expected [0:ALL_LENGTH-1];   // 镜像捕获：第一帧实际写入 FIFO 的序列
  integer widx = 0;

  // 监控 my_fifo 写端口：每次有效写入记录数据（只取第一帧的前 ALL_LENGTH 个）
  always @(posedge clk130)
    if (rst_n && dut.my_fifo_inst.wrreq && !dut.my_fifo_inst.wrfull && widx < ALL_LENGTH) begin
      expected[widx] = dut.my_fifo_inst.IN_DATA;
      widx = widx + 1;
    end

  initial begin
    fd = $fopen("sim_result.txt", "w");
    $fdisplay(fd, "=== TB START (real PDS fifo IP, OUT_REG=1)  ALL_LENGTH=%0d ===", ALL_LENGTH);

    rst_n = 1'b0;
    #103;
    rst_n = 1'b1;
    #800;                                // 等 FIFO 在 130M 域写满一帧

    mem0 = dut.my_fifo_inst.fifo_inst.u_ipm_distributed_fifo_fifo.ipm_distributed_sdpram_fifo.mem[0];
    mem1 = dut.my_fifo_inst.fifo_inst.u_ipm_distributed_fifo_fifo.ipm_distributed_sdpram_fifo.mem[1];

    got = 0; errors = 0; state = 1;
    $fdisplay(fd, "ENTER POLL (t=%0t) mem[0]=0x%04h mem[1]=0x%04h", $time, mem0, mem1);

    while (state == 1) begin
      fmc_wr_addr(16'h0001);
      fmc_rd_data(addr1);
      $fdisplay(fd, "[poll] addr1=%0d (t=%0t)", addr1, $time);
      if (addr1 == 16'd255) begin
        for (i = 0; i < ALL_LENGTH + 3; i = i + 1) begin
          fmc_wr_addr(16'h0002);
          fmc_rd_data(a2);
          fmc_wr_addr(16'h0003);
          fmc_rd_data(a3);
          $fdisplay(fd, "  i=%0d  num=%0d  data=0x%04h", i, a2, a3);
          if (a2 > 16'd3 && a2 < ALL_LENGTH + 16'd4) begin
            data_arr[a2-4] = a3;
            num_arr[a2-4]  = a2;
            got = got + 1;
          end
        end
        state = 0;
      end
    end

    // ---------- 自检 ----------
    $fdisplay(fd, "\n================ SELF-CHECK (OUTREG=%0d) ================", `OUTREG);
    $fdisplay(fd, "captured %0d words written to FIFO this frame", widx);
    $fdisplay(fd, "collected valid points: %0d (expect %0d)", got, ALL_LENGTH);
    if (got != ALL_LENGTH) begin
      $fdisplay(fd, "  [FAIL] point count mismatch"); errors = errors + 1;
    end

    // num=4+k 读回的数据，应等于该帧第 k 个写入 FIFO 的样本 expected[k]
    for (k = 0; k < ALL_LENGTH; k = k + 1) begin
      if (num_arr[k] !== (4 + k)) begin
        $fdisplay(fd, "  [FAIL] idx %0d : num=%0d expect %0d", k, num_arr[k], 4+k);
        errors = errors + 1;
      end
      if (data_arr[k] !== expected[k]) begin
        $fdisplay(fd, "  [FAIL] idx %0d : num=%0d read=0x%04h  but FIFO-written[%0d]=0x%04h",
                  k, num_arr[k], data_arr[k], k, expected[k]);
        errors = errors + 1;
      end
    end

    if (errors == 0)
      $fdisplay(fd, "RESULT: PASS  -- protocol self-consistent, num/data aligned");
    else
      $fdisplay(fd, "RESULT: FAIL  -- %0d issue(s)", errors);

    $fflush(fd);
    $fclose(fd);
    $finish;
  end

  // 看门狗
  initial begin
    #200000;
    $fdisplay(fd, "[TIMEOUT] simulation did not finish");
    $fflush(fd);
    $fclose(fd);
    $finish;
  end

endmodule
