/******************************************************************************************************** 
*                             
*                                    Address
*  
*                    (c) Copyright 20xx, Company Name, City, State
*                               All Rights Reserved
*
*
* FileName       : HostCom.h
* Version        : V2.0
* Programmer(s)  : zhaojun_xf
* Parameters     : LPC1700 12.000MHz
* DebugTools     : Realview MDK V4.72  JLINK V8(SWD)
* Description    : 
*
********************************************************************************************************/

#ifndef __COMMUNICATION0_H 
#define __COMMUNICATION0_H


#define UART0_RX_BUFLEN  200
#define UART0_TX_BUFLEN	 200

// 功能函数ID号
#define		IDSETLIFTPARA    0xA0
#define		IDGETLIFTPARA    0xA1
#define		IDSETLIFTRATE    0xA2
#define		IDGETLIFTRATE    0xA3
#define		IDREMOTECTRL     0xA4
#define		IDTIMEROPEN      0xA5
#define		IDCIRCLEOPEN     0xA6
#define		IDGUASHI         0xA7
#define		IDJIEGUA         0xA8
#define		IDGETRECORDNUM   0xA9
#define		IDGETRECORD      0xAA
#define		IDGETRECORDACK   0xAB
#define		IDREMOVERECORD   0xAC
#define		IDRESTORE        0xAD
#define		IDCOMMTEST       0xC0	
#define		IDUPDATEPROGRAM  0x52	



extern u8 recordRemoveFlag;
extern u8 liftRecoverFlag;
extern u8 liftUpdateFlag;


__packed typedef struct {
	u8   len;
	u8   count;
	u8   buf[UART0_RX_BUFLEN];
} UART0_RX;

typedef struct {
	u8   len;
	u8   count;
	u8   buf[UART0_TX_BUFLEN];
} UART0_TX;

/*----------------- 设置梯控参数(0xA0) -----------------*/
__packed typedef struct {
	u8   liftAddr;			// 梯控地址
	u8   liftSN;			// 梯控编号
	u8   startFloor;
	u8   projPswd[2];		// 项目密码
	u8   actiondly[2];
	u8   minDays;			// 用户卡提前日期提醒
	u8   minMoney;			// 用户卡最小金额提醒	
	u8   useMask;			// bit0-修改扇区；bit1-是否修改日期；bit2-是否修改扣费状态；bit4-是否扣费
	u8   time[6];
	u8   sector[3];
	u8   asciiValue;
} Comm0_Rx_SetLiftCtrlPara_Struct;

__packed typedef struct {
	u8 reserve[12];			// 预留未用(12) 
} Comm0_Tx_SetLiftCtrlPara_Struct;


/*----------------- 读取梯控参数(0xA1) -----------------*/
__packed typedef struct {
	u8 reserve[12];			// 预留未用(12) 
} Comm0_Rx_GetLiftCtrlPara_Struct;

__packed typedef struct {
	u8   liftAddr;			// 梯控地址
	u8   liftSN;			// 梯控编号
	u8   startFloor;
	u8   projPswd[2];		// 项目密码
	u8   actiondly[2];
	u8   minDays;			// 用户卡提前日期提醒
	u8   minMoney;			// 用户卡最小金额提醒	
	u8   time[6];
	u8   sector[3];
	u8   asciiValue;
	u8   version[3];
} Comm0_Tx_GetLiftCtrlPara_Struct;


/*----------------- 设置梯控费率(0xA2) -----------------*/
__packed typedef struct {
	u8   select;			// 8个楼层段选择掩码位 	
	RateStruct rateStruct[8];
} Comm0_Rx_SetLiftCtrlRate_Struct;

__packed typedef struct {
	u8 reserve[12];				// 预留未用(12)
} Comm0_Tx_SetLiftCtrlRate_Struct;


/*----------------- 读取梯控费率(0xA3) -----------------*/
__packed typedef struct {
	u8   reserve[12];			// 预留未用(12)
} Comm0_Rx_GetLiftCtrlRate_Struct;

__packed typedef struct {
	u8   rate[40];
} Comm0_Tx_GetLiftCtrlRate_Struct;


/*------------------- 远程控制(0xA4) -------------------*/
__packed typedef struct {
	u8   funSelect;
	u8   floorSelect[5];
	u8   calldly[2];	
	u8   actiondly[2];	
	u8   reserve[2];			// 预留未用(2)
} Comm0_Rx_RemoteCtrl_Struct;

__packed typedef struct {
	u8   reserve[12];			// 预留未用(12) 
} Comm0_Tx_RemoteCtrl_Struct;


/*------------------- 计时开放(0xA5) -------------------*/
__packed typedef struct {
	u8   useMask;
	u8   floorSelect[5];
	u8   time[3];
	u8   reserve[3];			// 预留未用(3) 
} Comm0_Rx_TimerOpenCtrl_Struct;

__packed typedef struct {
	u8   reserve[12];			// 预留未用(12) 
} Comm0_Tx_TimerOpenCtrl_Struct;


/*------------------- 循环开放(0xA6) -------------------*/
__packed typedef struct {
	u8   useMask;
	u8   floorSelect[5];
	u8   startDate[3];		// 年 月 日
	u8   endDate[3];		// 年 月 日
	u8   weekLimit;
	TimeStruct timeSlot[3]; 
} Comm0_Rx_CircleOpenCtrl_Struct;

__packed typedef struct {
	u8 reserve[12];			//预留未用(12) 
} Comm0_Tx_CircleOpenCtrl_Struct;


/*------------------- 挂失卡(0xA7) -------------------*/
__packed typedef struct {
	u8   num;				// 挂失的卡数量
	u8   sn[32][4];
} Comm0_Rx_Guashi_Struct;

__packed typedef struct {
	u8 reserve[12];			//预留未用(12) 
} Comm0_Tx_Guashi_Struct;


/*------------------- 解挂卡(0xA8) -------------------*/
__packed typedef struct {
	u8   num;				// 解挂的卡数量
	u8   sn[32][4];
} Comm0_Rx_Jiegua_Struct;

__packed typedef struct {
	u8 reserve[12];			// 预留未用(12) 
} Comm0_Tx_Jiegua_Struct;


/*------------------ 获取刷卡记录条数(0xA9)-----------------*/
__packed typedef struct {
	Date_Struct   startDate;		// 年 月 日
	Date_Struct   endDate;		// 年 月 日
	u8   reserve[6];		// 预留未用(6)
} Comm0_Rx_GetRecordNum_Struct;

__packed typedef struct {
	u16  recordnum;			// 记录条数
	u8   reserve[10];		// 预留未用(10)
} Comm0_Tx_GetRecordNum_Struct;


/*------------------- 读取刷卡记录(0xAA) ----------------*/
__packed typedef struct {
	u8   reserve[12];		// 预留未用(12)
} Comm0_Rx_GetRecord_Struct;

__packed typedef struct {
	DealRecord_Struct	record;
} Comm0_Tx_GetRecord_Struct;


/*---------------- 读取刷卡记录应答(0xAB) ----------------*/
__packed typedef struct {
	u16   dealNum;			// 刷卡流水号
	u8   reserve[10];		// 预留未用(10)	
} Comm0_Rx_GetRecordAck_Struct;

__packed typedef struct {
	u8   reserve[12];		// 预留未用(12)
} Comm0_Tx_GetRecordAck_Struct;


/*------------------- 删除刷卡记录(0xAC) ----------------*/
__packed typedef struct {
	u8   reserve[12];		// 预留未用(12)
} Comm0_Rx_RemoveRecord_Struct;

__packed typedef struct {
	u8   reserve[12];		// 预留未用(12)
} Comm0_Tx_RemoveRecord_Struct;


/*------------------- 恢复出厂设置(0xAD) ----------------*/
__packed typedef struct {
	u8   reserve[12];		// 预留未用(12)
} Comm0_Rx_Restore_Struct;

__packed typedef struct {
	u8   reserve[12];		// 预留未用(12)
} Comm0_Tx_Restore_Struct;


/*------------------- 串口测试(0xC0) ----------------*/
__packed typedef struct {
	u8   funselect;
	u8   floorArray[5];		
	u8   reserve[6];			// 预留未用(6)
} Comm0_Rx_Test_Struct;

__packed typedef struct {
	u8   rlyArray[5];
	u8   reserve[7];			// 预留未用(7)
} Comm0_Tx_Test_Struct;



/*------------------- 开始更新程序(0x52) ----------------*/
__packed typedef struct {
	u8   flag;
	u8   reserve[3];			// 预留未用(3)
} Comm_Rx_UpdateProgram_Struct;

__packed typedef struct {
	u8 okFlag;			/* 成功标志(1)         */
	u8 reserve[2];			// 预留未用(12)
} Comm0_Tx_UpdateProgram_Struct;






/*------------------- 接收的数据 ----------------*/
typedef struct {
	u8 deviceNum;		/* 设备机号            */
	u8 functionNum;		/* 功能号              */
	
	union
	{
		u8		buf[200];
		Comm0_Rx_SetLiftCtrlPara_Struct   setLiftCtrlPara;	// 0xa0
		Comm0_Rx_GetLiftCtrlPara_Struct   getLiftCtrlPara;	// 0xa1
		Comm0_Rx_SetLiftCtrlRate_Struct   setLiftCtrlRate;	// 0xa2
		Comm0_Rx_GetLiftCtrlRate_Struct   getLiftCtrlRate;	// 0xa3
		Comm0_Rx_RemoteCtrl_Struct        remoteCtrl;		// 0xa4
		Comm0_Rx_TimerOpenCtrl_Struct     timerOpenCtrl;	// 0xa5
		Comm0_Rx_CircleOpenCtrl_Struct    circleOpenCtrl;	// 0xa6
		Comm0_Rx_Guashi_Struct            guashi;			// 0xa7
		Comm0_Rx_Jiegua_Struct            jiegua;			// 0xa8
		Comm0_Rx_GetRecordNum_Struct      getRecordNum;		// 0xa9
		Comm0_Rx_GetRecord_Struct         getRecord;			// 0xaa
		Comm0_Rx_GetRecordAck_Struct      getRecordAck;		// 0xab
		Comm0_Rx_RemoveRecord_Struct      removeRecord;		// 0xac
		Comm0_Rx_Restore_Struct           restore;			// 0xad
		Comm0_Rx_Test_Struct			  test;				// 0xc0	
	}Union;
} Comm0_Rx_Struct;


/*------------------- 发送的数据 ----------------*/
typedef struct {
	u8 functionNum;		/* 功能号              */
	u8 deviceNum;		/* 设备机号            */
	
	union
	{
		u8		buf[200];
		Comm0_Tx_SetLiftCtrlPara_Struct   setLiftCtrlPara;	// 0xa0
		Comm0_Tx_GetLiftCtrlPara_Struct   getLiftCtrlPara;	// 0xa1
		Comm0_Tx_SetLiftCtrlRate_Struct   setLiftCtrlRate;	// 0xa2
		Comm0_Tx_GetLiftCtrlRate_Struct   getLiftCtrlRate;	// 0xa3
		Comm0_Tx_RemoteCtrl_Struct        remoteCtrl;		// 0xa4
		Comm0_Tx_TimerOpenCtrl_Struct     timerOpenCtrl;	// 0xa5
		Comm0_Tx_CircleOpenCtrl_Struct    circleOpenCtrl;	// 0xa6
		Comm0_Tx_Guashi_Struct            guashi;			// 0xa7
		Comm0_Tx_Jiegua_Struct            jiegua;			// 0xa8
		Comm0_Tx_GetRecordNum_Struct      getRecordNum;		// 0xa9
		Comm0_Tx_GetRecord_Struct         getRecord;		// 0xaa
		Comm0_Tx_GetRecordAck_Struct      getRecordAck;		// 0xab
		Comm0_Tx_RemoveRecord_Struct      removeRecord;		// 0xac
		Comm0_Tx_Restore_Struct           restore;			// 0xad
		Comm0_Tx_Test_Struct              test;				// 0xc0
		Comm0_Tx_UpdateProgram_Struct     updateProgram;		// 0x52
	} Union;
} Comm0_Tx_Struct;












/*------------------- 串口控制结构体 ----------------*/
typedef struct {
	u8 functionId;
	void (*Function)(void);
} Comm0_Control_Struct;


void comm0_Analyse(void *rxBuff, u16 *rxLength, void *txBuff, u16 *txLength);

#endif

/********************************************************************************************************
* End Of File         
********************************************************************************************************/
