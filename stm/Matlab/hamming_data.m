clc;
clear all;
close all;

% 参数设置
N = 1024;                       % 窗点数
win_type = 'hamming';           % 窗类型（可改为hann/blackman等）

% 生成窗函数
switch win_type
    case 'hamming'
        win = hamming(N);
    case 'hann'
        win = hann(N);
    case 'blackman'
        win = blackman(N);
    otherwise
        error('Unsupported window type');
end

% 转换为float32单精度浮点数
win_float32 = single(win);

% 创建并写入C语言头文件
filename = 'D:\Ele_Com_new\FPGA_new_new\now_work\body_FMC+FIFO_prefer_end - againing - can_use2 good - over\Project - success - again\Matlab\window_1024.h';
fid = fopen(filename, 'w');
if fid == -1
    error('Failed to open file for writing');
end

% 写入文件头
fprintf(fid, '/* Auto-generated window function (%s) */\n', win_type);
fprintf(fid, '#include <stdint.h>\n\n');
fprintf(fid, 'const float window[%d] = {\n', N);

% 写入数据内容（每行8个值）
values_per_line = 8;
for i = 1:N
    % 写入浮点数值（科学计数法保证精度）
    if mod(i, values_per_line) == 1
        fprintf(fid, '    ');  % 行首缩进
    end
    
    fprintf(fid, '%.8e', win_float32(i));  % 科学计数法格式
    
    % 添加逗号（最后一个除外）
    if i < N
        fprintf(fid, ', ');
    end
    
    % 换行控制
    if mod(i, values_per_line) == 0 || i == N
        fprintf(fid, '\n');
    end
end

% 结束数组
fprintf(fid, '};\n');

% 关闭文件
fclose(fid);

disp(['C header file generated successfully: ', filename]);
disp('First 5 values:');
disp(win_float32(1:5)');