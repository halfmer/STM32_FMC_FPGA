#ifndef __Delay_H__
#define __Delay_H__

#include "stm32h7xx.h"                  // Device header

void Delay_Init(uint16_t sysclk);
void Delay_us(uint32_t nus);

#endif
