#include "sys.h"
#include "sccb.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ο�������guanfu_wang���롣
//ALIENTEK��ӢSTM32������V3
//SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/18
//�汾��V1.0		    							    							  
//////////////////////////////////////////////////////////////////////////////////

 
void SCCB_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = Ov7670_SIOD_Pin; // �˿�����
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Ov7670_SIOD_GPIO_Port,&GPIO_InitStructure);
}

void SCCB_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = Ov7670_SIOD_Pin; // �˿�����
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT; //����
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(Ov7670_SIOD_GPIO_Port,&GPIO_InitStructure);
}

// ��ʼ�� SCCB
void SCCB_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // ʹ��GPIOBʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // ����PB11Ϊ��������ģʽ
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // ����PB10Ϊ�������ģʽ
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // �����������ų�ʼ״̬Ϊ��
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
    
    // ����SDAΪ���ģʽ������SCCBЭ����Ҫ��
    SCCB_SDA_OUT();
}

// SCCB ��ʼ�ź�
void SCCB_Start(void)
{
    SCCB_SDA_HIGH();    // �����߸ߵ�ƽ
    SCCB_SCL_HIGH();    // ��ʱ���߸ߵ�ʱ���������ɸ�����
    delay_us(50);
    SCCB_SDA_LOW();
    delay_us(50);
    SCCB_SCL_LOW();     // �����߻ָ��͵�ƽ
}

// SCCB ֹͣ�ź�
void SCCB_Stop(void)
{
    SCCB_SDA_LOW();
    delay_us(50);
    SCCB_SCL_HIGH();
    delay_us(50);
    SCCB_SDA_HIGH();
    delay_us(50);
}

// ���� NA �ź�
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
// SCCB д��һ���ֽ�
// ����ֵ: 0-�ɹ�; 1-ʧ��
uint8_t SCCB_WR_Byte(uint8_t dat)
{
    uint8_t j, res;
    for(j = 0; j < 8; j++) // ѭ��8�η�������
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
    SCCB_SDA_IN();      // ����SDAΪ����
    delay_us(50);
    SCCB_SCL_HIGH();    // ���յھ�λ�����ж��Ƿ��ͳɹ�
    delay_us(50);
    
    if(SCCB_READ_SDA()) 
        res = 1;  // SDA=1����ʧ�ܣ�����1
    else 
        res = 0;  // SDA=0���ͳɹ�������0
    SCCB_SCL_LOW();
    SCCB_SDA_OUT();     // ����SDAΪ���   
    return res;
}
// SCCB ��ȡһ���ֽ�
// ����ֵ: ����������
uint8_t SCCB_RD_Byte(void)
{
    uint8_t temp = 0, j;
    SCCB_SDA_IN();      // ����SDAΪ����
    
    for(j = 8; j > 0; j--) // ѭ��8�ν�������
    {
        delay_us(50);
        SCCB_SCL_HIGH();
        temp = temp << 1;
        if(SCCB_READ_SDA()) 
            temp++;
        delay_us(50);
        SCCB_SCL_LOW();
    }
    SCCB_SDA_OUT();     // ����SDAΪ���
    return temp;
}			    
//д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
u8 SCCB_WR_Reg(u8 reg,u8 data)
{
	u8 res=0;
	SCCB_Start(); 					//����SCCB����
	if(SCCB_WR_Byte(SCCB_ID))res=1;	//д����ID	  
	delay_us(100);
  	if(SCCB_WR_Byte(reg))res=1;		//д�Ĵ�����ַ	  
	delay_us(100);
  	if(SCCB_WR_Byte(data))res=1; 	//д����	 
  	SCCB_Stop();	  
  	return	res;
}		  					    
//���Ĵ���
//����ֵ:�����ļĴ���ֵ
u8 SCCB_RD_Reg(u8 reg)
{
	u8 val=0;
	SCCB_Start(); 				//����SCCB����
	SCCB_WR_Byte(SCCB_ID);		//д����ID	  
	delay_us(100);	 
  	SCCB_WR_Byte(reg);			//д�Ĵ�����ַ	  
	delay_us(100);	  
	SCCB_Stop();   
	delay_us(100);	   
	//���üĴ�����ַ�󣬲��Ƕ�
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID|0X01);	//���Ͷ�����	  
	delay_us(100);
  	val=SCCB_RD_Byte();		 	//��ȡ����
  	SCCB_No_Ack();
  	SCCB_Stop();
  	return val;
}


