#ifndef __MY_FFT_H__
#define __MY_FFT_H__

#include "stm32h7xx.h"                  // Device header
#include "arm_math.h"

#define 	FFT_Length		1024

void my_fft_init(void);
void my_fft(float32_t *fft_wave);
void tri_sin(float32_t *fft_wave);

#endif
