#ifndef _sd_sdio_H
#define _sd_sdio_H


#include "system.h"


/*用户配置区			  
 *	SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK一般为72Mhz
 *	使用DMA模式的时候,传输速率可以到24Mhz,不过如果你的卡不是高速卡,可能也会出错
 *	出错就请降低时钟,使用查询模式的话,推荐SDIO_TRANSFER_CLK_DIV设置为3或者更大
*/
#define SDIO_INIT_CLK_DIV        0xB2 		//SDIO初始化频率，最大400Kh  
#define SDIO_TRANSFER_CLK_DIV    0x04		  //SDIO传输频率,该值太小可能会导致读写文件出错 
 
//SDIO工作模式定义,通过SD_SetDeviceMode函数设置.
#define SD_POLLING_MODE         	0     	//查询模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.
#define SD_DMA_MODE    	        	1	      //DMA模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.  
#define SD_MODE                   SD_POLLING_MODE    //设置模式


//测试专用 0关闭 1启动 
#define  debug 1


//GPIO端口设置

#define SD_Pin_D0                GPIO_Pin_8
#define SD_Pin_D1								 GPIO_Pin_9		
#define SD_Pin_D2                GPIO_Pin_10
#define SD_Pin_D3                GPIO_Pin_11
#define SD_Pin_SCK               GPIO_Pin_12
#define SD_Pin_CMD               GPIO_Pin_2

/*=====================================================================================================================*/

/*-=SDIO 各种错误枚举定义=-*/
typedef enum
{	 
	//特殊错误定义 
	SD_CMD_CRC_FAIL                    = (1),  // 收到命令响应（但 CRC 校验失败） 
	SD_DATA_CRC_FAIL                   = (2),  // 数据块发送/接收（CRC 校验失败） 
	SD_CMD_RSP_TIMEOUT                 = (3),  // 命令响应超时 
	SD_DATA_TIMEOUT                    = (4),  // 数据超时 
	SD_TX_UNDERRUN                     = (5),  // 发送 FIFO 欠载 
	SD_RX_OVERRUN                      = (6),  // 接收 FIFO 溢出 
	SD_START_BIT_ERR                   = (7),  // 在宽总线模式下未在所有数据信号上检测到起始位 
	SD_CMD_OUT_OF_RANGE                = (8),  // CMD 的参数超出范围。
	SD_ADDR_MISALIGNED                 = (9),  // 地址错位误
	SD_BLOCK_LEN_ERR                   = (10), // 卡不允许传输的块长度或传输的字节数与块长度不匹配 
	SD_ERASE_SEQ_ERR                   = (11), // 擦除命令的顺序出现错误。
	SD_BAD_ERASE_PARAM                 = (12), // 擦除组的无效选择
	SD_WRITE_PROT_VIOLATION            = (13), // 尝试对写保护块进行编程 
	SD_LOCK_UNLOCK_FAILED              = (14), // 在解锁命令中检测到序列或密码错误，或者尝试访问锁定的卡 
	SD_COM_CRC_FAILED                  = (15), // 上一条命令的 CRC 检查失败 
	SD_ILLEGAL_CMD                     = (16), // 命令对卡状态不合法 
	SD_CARD_ECC_FAILED                 = (17), // 已应用卡内部 ECC 但未能更正数据 
	SD_CC_ERROR                        = (18), // 内部卡控制器错误 
	SD_GENERAL_UNKNOWN_ERROR           = (19), // 一般或未知错误 
	SD_STREAM_READ_UNDERRUN            = (20), // 该卡无法在流读取操作中支持数据传输。 
	SD_STREAM_WRITE_OVERRUN            = (21), // 该卡无法支持流模式下的数据编程 
	SD_CID_CSD_OVERWRITE               = (22), // CID/CSD 覆盖错误 
	SD_WP_ERASE_SKIP                   = (23), // 仅部分地址空间被擦除 
	SD_CARD_ECC_DISABLED               = (24), // 命令已在不使用内部 ECC 的情况下执行 
	SD_ERASE_RESET                     = (25), // 由于接收到超出擦除序列的命令，擦除序列在执行前被清除 
	SD_AKE_SEQ_ERROR                   = (26), // 验证顺序错误。 
	SD_INVALID_VOLTRANGE               = (27), // SD卡无效电压
	SD_ADDR_OUT_OF_RANGE               = (28), 
	SD_SWITCH_ERROR                    = (29), 
	SD_SDIO_DISABLED                   = (30),
	SD_SDIO_FUNCTION_BUSY              = (31),
	SD_SDIO_FUNCTION_FAILED            = (32),
	SD_SDIO_UNKNOWN_FUNCTION           = (33),
	//标准错误定义
	SD_INTERNAL_ERROR, 
	SD_NOT_CONFIGURED,
	SD_REQUEST_PENDING, 
	SD_REQUEST_NOT_APPLICABLE,                //SD 请求不适用
	SD_INVALID_PARAMETER,                     //无效的参数
	SD_UNSUPPORTED_FEATURE,  
	SD_UNSUPPORTED_HW,  
	SD_ERROR,  
	SD_OK = 0  
} SD_Error;		  


/*-=SD卡CSD寄存器数据=-*/		  
typedef struct
{
	u8  CSDStruct;            // CSD结构 
	u8  SysSpecVersion;       // 系统规格版本 
	u8  Reserved1;            // Reserved 
	u8  TAAC;                 // 数据读访问时间 
	u8  NSAC;                 // 时钟周期(NSAC*100)中的数据读访问时间） 
	u8  MaxBusClkFrec;        // 最大数据传输速率
	u16 CardComdClasses;      // 卡命令类 
	u8  RdBlockLen;           // 最大读取数据长度
	u8  PartBlockRead;        // 允许分块读 
	u8  WrBlockMisalign;      // 写块不对齐 
	u8  RdBlockMisalign;      // 读块不对齐 
	u8  DSRImpl;              // 执行的 DSR
	u8  Reserved2;            // Reserved 
	u32 DeviceSize;           // 设备尺寸
	
  //1.0/2.0/MMC卡参数 	
	u8  MaxRdCurrentVDDMin;   // 最大限度。 读取电流@ VDD 最小值 
	u8  MaxRdCurrentVDDMax;   // 最大限度。 读取电流@ VDD 最大值 
	u8  MaxWrCurrentVDDMin;   // 最大限度。 写入电流@ VDD 最小值 
	u8  MaxWrCurrentVDDMax;   // 最大限度。 写入电流@ VDD 最大值 
	u8  DeviceSizeMul;        // 设备尺寸乘数 
	
	u8  EraseGrSize;          // 擦除组大小 
	u8  EraseGrMul;           // 擦除组大小乘数 
	u8  WrProtectGrSize;      // 写保护组大小 
	u8  WrProtectGrEnable;    // 写保护组使能 
	u8  ManDeflECC;           // 制造商默认 ECC 
	u8  WrSpeedFact;          // 写入速度系数 
	u8  MaxWrBlockLen;        // 最大限度写入数据块长度/允许块部分写
	u8  WriteBlockPaPartial;  // 允许写入的部分块 
	u8  Reserved3;            // Reserded 
	u8  ContentProtectAppli;  // 内容保护应用 
	u8  FileFormatGrouop;     // 文件格式组 
	u8  CopyFlag;             // 复制标志 (OTP) 
	u8  PermWrProtect;        // 永久写保护
	u8  TempWrProtect;        // 临时写保护 
	u8  FileFormat;           // 文件格式 
	u8  ECC;                  // ECC码 
	u8  CSD_CRC;              // CSD CRC 
	u8  Reserved4;            // always 1
} SD_CSD;   

/*-=SD卡CID寄存器数据=-*/
typedef struct
{
	u8  ManufacturerID;       // 制造商ID 
	u16 OEM_AppliID;          // OEM/应用程序 ID 
	u32 ProdName1;            // 产品名称第 1 部分 
	u8  ProdName2;            // 产品名称第 2 部分
	u8  ProdRev;              // 产品版本 
	u32 ProdSN;               // 产品序列号 
	u8  Reserved1;            // Reserved1 
	u16 ManufactDate;         // 生产日期 
	u8  CID_CRC;              // CID CRC 
	u8  Reserved2;            // always 1 
} SD_CID;	 

/*-=SD卡状态=-*/
typedef enum
{
	SD_CARD_READY                  = ((uint32_t)0x00000001), //准备状态
	SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002), //识别状态
	SD_CARD_STANDBY                = ((uint32_t)0x00000003), //待机状态
	SD_CARD_TRANSFER               = ((uint32_t)0x00000004), //传输状态
	SD_CARD_SENDING                = ((uint32_t)0x00000005), //发送数据状态
	SD_CARD_RECEIVING              = ((uint32_t)0x00000006), //接受数据状态
	SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007), //编程状态
	SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008), //断开连接状态
	SD_CARD_ERROR                  = ((uint32_t)0x000000FF)  //错误/无效状态
}SDCardState;

/*-=SD卡信息,包括CSD,CID等数据=-*/
typedef struct
{
  SD_CSD SD_csd;								//SD卡 CSD
  SD_CID SD_cid;								//SD卡 CID
  long long CardCapacity;     	//SD卡容量,单位:字节,最大支持2^64字节大小的卡.
  u32 CardBlockSize; 	        	//SD卡块大小	
  u16 RCA;					            //卡相对地址
  u8 CardType;		      	    	//卡类型
} SD_CardInfo;
extern SD_CardInfo SDCardInfo;  //SD卡信息			



/*-=SDIO 指令集=-*/
#define SD_CMD0                       ((u8)0)  //复位卡到空闲状态
#define SD_CMD1                       ((u8)1) 
#define SD_CMD2                       ((u8)2) 
#define SD_CMD3                       ((u8)3)  // < SDIO_SEND_REL_ADDR 用于 SD 卡 
#define SD_CMD4                       ((u8)4  
#define SD_CMD5                       ((u8)5)
#define SD_CMD6                       ((u8)6)
#define SD_CMD7                       ((u8)7)
#define SD_CMD8                       ((u8)8)
#define SD_CMD9                       ((u8)9)
#define SD_CMD10                      ((u8)10)
#define SD_CMD11                      ((u8)11) //SD卡不支持
#define SD_CMD12                   		((u8)12) //停止（stop）命令
#define SD_CMD13                      ((u8)13)
#define SD_CMD14                      ((u8)14)
#define SD_CMD15                  		((u8)15)
#define SD_CMD16                      ((u8)16)
#define SD_CMD17                   		((u8)17)
#define SD_CMD18                     	((u8)18)
#define SD_CMD19                   		((u8)19)
#define SD_CMD20                			((u8)20) 
#define SD_CMD23                     	((u8)23) 
#define SD_CMD24                 			((u8)24)
#define SD_CMD25                   		((u8)25)
#define SD_CMD26                      ((u8)26)
#define SD_CMD27                      ((u8)27)
#define SD_CMD28                    	((u8)28)
#define SD_CMD29                      ((u8)29)
#define SD_CMD30                    	((u8)30)
#define SD_CMD32                			((u8)32) // 设置要擦除的第一个写块的地址。 （仅适用于 SD 卡） 
#define SD_CMD33                   		((u8)33) // 设置要擦除的连续范围的最后一个写块的地址。 （仅适用于 SD 卡） 
#define SD_CMD35                    	((u8)35) // 设置要擦除的第一个写块的地址。（仅适用于 MMC 卡规范 3.31）
#define SD_CMD36                      ((u8)36) // 设置要擦除的连续范围的最后一个写块的地址。 （仅适用于 MMC 卡规范 3.31） 
#define SD_CMD38                      ((u8)38)
#define SD_CMD39                      ((u8)39) // SD卡不支持 
#define SD_CMD40                      ((u8)40) // SD卡不支持 
#define SD_CMD42                      ((u8)42)
#define SD_CMD55                      ((u8)55)
#define SD_CMD56                      ((u8)56)
#define SD_CMD64                      ((u8)64)


/*	以下命令是 SD 卡特定命令。
 *			在发送这些命令之前应该发送CMD55。
 */
#define SD_ACMD6               			 ((u8)6)  //仅适用于 SD 卡
#define SD_ACMD13                    ((u8)13) //仅适用于 SD 卡 
#define SD_ACMD22        						 ((u8)22) //仅适用于 SD 卡 
#define SD_ACMD41                    ((u8)41) //仅适用于 SD 卡 
#define SD_ACMD42          					 ((u8)42) //仅适用于 SD 卡 
#define SD_ACMD51                    ((u8)51) //仅适用于 SD 卡 
#define SD_ACMD52                    ((u8)52) //仅适用于 SD I/O 卡
#define SD_ACMD53                    ((u8)53) //仅适用于 SD I/O 卡

/*	以下命令是 SD 卡安全命令。
 *			在发送这些命令之前应该发送CMD55。
 */
#define SD_ACMD18 									 ((u8)18) //仅适用于 SD 卡 
#define SD_ACMD25 									 ((u8)25) //仅适用于 SD 卡 
#define SD_ACMD38   		             ((u8)38) //仅适用于 SD 卡 
#define SD_ACMD43                    ((u8)43) //仅适用于 SD 卡 
#define SD_ACMD44                    ((u8)44) //仅适用于 SD 卡 
#define SD_ACMD45      		           ((u8)45) //仅适用于 SD 卡 
#define SD_ACMD46           	       ((u8)46) //仅适用于 SD 卡 
#define SD_ACMD47             	     ((u8)47) //仅适用于 SD 卡 
#define SD_ACMD49           	       ((u8)49) //仅适用于 SD 卡 
#define SD_ACMD48_SD_APP_GET_CER_RES1                 ((u8)48) //仅适用于 SD 卡 
#define SD_ACMD48_SD_APP_SECURE_WRITE_MKB             ((u8)48) //仅适用于 SD 卡 

/*-=支持的SD卡定义=-*/
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)

//SDIO相关参数定义
#define NULL 0
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF) //静态标志
#define SDIO_CMD0TIMEOUT                ((u32)0x00010000)	//CMD0超时 
#define SDIO_DATATIMEOUT                ((u32)0xFFFFFFFF)	//数据超时 
#define SDIO_FIFO_Address               ((u32)0x40018080) 

//卡状态 R1（OCR 寄存器） 
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000) //命令的参数超出卡的接受范围/地址超出范围
#define SD_OCR_ADDRESS_ERROR            ((u32)0x40000000) //没对齐的地址，同命令中使用的块长度不匹配
#define SD_OCR_BLOCK_LEN_ERR            ((u32)0x20000000) //卡不接受的块长度，或者传输的长度同块长度不匹配
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000) //擦除命令序列有错
#define SD_OCR_BAD_ERASE_PARAM          ((u32)0x08000000) //要擦除的“写模块”的选择无效
#define SD_OCR_WRITE_PROT_VIOLATION     ((u32)0x04000000) //对保护模块进行写操作。对写保护卡进行操作
//#define SD_OCR_CARD_IS_LOCKED           ((u32)0x02000000) //表明卡被主机加锁了
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000) //加锁解锁命令发生错误
#define SD_OCR_COM_CRC_FAILED           ((u32)0x00800000) //前一个命令的 CRC 检查错误
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000) //卡当前状态不接受的命令
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000) //内部 ECC 收到但是数据不对
#define SD_OCR_CC_ERROR                 ((u32)0x00100000) //内部的卡控制器错误
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000) //通用或者未知错误
#define SD_OCR_STREAM_READ_UNDERRUN     ((u32)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((u32)0x00020000) 
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000) //CSD 的只读部分同卡的内容不匹配，试图方向拷贝，或者永久写保护位
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000) //因为保护块，或者是写保护卡，只擦除了部分指定的地址。
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000) //命令已经执行，没使用内部 ECC
#define SD_OCR_ERASE_RESET              ((u32)0x00002000) //因为收到退出擦除序列命令，在执行之前，擦除序列被清除

#define SD_OCR_CURRENT_STATE_dis        ((u32)0x00001000) //断开连接状态(Disconnect State)
#define SD_OCR_CURRENT_STATE_prg        ((u32)0x00000E00) //编程状态(Programming State)
#define SD_OCR_CURRENT_STATE_rcv        ((u32)0x00000C00) //接收数据状态(Receive-data State)
#define SD_OCR_CURRENT_STATE_data       ((u32)0x00000A00) //发送数据状态(Sending-data State)
#define SD_OCR_CURRENT_STATE_tran       ((u32)0x00000800) //传输状态(Transfer State)
#define SD_OCR_CURRENT_STATE_stby       ((u32)0x00000600) //待机状态(Stand-by State)
#define SD_OCR_CURRENT_STATE_ident      ((u32)0x00000400) //识别状态(Identification State)
#define SD_OCR_CURRENT_STATE_ready      ((u32)0x00000200) //准备状态(Ready State)
#define SD_OCR_CURRENT_STATE_idle       ((u32)0x00000000) //空闲状态(Idle State)
#define SD_OCR_READY_FOR_DATA           ((u32)0x00000100) //总线上表示缓存空的状态，即可以接受数据
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008) //认证过程序列发生错误
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008) //错误位

//R6 响应掩码
//参考stm32手册SD R6响应
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000) //通用报错
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000) //非法命令
#define SD_R6_COM_CRC_FAILED            ((u32)0x00008000) //端口CRC失败


#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)//stm32 主机供电3.6v  具体查看OCR寄存器说明
#define SD_HIGH_CAPACITY                ((u32)0x40000000)//[30] CCS:1  大容量SD卡
#define SD_STD_CAPACITY                 ((u32)0x00000000)//[30] CCS:0  低容量SD卡
#define SD_CHECK_PATTERN                ((u32)0x000001AA)//[19:16]VHS(支持电压):0x1(0001b)
																												 //[15:8]检测模式:0xAA(10101010b)
#define SD_VOLTAGE_WINDOW_MMC           ((u32)0x80FF8000) //电压窗口MMC
#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF) //电压试验 ACMD41计数 超过则电压无效
#define SD_ALLZERO                      ((u32)0x00000000) //无报错

#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000) //支持宽总线
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000) //SD单总线支持
#define SD_CARD_LOCKED                  ((u32)0x02000000) //SD卡锁定状态
#define SD_CARD_PROGRAMMING             ((u32)0x00000007) //SD卡编程状态
#define SD_CARD_RECEIVING               ((u32)0x00000006) //SD卡接收状态
#define SD_DATATIMEOUT                  ((u32)0xFFFFFFFF) //数据最大时间 
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((u32)0x01FFFFFF) //最大接收长度

#define SD_HALFFIFO                     ((u32)0x00000008) //半FIFO
#define SD_HALFFIFOBYTES                ((u32)0x00000020) //半FIFO字节

//支持的命令类  
#define SD_CCCC_LOCK_UNLOCK             ((u32)0x00000080)
#define SD_CCCC_WRITE_PROT              ((u32)0x00000040)
#define SD_CCCC_ERASE                   ((u32)0x00000020)
																	 
//CMD8指令
#define SDIO_SEND_IF_COND               ((u32)0x00000008)

/*=================================================================================*/

SD_Error SD_Init(void);//初始化SD卡
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt); 	//读SD卡,fatfs/usb调用
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);	//写SD卡,fatfs/usb调用


SD_Error SD_PowerON(void);//SD卡上电
SD_Error SD_PowerOFF(void);//SD卡关电
SD_Error SD_InitializeCards(void);//初始化所有的SD卡
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);//获取SD卡信息
SD_Error SD_SelectDeselect(u32 addr);//选择SD卡
SD_Error FindSCR(u16 rca,u32 *pscr);//查找SD卡的SCR寄存器值
SD_Error SD_EnableWideBusOperation(u32 wmode);//设置总线宽度
SD_Error SD_SetDeviceMode(u32 Mode);//设置SD卡工作模式
SD_Error SD_SendStatus(uint32_t *pcardstatus);//读取当前卡状态
SDCardState SD_GetState(void);//返回SD卡的状态

/*SDIO相关*/
void SDIO_Clock_Set(u8 clkdiv);//SDIO时钟设置
SD_Error SDEnWideBus(u8 enx);//SDIO使能宽总线模式
u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes);
void SD_DMA_Config(u32*mbuf,u32 bufsize,u32 DMA_DIR);//配置SDIO DMA


SD_Error SD_ReadBlock(u8 *buf,long long addr,u16 blksize); //SD卡读取一个块  
SD_Error SD_ReadMultiBlocks(u8 *buf,long long  addr,u16 blksize,u32 nblks); //SD卡读取多个块  
SD_Error SD_WriteBlock(u8 *buf,long long addr,  u16 blksize);	//SD卡写一个块
SD_Error SD_WriteMultiBlocks(u8 *buf,long long addr,u16 blksize,u32 nblks); //SD卡写多个块
SD_Error SD_ProcessIRQSrc(void);//SDIO中断处理函数
SD_Error IsCardProgramming(u8 *pstatus);//检查卡是否正在执行写操作



SD_Error Cmd0Error(void);												//检查CMD0的执行状态
SD_Error CmdResp1Error(u8 cmd);									//检查R1响应的错误状态
SD_Error CmdResp2Error(void);										//检查R2响应的错误状态
SD_Error CmdResp3Error(void);										//检查R3响应的错误状态
SD_Error CmdResp6Error(u8 cmd,u16*prca);				//检查R6响应的错误状态
SD_Error CmdResp7Error(void);										//检查R7响应的错误状态







#endif









