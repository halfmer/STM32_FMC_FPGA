#ifndef __FMC_FPGA_H__
#define __FMC_FPGA_H__

#include "stm32h7xx.h"                  // Device header
#include "arm_math.h"
/* FPGA register window. */
typedef struct
{
    volatile uint16_t FPGA_REG;
    volatile uint16_t FPGA_RAM;
} FPGA_TypeDef;

#define FPGA_FSMC_NEX         2          
#define FPGA_FSMC_AX          12

/* FSMC_A12, Bank2 */
#define FPGA_BASE        (uint32_t)((0x60000000 + (0x4000000 * (FPGA_FSMC_NEX - 1))) | (((1 << FPGA_FSMC_AX) * 2) -2))
#define FPGA             ((FPGA_TypeDef *) FPGA_BASE)

void FMC_WR_ADDR(volatile uint16_t addr);
void FMC_WR_DATA(volatile uint16_t data);
void FMC_WriteReg(uint16_t addr,uint16_t data);
uint16_t FMC_RD_DATA(void);
void FMC_RD_ADC_Data(uint16_t addr,uint16_t *buffer, uint16_t buffer_size);
void FMC_DET_DATA(uint16_t *FMC_NUM , uint16_t *FMC_ADC_DATA);
void FMC_SelfTest_Display(void);
#endif
