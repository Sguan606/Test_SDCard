#ifndef __DELAY_H
#define __DELAY_H

#include "stm32h7xx_hal.h"

void TIM1_Delay_Init(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
void delay_s(uint16_t s);



#endif
