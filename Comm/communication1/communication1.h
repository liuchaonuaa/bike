/******************************************************************************************************** 
*                             
*                                    Address
*  
*                    (c) Copyright 20xx, Company Name, City, State
*                               All Rights Reserved
*
*
* FileName       : communication2.h
* Version        : V2.0
* Programmer(s)  : zhaojun_xf
* Parameters     : LPC1700 12.000MHz
* DebugTools     : Realview MDK V4.72  JLINK V8(SWD)
* Description    : 
*
********************************************************************************************************/

#ifndef __COMMUNICATION1_H 
#define __COMMUNICATION1_H


#define UART1_RX_BUFLEN  160
#define UART1_TX_BUFLEN	 160

// 功能函数ID号
#define  IDKEYCODE      	 0x40	
#define  IDUSRCARDINFO       0x41	
#define  IDCFGCARDINFO       0x42	
#define  IDGUASHICARDINFO    0x43	
#define  IDJIEGUACARDINFO    0x44	
#define  IDSUPERUSRCARDINFO  0x45	
#define  IDOPENSLOTCARDINFO  0x46
#define  IDRATECARDINFO      0x47
#define  IDTIMECFGCARDINFO   0x48	
#define  IDCHECKCARDINFO     0x49
#define  IDCODECFGCARDINFO   0x4a

__packed typedef struct {
	u8   len;
	u8   count;
	u8   buf[UART1_RX_BUFLEN];
} UART1_RX;

typedef struct {
	u8   len;
	u8   count;
	u8   buf[UART1_TX_BUFLEN];
} UART1_TX;


/*------------------- 串口测试(0xC0) ----------------*/
__packed typedef struct {
	u8   reserve[12];			// 预留未用(12)
} Comm1_Rx_Test_Struct;

__packed typedef struct {
	u8   rlyArray[5];
	u8   reserve[12];			// 预留未用(12)
} Comm1_Tx_Test_Struct;


/*-------------------按键密码处理(0x40) ----------------*/
__packed typedef struct {
	u8 floorselect;
	u8 keycode[3];
} Comm1_Rx_KeyCode_Struct;


__packed typedef struct {
	u8 reserve[4];
} Comm1_Tx_KeyCode_Struct;


/*------------------- 用户卡信息(0x41) ----------------*/
__packed typedef struct {
    u8   cardSN[4];
	
	u8   cardType;					
	u8   projPswd[2];		
	u8   funSelect;			
	u8   liftAddr[8];		
	u8   useMask;
	u8   reserved1[3];	

	u8   floorSelect[8][4];
	
	u8   startDate[3];		
	u8   endDate[3];		
	u8   weekLimit;
	u8   countLimit[2];
	u8   money[2];
	u8   sn[4];
	u8   reserved2;
	
	TimeStruct timeSlot[3];
	u8   reserved3[4];		
} Comm1_Rx_UsrCardInfo_Struct;


__packed typedef struct {
	u8 changeFlag;			// bit0：取消挂失标记；bit1：次数减一
	u8 reserve[3];
} Comm1_Tx_UsrCardInfo_Struct;


/*------------------- 设置卡信息(0x42) ----------------*/
__packed typedef struct {
    u8   cardSN[4];

    u8   cardType;			
	u8   useMask;			
	u8   count;				
	u8   time[6];
	u8   reserved1[7];
	
	u8   liftAddr;			
	u8   projPswd[2];	
	u8   actiondly[2];
	u8   calldly[2];
	u8   sector[3];
	u8   reserved2[6];
	
	
	u8   startFloor;
	u8   liftSN;
	u8   minMoney;		
	u8   minDays; 
	u8   reserved3[12];
} Comm1_Rx_CfgCardInfo_Struct;


__packed typedef struct {
	u8 changeFlag;			
	u8 reserve[3];
} Comm1_Tx_CfgCardInfo_Struct;


/*------------------- 挂失卡信息(0x43) ----------------*/
__packed typedef struct {
    u8   cardSN[4];    
    
	u8   cardType;				
	u8   projPswd[2];		
	u8   num;				
	u8   time[6];
	u8   reserved1[6];
	
	u8   sn[32][4]; 
} Comm1_Rx_GuashiCardInfo_Struct;

__packed typedef struct {
	u8 changeFlag;			// bit0：次数减一
	u8 reserve[3];
} Comm1_Tx_GuashiCardInfo_Struct;


/*------------------- 解挂卡信息(0x44) ----------------*/
__packed typedef struct {
    u8   cardSN[4];    
    
	u8   cardType;				
	u8   projPswd[2];		
	u8   num;				
	u8   time[6];	
	u8   reserved1[6];
	
	u8   sn[32][4]; 
} Comm1_Rx_JieguaCardInfo_Struct;

__packed typedef struct {
	u8 changeFlag;			// bit0：次数减一
	u8 reserve[3];
} Comm1_Tx_JieguaCardInfo_Struct;



/*------------------- 超级用户卡信息(0x45) ----------------*/
__packed typedef struct {
    u8   cardSN[4];    
    
	u8   cardType;						
	u8   projPswd[2];		
	u8   isForAll;
	u8   liftAddr[4];
	u8   useMask;
	u8   reserved1[7];
	
	u8   floorSelect[4][8];
	
	u8   startDate[3];	
	u8   endDate[3];	
	u8   weekLimit;
	u8   countLimit[2];
	u8   reserved2[7];

	TimeStruct timeSlot[3]; 
	u8   reserved3[4];
} Comm1_Rx_SuperusrCardInfo_Struct;


__packed typedef struct {
	u8 changeFlag;			// bit0：次数减一
	u8 reserve[3];
} Comm1_Tx_SuperusrCardInfo_Struct;



/*------------------- 开放时段卡信息(0x46) ----------------*/
__packed typedef struct {
    u8   cardSN[4];    

	u8   cardType;			// 卡类型			
	u8   projPswd[2];		// 项目密码
	u8   modeSelect;			// 3种功能 计时开放 循环开放  永久开放
	u8   useMask;			// bit0：计时开放；bit2 bit1：循环开放的三种方式；bit3：启用永久开放  bit4：永久开放（开放）  
	u8   time[6];			// 年月日 时分秒
	u8   reserved1[5];
	
	u8   floorSelect[5];
	u8   circleUseMask;
	u8   circleStartDate[3];	// 循环开放  起始日期
	u8   circleEndDate[3];		// 循环开放  截止日期
	u8   circleWeekLimit;		
	u8   reserved2[3];

	TimeStruct circleTimeSlot[3]; 
	u8 timerTime[3];			// 计时开放 时 分 秒 
	u8   reserved3;
} Comm1_Rx_OpenSlotCardInfo_Struct;

__packed typedef struct {
	u8 changeFlag;			// bit0：次数减一
	u8 reserve[3];
} Comm1_Tx_OpenSlotCardInfo_Struct;



/*------------------- 费率设置卡信息(0x47) ----------------*/
__packed typedef struct {
    u8   cardSN[4];    

	u8   cardType;			// 卡类型			
	u8   projPswd[2];		// 项目密码
	u8   select;			// 8个楼层段选择掩码位 
	u8   time[6];			// 年月日 时分秒
	u8   reserved1[6];
	
	RateStruct rateStruct[8];
} Comm1_Rx_RateCardInfo_Struct;

__packed typedef struct {
	u8 changeFlag;			// bit0：次数减一
	u8 reserve[3];
} Comm1_Tx_RateCardInfo_Struct;


/*------------------- 时间设置卡信息(0x48) ----------------*/
__packed typedef struct {
    u8   cardSN[4];    

	u8   cardType;			// 卡类型			
	u8   projPswd[2];		// 项目密码
	u8   timeSet[6];		// 需要设置的时间
	u8   reserved1[7];
} Comm1_Rx_TimeCfgCardInfo_Struct;

__packed typedef struct {
	u8 changeFlag;			// bit0：次数减一
	u8 reserve[3];
} Comm1_Tx_TimeCfgCardInfo_Struct;



/*------------------- 查看卡信息(0x49) ----------------*/
__packed typedef struct {
    u8   cardSN[4];    

	u8   cardType;			// 卡类型			
	u8   count;				// 有效次数
	u8   time[6];
	u8   reserved1[8];
} Comm1_Rx_CheckCardInfo_Struct;

__packed typedef struct {
	u8   cardType;			// 卡类型			
	u8   count;				// 有效次数
	u8   time[6];
	u8   reserved1[8];
	
	u8   liftAddr;			// 梯控地址
	u8   projPswd[2];		// 项目密码
	u8   actiondly[2];
	u8   calldly[2];
	u8   sector[3];
	u8   reserved2[6];
	
	u8   startFloor;
	u8   liftSN;
	u8   minMoney;			// 用户卡最小金额提醒	
	u8   minDays;			// 用户卡提前日期提醒
	u8   reserved3[12];
} Comm1_Tx_CheckCardInfo_Struct;


/*------------------- 用户卡信息(0x41) ----------------*/
__packed typedef struct {
	u8 code[5][3];	
	u8 reserved;
} KeyCode_Struct;


__packed typedef struct {
    u8   cardSN[4];
	
	u8   cardType;					
	u8   projPswd[2];				
	u8   codeselect[5];		
	u8   time[6];
	u8   reserved1[2];	

	KeyCode_Struct  keycode[8];	
} Comm1_Rx_CodecfgCardInfo_Struct;


__packed typedef struct {
	u8 changeFlag;			// bit0：取消挂失标记；bit1：次数减一
	u8 reserve[3];
} Comm1_Tx_CodecfgCardInfo_Struct;





/*------------------- 接收的数据 ----------------*/
typedef struct {
	u8 flag;
	u8 functionNum;		/* 功能号              */
	
	union
	{
		u8  buf[160];
		Comm1_Rx_KeyCode_Struct           r_keycodeInfo;
		Comm1_Rx_UsrCardInfo_Struct       r_usrcardInfo;
		Comm1_Rx_CfgCardInfo_Struct       r_cfgcardInfo; 
		Comm1_Rx_GuashiCardInfo_Struct    r_guashicardInfo;
		Comm1_Rx_GuashiCardInfo_Struct    r_jieguacardInfo;	
		Comm1_Rx_SuperusrCardInfo_Struct  r_superusrcardInfo;
		Comm1_Rx_OpenSlotCardInfo_Struct  r_openslotcardInfo;  
		Comm1_Rx_RateCardInfo_Struct      r_ratecardInfo;
		Comm1_Rx_TimeCfgCardInfo_Struct   r_timecfgcardInfo;
		Comm1_Rx_CheckCardInfo_Struct     r_checkcardInfo; 
		Comm1_Rx_CodecfgCardInfo_Struct   r_codecfgcardInfo;
	}Union;
} Comm1_Rx_Struct;


/*------------------- 发送的数据 ----------------*/
typedef struct {
	u8 flag;
	u8 functionNum;		/* 功能号              */
	
	union
	{
		u8  buf[160];
		Comm1_Tx_UsrCardInfo_Struct       s_usrcardInfo;
		Comm1_Tx_CfgCardInfo_Struct       s_cfgcardInfo;
		Comm1_Tx_GuashiCardInfo_Struct    s_guashicardInfo;
		Comm1_Tx_GuashiCardInfo_Struct    s_jieguacardInfo;			
		Comm1_Tx_SuperusrCardInfo_Struct  s_superusrcardInfo;
		Comm1_Tx_OpenSlotCardInfo_Struct  s_openslotcardInfo;  
		Comm1_Tx_RateCardInfo_Struct      s_ratecardInfo;
		Comm1_Tx_TimeCfgCardInfo_Struct   s_timecfgcardInfo;
		Comm1_Tx_CheckCardInfo_Struct     s_checkcardInfo; 
	} Union;
} Comm1_Tx_Struct;


/*------------------- 串口控制结构体 ----------------*/
typedef struct {
	u8 functionId;
	void (*Function)(void);
} Comm1_Control_Struct;


u8 usrcardCommDeal(void);

void comm1_Analyse(void *rxBuff, u16 *rxLength, void *txBuff, u16 *txLength);

#endif

/********************************************************************************************************
* End Of File         
********************************************************************************************************/
