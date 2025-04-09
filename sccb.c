#include "sys.h"
#include "sccb.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序参考自网友guanfu_wang代码。
//ALIENTEK精英STM32开发板V3
//SCCB 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/18
//版本：V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////

 
void SCCB_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = Ov7670_SIOD_Pin; // 端口配置
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Ov7670_SIOD_GPIO_Port,&GPIO_InitStructure);
}

void SCCB_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = Ov7670_SIOD_Pin; // 端口配置
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT; //输入
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(Ov7670_SIOD_GPIO_Port,&GPIO_InitStructure);
}

// 初始化 SCCB
void SCCB_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // 配置PB11为输入上拉模式
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // 配置PB10为推挽输出模式
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // 设置两个引脚初始状态为高
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
    
    // 设置SDA为输出模式（根据SCCB协议需要）
    SCCB_SDA_OUT();
}

// SCCB 起始信号
void SCCB_Start(void)
{
    SCCB_SDA_HIGH();    // 数据线高电平
    SCCB_SCL_HIGH();    // 在时钟线高的时候数据线由高至低
    delay_us(50);
    SCCB_SDA_LOW();
    delay_us(50);
    SCCB_SCL_LOW();     // 数据线恢复低电平
}

// SCCB 停止信号
void SCCB_Stop(void)
{
    SCCB_SDA_LOW();
    delay_us(50);
    SCCB_SCL_HIGH();
    delay_us(50);
    SCCB_SDA_HIGH();
    delay_us(50);
}

// 产生 NA 信号
void SCCB_No_Ack(void)
{
    delay_us(50);
    SCCB_SDA_HIGH();
    SCCB_SCL_HIGH();
    delay_us(50);
    SCCB_SCL_LOW();
    delay_us(50);
    SCCB_SDA_LOW();
    delay_us(50);
}
// SCCB 写入一个字节
// 返回值: 0-成功; 1-失败
uint8_t SCCB_WR_Byte(uint8_t dat)
{
    uint8_t j, res;
    for(j = 0; j < 8; j++) // 循环8次发送数据
    {
        if(dat & 0x80) 
            SCCB_SDA_HIGH();
        else 
            SCCB_SDA_LOW();
        dat <<= 1;
        delay_us(50);
        SCCB_SCL_HIGH();
        delay_us(50);
        SCCB_SCL_LOW();
    }
    SCCB_SDA_IN();      // 设置SDA为输入
    delay_us(50);
    SCCB_SCL_HIGH();    // 接收第九位，以判断是否发送成功
    delay_us(50);
    
    if(SCCB_READ_SDA()) 
        res = 1;  // SDA=1发送失败，返回1
    else 
        res = 0;  // SDA=0发送成功，返回0
    SCCB_SCL_LOW();
    SCCB_SDA_OUT();     // 设置SDA为输出   
    return res;
}
// SCCB 读取一个字节
// 返回值: 读到的数据
uint8_t SCCB_RD_Byte(void)
{
    uint8_t temp = 0, j;
    SCCB_SDA_IN();      // 设置SDA为输入
    
    for(j = 8; j > 0; j--) // 循环8次接收数据
    {
        delay_us(50);
        SCCB_SCL_HIGH();
        temp = temp << 1;
        if(SCCB_READ_SDA()) 
            temp++;
        delay_us(50);
        SCCB_SCL_LOW();
    }
    SCCB_SDA_OUT();     // 设置SDA为输出
    return temp;
}			    
//写寄存器
//返回值:0,成功;1,失败.
u8 SCCB_WR_Reg(u8 reg,u8 data)
{
	u8 res=0;
	SCCB_Start(); 					//启动SCCB传输
	if(SCCB_WR_Byte(SCCB_ID))res=1;	//写器件ID	  
	delay_us(100);
  	if(SCCB_WR_Byte(reg))res=1;		//写寄存器地址	  
	delay_us(100);
  	if(SCCB_WR_Byte(data))res=1; 	//写数据	 
  	SCCB_Stop();	  
  	return	res;
}		  					    
//读寄存器
//返回值:读到的寄存器值
u8 SCCB_RD_Reg(u8 reg)
{
	u8 val=0;
	SCCB_Start(); 				//启动SCCB传输
	SCCB_WR_Byte(SCCB_ID);		//写器件ID	  
	delay_us(100);	 
  	SCCB_WR_Byte(reg);			//写寄存器地址	  
	delay_us(100);	  
	SCCB_Stop();   
	delay_us(100);	   
	//设置寄存器地址后，才是读
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID|0X01);	//发送读命令	  
	delay_us(100);
  	val=SCCB_RD_Byte();		 	//读取数据
  	SCCB_No_Ack();
  	SCCB_Stop();
  	return val;
}


