/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types （获取整数类型）*/
#include "diskio.h"		/* Declarations of disk functions （磁盘函数声明）*/
#include "sd_sdio.h"
#include "flash.h"
#include "malloc.h"	
#include "usart.h"

/* Definitions of physical drive number for each drive */
/* 每个驱动器的物理驱动器编号定义 */
#define DEV_SD_CARD		   0	 //SD卡,卷标为0
#define DEV_EX_FLASH		 1	 //外部flash,卷标为1

//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,字库占用3.09M.
u16	    FLASH_SECTOR_COUNT=2048*12;	//W25Q1218,前12M字节给FATFS占用
#define FLASH_SECTOR_SIZE 	512			//块大小(KB)
#define FLASH_BLOCK_SIZE   	16     	//每个BLOCK有16个扇区



/*-----------------------------------------------------------------------*/
/* Get Drive Status 获取驱动器状态                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive （用于识别驱动器的物理驱动器号） */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive 初始化驱动器                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	uint8_t result;

	switch (pdrv) {
	case DEV_SD_CARD :   //SD卡
		result = SD_Init();//SD卡初始化
		break;
	
	case DEV_EX_FLASH :		//外部FLASH
		EN25QXX_Init();     //外部FLASH初始化
		FLASH_SECTOR_COUNT=2048*12;
		result=0;
		break;
	
	default:
		result=1;
		break;
	}
	if(result==0)
		return  STA_NOINIT;
	else
		return 0; //初始化成功 
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s) 读取扇区																							 */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* 用于识别驱动器的物理驱动器号 */
	BYTE *buff,		/* 用于存储读取数据的数据缓冲区 */
	LBA_t sector,	/* Start sector in LBA （扇区地址） */
	UINT count		/* 需要读取的扇区数 */
)
{
	uint8_t result=0; 
  if (!count)
		return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case DEV_SD_CARD://SD卡
			result=SD_ReadDisk(buff,sector,count);	 
			while(result)//读出错
			{
				SD_Init();	//重新初始化SD卡
				result=SD_ReadDisk(buff,sector,count);	
				printf("Read Sector Error:%d\r\n",result);
			}
			break;
		case DEV_EX_FLASH://外部flash
			for(;count>0;count--)
			{
				EN25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			result=0;
			break;
		default:
			result=1; 
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(result==0x00)return RES_OK;	 
    else return RES_ERROR;	 
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s) 写入扇区																							 */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	uint8_t result;

	if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误	
	
	switch (pdrv) {
	case DEV_SD_CARD :
		result=SD_WriteDisk((u8*)buff,sector,count);
		if(result!=0)
		printf("sd wr error:%d\r\n",result);
		break;
		
	case DEV_EX_FLASH :
		for(;count>0;count--)
		{										    
			EN25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
			sector++;
			buff+=FLASH_SECTOR_SIZE;
		}
		result=0;
		break;
		
	default:
		result=1;
		break;
		
	}
	//处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
	if(result == 0x00)
		return RES_OK;	 
	else 
		return RES_ERROR;	
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions   其他表参数的获得                            */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code (控制代码)*/
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT result=RES_OK;

	switch (pdrv) {
		case DEV_SD_CARD :
		switch(cmd)
		{
			case CTRL_SYNC:
				result = RES_OK; 
				break;	 
			case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
				result = RES_OK;
				break;	 
			case GET_BLOCK_SIZE:
				*(WORD*)buff = SDCardInfo.CardBlockSize;
				result = RES_OK;
				break;	 
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = SDCardInfo.CardCapacity/512;
				result = RES_OK;
				break;
			default:
				result = RES_PARERR;
				break;
		}
		break;
		
		case DEV_EX_FLASH :
		switch(cmd)
		{
			case CTRL_SYNC:
				result = RES_OK; 
				break;	 
			case GET_SECTOR_SIZE:
				*(WORD*)buff = FLASH_SECTOR_SIZE;
				result = RES_OK;
				break;	 
			case GET_BLOCK_SIZE:
				*(WORD*)buff = FLASH_BLOCK_SIZE;
				result = RES_OK;
				break;	 
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = FLASH_SECTOR_COUNT;
				result = RES_OK;
				break;
			default:
				result = RES_PARERR;
				break;
		}
		break;
	default:
		result = RES_ERROR;
		break;
	}
	
	return result;
}

