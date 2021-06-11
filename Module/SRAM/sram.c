#include "sram.h"


/*******************************************************************************
* 函 数 名         : FSMC_SRAM_Init
* 函数功能		     : 初始化外部SRAM
*******************************************************************************/
void FSMC_SRAM_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_ReadWriteNORSRAMTiming; 

	//PinD,PinE-FSMC_DATA 	  PinF,PinG-FSMC_A
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;	  //FSMC_NE3 PG10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;//FSMC_NBL0~1 PE0	PE1
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;	  //FSMC_NOE,FSMC_NWE PinD4 PinD5
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10 
															|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9
	                            |GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12
								              |GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3 
															|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_12
															|GPIO_Pin_13|GPIO_Pin_14| GPIO_Pin_15 );
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2  
															|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	 
 	FSMC_ReadWriteNORSRAMTiming.FSMC_AddressSetupTime=0x00;	 //地址建立时间（ADDSET）为1个HCLK 
	FSMC_ReadWriteNORSRAMTiming.FSMC_AddressHoldTime=0x00;	 //地址保持时间（ADDHLD）模式A未用到	
	FSMC_ReadWriteNORSRAMTiming.FSMC_DataSetupTime=0x08;		 ////数据保持时间（DATAST）为9个HCLK 	 	 
	FSMC_ReadWriteNORSRAMTiming.FSMC_BusTurnAroundDuration=0x00;
	FSMC_ReadWriteNORSRAMTiming.FSMC_CLKDivision=0x00;
	FSMC_ReadWriteNORSRAMTiming.FSMC_DataLatency=0x00;
	FSMC_ReadWriteNORSRAMTiming.FSMC_AccessMode=FSMC_AccessMode_A;	 //模式A 
    
	FSMC_NORSRAMInitStructure.FSMC_Bank=FSMC_Bank1_NORSRAM3;//  这里我们使用NE3 ，也就对应BTCR[4],[5]。
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux=FSMC_DataAddressMux_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_MemoryType=FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth=FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit  
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode=FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity=FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode=FSMC_WrapMode_Disable;   
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive=FSMC_WaitSignalActive_BeforeWaitState;  
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation=FSMC_WriteOperation_Enable;	//存储器写使能 
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal=FSMC_WaitSignal_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode=FSMC_ExtendedMode_Disable; // 读写使用相同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst=FSMC_WriteBurst_Disable;  
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct=&FSMC_ReadWriteNORSRAMTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct=&FSMC_ReadWriteNORSRAMTiming; //读写同样时序

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // 使能BANK1区域3										  
											
}


/*******************************************************************************
* 函 数 名         : FSMC_SRAM_WriteBuffer
* 函数功能		     : 在指定地址(WriteAddr+Bank1_SRAM3_ADDR)开始,连续写入n个字节.
* 输    入         : pBuffer  :字节指针 
					           WriteAddr:要写入的地址
										 n        :要写入的字节数
* 输    出         : 无
*******************************************************************************/
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)  
	{										    
		*(u8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer++;	 		
		WriteAddr++;
	}   
}		


/*******************************************************************************
* 函 数 名         : FSMC_SRAM_WriteBuffer
* 函数功能		     : 在指定地址((WriteAddr+Bank1_SRAM3_ADDR))开始,连续读出n个字节.
* 输    入         : pBuffer  :字节指针 
					           WriteAddr:要写入的地址
										 n        :要写入的字节数
* 输    出         : FSMC_SRAM_ReadBuffer
*******************************************************************************/
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)  
	{											    
		*pBuffer++=*(u8*)(Bank1_SRAM3_ADDR+ReadAddr);    
		ReadAddr++;
	}  
} 


/*******************************************************************************
* 函 数 名         : ExSRAM_Cap_Test
* 函数功能		     : 外部内存测试(最大支持1M字节内存测试)	
* 输    出         : 串口输出打印
*******************************************************************************/ 
/*
void ExSRAM_Cap_Test(void)
{
  u8 writeData=0xf0, readData;
	u16 cap=0;
  u32 addr;
	
	addr = 1024; //从1KB位置开始算起

	while(1)
	{
		FSMC_SRAM_WriteBuffer(&writeData, addr, 1);
		FSMC_SRAM_ReadBuffer(&readData,addr,1);
		
		//查看读取到的数据是否跟写入数据一样
    if(readData == writeData)
    {
			cap++;
			addr += 1024;
			readData = 0;
			if(addr > 1024 * 1024)break; //SRAM容量最大为1MB   
		}
		else break;    
	}
	
	printf("SRAM容量为：%dKB\r\n",cap);
}
*/




