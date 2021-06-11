#include "rtc.h"


//月份
u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};




/*	RTC初始化
 *	返回值：0成功 ，1失败
 */
u8 RTC_Init(void)
{
	
	u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问
  
	//检查是否第一次配置时钟
	if (BKP_ReadBackupRegister(BKP_DRx) != 0xFF00)		//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
	{	 			
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
		{
			temp++;
			delay_ms(10);
		}
		if(temp>=250)
			return 1;//初始化时钟失败,晶振有问题	
    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		
		RTC_WaitForSynchro();		//等待RTC寄存器同步  
		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断		
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		
		RTC_EnterConfigMode(); // 允许配置	
		RTC_SetPrescaler(32767); //设置RTC预分频 分频系数自动+1
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		
		RTC_SetCounter(0); //初始化为0
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DRx,0xFF00);//向指定的后备寄存器中写入用户程序数据
	}
	else//系统继续计时
	{
		RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
	}		    				     
	
	PWR_BackupAccessCmd(DISABLE); //关闭后备寄存器访问
	return 0; //ok

}		 				    

/*	判断润年
 *	返回值：1润年 0平年
 */

u8 Runnian(u16 year)
{
	if(year%4==0)
	{
		if(year%100==0)
		{
			if(year%400==0)return 1;
			else return 0;
		}
		else return 1;
	}
	else return 0;
}

/*	设置RTC时间
 *	返回值：0成功 1失败
 */
 
u8 RTC_Time_Set(u16 year,u8 month,u8 day,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 time;
	
	if(year<1970||year>2099)
		return 1;	
	
	//计算年
	for(t=1970;t<year;t++)
	{
		if(Runnian(t))
			time += 31622400; //闰年的秒钟数
		else
			time += 31536000; //平年的秒钟数
	}
	//计算月
	if(Runnian(year))mon_table[1]=29;//润年2月多1天
	for(t=0;t<month-1;t++)
	{
			time += mon_table[t]*86400;//每天的秒钟数
	}	
	mon_table[1]=28;
	//计算日
	time += (day-1)*86400;
	//计算时
	time += hour *3600;
	//计算分
	time += min *60;
	//计算秒
	time += sec;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
	RTC_SetCounter(time);	//设置RTC计数器的值
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成 
	PWR_BackupAccessCmd(DISABLE); //关闭RTC和后备寄存器访问
	return 0;
}


/* 	获取RTC时间
 *	返回值：时间结构体
 */

_calendar RTC_Get(void)
{
	u16 data;
	u32 temp;
	u32 rtc_data;
	u16 temp1; 
	_calendar calendar;
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
	
	rtc_data = RTC_GetCounter();
	//得到总天数
	temp = rtc_data/86400; 
	
	//判断是否达到1天以上
	if(temp)
	{
		data=1970;
		//计算年份
		while(temp >= 365)
		{
			
			if(Runnian(data))
			{
				if(temp >= 366)
				{
					temp -= 366;
					data++;
				}
				else 
					break;		
			}
			else
			{
				temp -= 365;
				data++;
			}
		}
		calendar.year=data;
		temp1=temp+1;
		//计算月份
		data=0;
		if(Runnian(calendar.year))mon_table[1]=29;//润年2月多1天
		while(temp >= 28)
		{
			temp-=mon_table[data];
			data++;
		}
		calendar.month=data+1;
		//计算日
		calendar.day=temp+1;	
	}
	
	//计算时/分/秒
	temp=rtc_data%86400;
	calendar.hour=temp/3600;     	//小时
	calendar.min=(temp%3600)/60; 	//分钟	
	calendar.sec=(temp%3600)%60; 	//秒钟
	
	//计算星期
	data=0;
	data=(calendar.year-1)
	+((calendar.year-1)/4)
	-((calendar.year-1)/100)
	+((calendar.year-1)/400)
	+temp1;
	
	calendar.week=data;
	
	
	
	return calendar;
}




















