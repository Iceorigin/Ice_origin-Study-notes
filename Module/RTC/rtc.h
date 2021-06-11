#ifndef _rtc_H
#define _rtc_H

#include "system.h"

/*用户配置*/
#define BKP_DRx BKP_DR1






//时间结构体
typedef struct 
{
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec;	
	u8 week;		 
}_calendar;					 

extern _calendar calendar;	//日历结构体

u8 RTC_Init(void); //RTC初始化
u8 RTC_Time_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec); //设置RTC时间
u8 Runnian(u16 year); //判断润年
_calendar RTC_Get(void); //获取RTC时间


#endif








