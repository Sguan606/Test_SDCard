#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"

#include "stm32h7xx_hal.h"
#include "gpio.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ο�������guanfu_wang���롣
//ALIENTEKս��STM32������V3
//SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/18
//�汾��V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////
  
//#define SCCB_SDA_IN()  {GPIOG->CRH&=0XFF0FFFFF;GPIOG->CRH|=0X00800000;}
//#define SCCB_SDA_OUT() {GPIOG->CRH&=0XFF0FFFFF;GPIOG->CRH|=0X00300000;}


// ���� SCCB ������
#define SCCB_SDA_HIGH() HAL_GPIO_WritePin(Ov7670_SIOD_GPIO_Port, Ov7670_SIOD_Pin, GPIO_PIN_SET)
#define SCCB_SDA_LOW()  HAL_GPIO_WritePin(Ov7670_SIOD_GPIO_Port, Ov7670_SIOD_Pin, GPIO_PIN_RESET)
#define SCCB_SCL_HIGH() HAL_GPIO_WritePin(Ov7670_SIOC_GPIO_Port, Ov7670_SIOC_Pin, GPIO_PIN_SET)
#define SCCB_SCL_LOW()  HAL_GPIO_WritePin(Ov7670_SIOC_GPIO_Port, Ov7670_SIOC_Pin, GPIO_PIN_RESET)
#define SCCB_READ_SDA() HAL_GPIO_ReadPin(Ov7670_SIOD_GPIO_Port, Ov7670_SIOD_Pin) 
   
#define SCCB_ID   			0X42  			//OV7670��ID

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);
#endif













