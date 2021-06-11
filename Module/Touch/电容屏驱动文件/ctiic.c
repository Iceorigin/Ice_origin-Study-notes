#include "ctiic.h"


/*******************************************************************************
* 函 数 名         : CT_SDA_OUT
* 函数功能		     : CT_SDA_OUT输出配置	   
*******************************************************************************/
void CT_SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
}

/*******************************************************************************
* 函 数 名         : CT_SDA_IN
* 函数功能		     : CT_SDA_IN输出配置	   
*******************************************************************************/
void CT_SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
}

/*******************************************************************************
* 函 数 名         : CT_Delay
* 函数功能		     : 控制I2C速度的延时   
*******************************************************************************/
void CT_Delay(void)
{
	delay_us(5);
}

/********************************************************************************
* 函 数 名         : CT_IIC_Init
* 函数功能		     : 电容触摸芯片IIC接口初始化  
*******************************************************************************/
void CT_IIC_Init(void)
{			
	GPIO_InitTypeDef  GPIO_InitStructure;	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}


/********************************************************************************
* 函 数 名         : CT_IIC_Start
* 函数功能		     : 产生IIC起始信号  
*******************************************************************************/
void CT_IIC_Start(void)
{
	CT_SDA_OUT();//sda线输出
	CT_IIC_SDA=1;	  	  
	CT_IIC_SCL=1;
	delay_us(30);
 	CT_IIC_SDA=0;
	CT_Delay();
	CT_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  


/********************************************************************************
* 函 数 名         : CT_IIC_Start
* 函数功能		     : 产生IIC停止信号 
*******************************************************************************/
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda线输出
	CT_IIC_SCL=1;
	delay_us(30);
	CT_IIC_SDA=0;
	CT_Delay();
	CT_IIC_SDA=1;//发送I2C总线结束信号  
}

/*******************************************************************************
* 函 数 名         : CT_IIC_Wait_Ack
* 函数功能		     : 等待应答信号到来   
* 输    出         : 1，接收应答失败
        			       0，接收应答成功
*******************************************************************************/
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	CT_SDA_IN();      //SDA设置为输入  
	CT_IIC_SDA=1;	   
	CT_IIC_SCL=1;
	CT_Delay();
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 

/*******************************************************************************
* 函 数 名         : CT_IIC_Ack
* 函数功能		     : 产生ACK应答  
*******************************************************************************/
void CT_IIC_Ack(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=0;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}

/*******************************************************************************
* 函 数 名         : CT_IIC_NAck
* 函数功能		     : 产生NACK非应答  
*******************************************************************************/		 
void CT_IIC_NAck(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=1;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}		


/*******************************************************************************
* 函 数 名         : CT_IIC_Send_Byte
* 函数功能		     : IIC发送一个字节 
* 输    入         : txd：发送一个字节
*******************************************************************************/	
void CT_IIC_Send_Byte(u8 txd)
{                        
  u8 t;   
	CT_SDA_OUT(); 	    
  CT_IIC_SCL=0;//拉低时钟开始数据传输
	CT_Delay();
	for(t=0;t<8;t++)
  {              
		CT_IIC_SDA=(txd&0x80)>>7;
		txd<<=1; 	      
		CT_IIC_SCL=1; 
		CT_Delay();
		CT_IIC_SCL=0;	
		CT_Delay();
   }	 
} 	    

/*******************************************************************************
* 函 数 名         : CT_IIC_Read_Byte
* 函数功能		     : IIC读一个字节 
* 输    入         : ack=1时，发送ACK，ack=0，发送nACK 
* 输    出         : 应答或非应答
*******************************************************************************/    
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	CT_SDA_IN();//SDA设置为输入
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL=0; 	    	   
		CT_Delay();
		CT_IIC_SCL=1;	 
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//发送nACK
	else CT_IIC_Ack(); //发送ACK   
 	return receive;
}

