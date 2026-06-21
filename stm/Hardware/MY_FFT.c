#include "main.h"                  // Device header
#include "MY_FFT.h"
#include <stdio.h>
#include "arm_math.h"
#include "ADD_WIN.h"
#include "MY_FIR.h"

arm_rfft_fast_instance_f32 S;
arm_fir_instance_f32 F;

float32_t testInput_f32_fir[FFT_Length];

float32_t testOutput_f32[FFT_Length];
float32_t testOutputMag_f32[FFT_Length/2+1];

uint8_t ifftFlag;//0--FFT

#define tri_threshold 			5000

int WaveA_Tri;

int WaveA_Tri_3;
int WaveA_Tri_5;
int WaveA_Tri_7;


int WaveB_Tri;

int WaveB_Tri_3;
int WaveB_Tri_5;
int WaveB_Tri_7;

void my_fft_init(void)
{
	arm_rfft_fast_init_f32(&S, FFT_Length);
	arm_fir_init_f32(&F, FIR_LENGTH, &LOW_5M_100[0], &LOW_5M_100_state[0], FFT_Length);
}

void my_fft(float32_t *testInput_f32)
{

	ifftFlag = 0;
	float maxVal1;
	float maxVal2;
	uint32_t maxIndex1;
	uint32_t maxIndex2;
	
	float32_t Direct_flow;
	
	arm_fir_f32(&F, testInput_f32, testInput_f32_fir, FFT_Length);
	
	for(int i = 0; i < FFT_Length; i++) //去直流
	{
		Direct_flow += testInput_f32_fir[i];
	}
	Direct_flow /= FFT_Length;

	for(int i = 0; i < FFT_Length; i++)
	{
		testInput_f32_fir[i] = testInput_f32_fir[i] - Direct_flow;
	}
	
	//for(int i = 0; i < FFT_Length; i++) 
	//{
	//	printf("%f,%f\r\n",testInput_f32[i],testInput_f32_fir[i]);
	//}
	
	arm_rfft_fast_f32(&S, testInput_f32_fir, testOutput_f32, ifftFlag);/* 1024 点实序列快速 FFT */ 

	arm_cmplx_mag_f32(testOutput_f32, testOutputMag_f32, FFT_Length/2);/* 求解了 1024 组模值*/ 

	//for(int i = 0; i < FFT_Length/2+1; i++)
	//{
	//	printf("%f\r\n",testInput_f32_fir[i]);
	//}
	
	arm_max_f32(testOutputMag_f32, FFT_Length/2, &maxVal1, &maxIndex1);/* 寻找最大幅值和索引点*/ 
	
	uint32_t freA = maxIndex1*(130000000/FFT_Length);
	
	testOutputMag_f32[maxIndex1] = 0;
	
	arm_max_f32(testOutputMag_f32, FFT_Length/2, &maxVal2, &maxIndex2);/* 寻找最大幅值和索引点*/ 
	
	uint32_t freB = maxIndex2*(130000000/FFT_Length);
	
	printf("%f,%d,%f,%d\r\n",maxVal1,freA,maxVal2,freB);
	
}

void tri_sin(float32_t *testInput_f32)
{
	ifftFlag = 0;
	float maxVal;
	uint32_t maxIndex1;
	uint32_t maxIndex2;
	WaveA_Tri=0;
	WaveB_Tri=0;
	
	float32_t Direct_flow;
	
	arm_rfft_fast_init_f32(&S, FFT_Length);
	
	for(int i = 0; i < FFT_Length; i++) //去直流
	{
		Direct_flow += testInput_f32[i];
	}
	Direct_flow /= FFT_Length;

	for(int i = 0; i < FFT_Length; i++)
	{
		testInput_f32[i] = testInput_f32[i] - Direct_flow;
	}

	for (int i = 0; i < FFT_Length; i++) 
	{
		testInput_f32[i] = testInput_f32[i]*hanning_window[i];
	}
	
	//for(int i = 0; i < FFT_Length; i++) //去直流
	//{
	//	printf("%f\r\n",testInput_f32[i]);
	//}
	
	arm_rfft_fast_f32(&S, testInput_f32, testOutput_f32, ifftFlag);/* 4096 点实序列快速 FFT */ 

	arm_cmplx_mag_f32(testOutput_f32, testOutputMag_f32, FFT_Length/2);/* 求解了 2048 组模值*/ 

	//for(int i = 0; i < FFT_Length/2+1; i++) //去直流
	//{
	//	printf("%f,%d\r\n",testOutputMag_f32[i],i);
	//}
	
	arm_max_f32(testOutputMag_f32, FFT_Length/2, &maxVal, &maxIndex1);/* 寻找最大幅值和索引点*/ 
	
	WaveA_Tri_3 = (maxIndex1*3<FFT_Length/2)?maxIndex1*3:0;
	WaveA_Tri_5 = (maxIndex1*5<FFT_Length/2)?maxIndex1*5:0;
	WaveA_Tri_7 = (maxIndex1*7<FFT_Length/2)?maxIndex1*7:0;
	
	if(testOutputMag_f32[WaveA_Tri_3] > tri_threshold)
	{
		WaveA_Tri++;
	}
	if(testOutputMag_f32[WaveA_Tri_5] > tri_threshold)
	{
		WaveA_Tri++;
	}
	if(testOutputMag_f32[WaveA_Tri_7] > tri_threshold)
	{
		WaveA_Tri++;
	}
	
	uint32_t freA = maxIndex1*(130000000/FFT_Length);
		
	printf("%d,%d,%d\r\n",maxIndex1,freA,WaveA_Tri);
	
}

