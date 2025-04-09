#include "delay.h"

// TIM1 初始化函数
void TIM1_Delay_Init(void)
{
    // 使能 TIM1 时钟
    __HAL_RCC_TIM1_CLK_ENABLE();
    
    // 配置 TIM1 为基本定时器模式
    TIM1->PSC = HAL_RCC_GetHCLKFreq() / 1000000 - 1;  // 分频至 1MHz (1us/tick)
    TIM1->ARR = 0xFFFF;  // 最大自动重装载值
    TIM1->EGR = TIM_EGR_UG;  // 生成更新事件，重载预分频器
    TIM1->CR1 = TIM_CR1_CEN;  // 启动定时器
    
    // 等待定时器稳定
    while(!(TIM1->SR & TIM_SR_UIF));
    TIM1->SR = ~TIM_SR_UIF;
}


// 微秒级延时 (1-65535 us)
void delay_us(uint16_t us)
{
    TIM1->CNT = 0;  // 重置计数器
    while(TIM1->CNT < us);  // 等待计数达到指定值
}

// 毫秒级延时 (1-65535 ms)
void delay_ms(uint16_t ms)
{
    while(ms--)
    {
        delay_us(1000);  // 调用1000次1us延时
    }
}

// 秒级延时 (1-65535 s)
void delay_s(uint16_t s)
{
    while(s--)
    {
        delay_ms(1000);  // 调用1000次1ms延时
    }
}
