/******************************************************************************************************** 
*                             
*                                    Address
*  
*                    (c) Copyright 20xx, Company Name, City, State
*                               All Rights Reserved
*
*
* FileName       : Communication.c
* Version        : V1.0
* Programmer(s)  : liuchao
* Date           : 2014/12
* Parameters     : LPC1700 12.000MHz
* DebugTools     : Realview MDK V4.72  JLINK V8(SWD)
* Description    :
*
*
********************************************************************************************************/

#include "../UserCode/source/config.h"    			 // ͷ�ļ�

static Comm0_Rx_Struct	*comm0_Rx;
static Comm0_Tx_Struct	*comm0_Tx;

static	u16 *comm0_lenRx;
static	u16 *comm0_lenTx;

static void comm0_SetLiftCtrlPara(void);	
static void comm0_GetLiftCtrlPara(void);	
static void comm0_SetLiftCtrlRate(void);	
static void comm0_GetLiftCtrlRate(void);	
static void comm0_RemoteCtrl(void);		
static void comm0_TimerOpenCtrl(void);	
static void comm0_CircleOpenCtrl(void);	
static void comm0_Guashi(void);			
static void comm0_Jiegua(void);			
static void comm0_GetRecordNum(void);	
static void comm0_GetRecord(void);		
static void comm0_GetRecordAck(void);	
static void comm0_RemoveRecord(void);	
static void comm0_Restore(void);		
static void comm0_Test(void);		
static void comm0_UpdateProgram(void);		


static Comm0_Control_Struct comm0_Control[] = {
	{ IDSETLIFTPARA,   comm0_SetLiftCtrlPara },
	{ IDGETLIFTPARA,   comm0_GetLiftCtrlPara },
	{ IDSETLIFTRATE,   comm0_SetLiftCtrlRate },
	{ IDGETLIFTRATE,   comm0_GetLiftCtrlRate },
	{ IDREMOTECTRL,    comm0_RemoteCtrl      },
	{ IDTIMEROPEN,     comm0_TimerOpenCtrl   },
	{ IDCIRCLEOPEN,    comm0_CircleOpenCtrl  },
	{ IDGUASHI,        comm0_Guashi          },
};

#define COMM0_MAX_FUNC   (sizeof(comm0_Control) / sizeof(comm0_Control[0]))


u8 recordRemoveFlag = 0;
u8 liftRecoverFlag = 0;
u8 liftUpdateFlag = 0;




/*
 ****************************************************************************************
 * �������ƣ�comm0_SetLiftCtrlPara	
 * ���������������ݿ��豸����	
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_SetLiftCtrlPara(void)	// 0xA0
{ 	
	LiftCtrlPara para;
	u16 temp;
	
	memcpy(&(para.flag), &(lift_ctrl_para.flag), sizeof(LiftCtrlPara));

	para.liftAddr    = comm0_Rx->Union.setLiftCtrlPara.liftAddr;
	para.liftSN      = comm0_Rx->Union.setLiftCtrlPara.liftSN;
	para.startFloor  = comm0_Rx->Union.setLiftCtrlPara.startFloor;
	para.minMoney    = comm0_Rx->Union.setLiftCtrlPara.minMoney;
	para.minDays     = comm0_Rx->Union.setLiftCtrlPara.minDays;
	para.projPswd[0] = comm0_Rx->Union.setLiftCtrlPara.projPswd[0];
	para.projPswd[1] = comm0_Rx->Union.setLiftCtrlPara.projPswd[1];
	para.asciiValue  = comm0_Rx->Union.setLiftCtrlPara.asciiValue;
	
	temp = ((comm0_Rx->Union.setLiftCtrlPara.actiondly[0]) << 8) | (comm0_Rx->Union.setLiftCtrlPara.actiondly[1]);
		   
	if ((temp < 600) && (temp > 0))
		para.actionDly = temp;

	if (comm0_Rx->Union.setLiftCtrlPara.useMask & (1<<0))
	{
		memcpy(para.sector, comm0_Rx->Union.setLiftCtrlPara.sector, 3);
		liftCtrlSetPassword(para.sector);
	}

	if (comm0_Rx->Union.setLiftCtrlPara.useMask & (1<<4))
	{
		para.liftFlag |= 1<<0;	
	}
	else
	{
		para.liftFlag &= ~(1<<0);	
	}
	
	writeLiftCtrlPara(para);
	readLiftCtrlPara();	
	
	RTCSet.year	  = comm0_Rx->Union.setLiftCtrlPara.time[0]; 
	RTCSet.month  = comm0_Rx->Union.setLiftCtrlPara.time[1];   
	RTCSet.day    = comm0_Rx->Union.setLiftCtrlPara.time[2];   
	RTCSet.week   = GetWeek(RTCSet.year, RTCSet.month, RTCSet.day);    
	RTCSet.hour   = comm0_Rx->Union.setLiftCtrlPara.time[3];   
	RTCSet.minute = comm0_Rx->Union.setLiftCtrlPara.time[4];   
	RTCSet.second = comm0_Rx->Union.setLiftCtrlPara.time[5];    
	
	RTCSetTime(RTCSet);	
	
	memset(comm0_Tx->Union.setLiftCtrlPara.reserve, 0x00, 12);
	
 	*comm0_lenTx = sizeof(Comm0_Tx_SetLiftCtrlPara_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm_GetSerialNum	
 * ������������ȡ�ݿ��豸����
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_GetLiftCtrlPara(void)	// 0xA1
{
	CurrentTime = RTCGetTime();

	comm0_Tx->Union.getLiftCtrlPara.liftAddr     = lift_ctrl_para.liftAddr;
	comm0_Tx->Union.getLiftCtrlPara.liftSN       = lift_ctrl_para.liftSN;
	comm0_Tx->Union.getLiftCtrlPara.startFloor   = lift_ctrl_para.startFloor;
	comm0_Tx->Union.getLiftCtrlPara.projPswd[0]  = lift_ctrl_para.projPswd[0];
	comm0_Tx->Union.getLiftCtrlPara.projPswd[1]  = lift_ctrl_para.projPswd[1];
	comm0_Tx->Union.getLiftCtrlPara.actiondly[0] = lift_ctrl_para.actionDly >> 8;
	comm0_Tx->Union.getLiftCtrlPara.actiondly[1] = lift_ctrl_para.actionDly & 0xff;
	comm0_Tx->Union.getLiftCtrlPara.minDays = lift_ctrl_para.minDays;
	comm0_Tx->Union.getLiftCtrlPara.minMoney = lift_ctrl_para.minMoney;

	memcpy(&(comm0_Tx->Union.getLiftCtrlPara.time[0]), &(CurrentTime.year), 3);
	memcpy(&(comm0_Tx->Union.getLiftCtrlPara.time[3]), &(CurrentTime.hour), 3);	
	memcpy(comm0_Tx->Union.getLiftCtrlPara.sector, lift_ctrl_para.sector, 3);	
	
	comm0_Tx->Union.getLiftCtrlPara.asciiValue = lift_ctrl_para.asciiValue;
	comm0_Tx->Union.getLiftCtrlPara.version[0] = SOFTVERTION/100;
	comm0_Tx->Union.getLiftCtrlPara.version[1] = SOFTVERTION%100/10;
	comm0_Tx->Union.getLiftCtrlPara.version[2] = SOFTVERTION%10;
	
	*comm0_lenTx = sizeof(Comm0_Tx_GetLiftCtrlPara_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_SetLiftCtrlRate	
 * ���������������ݿط���	
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_SetLiftCtrlRate(void)	// 0xA2	
{
	LiftCtrlPara para;
	u8 i, j;
	
	memcpy(&(para.flag), &(lift_ctrl_para.flag), sizeof(LiftCtrlPara));
	
	memset(para.rate, 0x02, sizeof(lift_ctrl_para.rate));	
	
	for (i = 0; i < 8; i++)
	{
		if (comm0_Rx->Union.setLiftCtrlRate.select & (1<<i))
		{
			if ((comm0_Rx->Union.setLiftCtrlRate.rateStruct[i].startFloor) && 
				(comm0_Rx->Union.setLiftCtrlRate.rateStruct[i].endFloor < 41) &&
				(comm0_Rx->Union.setLiftCtrlRate.rateStruct[i].startFloor < comm0_Rx->Union.setLiftCtrlRate.rateStruct[i].endFloor))
			{
				for (j = (comm0_Rx->Union.setLiftCtrlRate.rateStruct[i].startFloor)-1; j < (comm0_Rx->Union.setLiftCtrlRate.rateStruct[i].endFloor)-1; j++)
				{
					para.rate[j] = comm0_Rx->Union.setLiftCtrlRate.rateStruct[i].rateMoney;
				}
			}
		}
	}
	
	writeLiftCtrlPara(para);
	readLiftCtrlPara();
	
	memset(comm0_Tx->Union.setLiftCtrlRate.reserve, 0x00, 12);

	*comm0_lenTx = sizeof(Comm0_Tx_SetLiftCtrlRate_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_SetLiftCtrlRate
 * ������������ȡ�ݿط���
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_GetLiftCtrlRate(void)	// 0xA3
{	
	memcpy(comm0_Tx->Union.getLiftCtrlRate.rate, lift_ctrl_para.rate, sizeof(LiftCtrlPara));

	*comm0_lenTx = sizeof(Comm0_Tx_GetLiftCtrlRate_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_RemoteCtrl
 * ����������Զ�̿���
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_RemoteCtrl(void)	// 0xA4
{
	memcpy(&remotectrlpara, &(comm0_Rx->Union.remoteCtrl), sizeof(RemoteCtrlParaStruct));

	LiftRemoteCtrl(remotectrlpara.floorSelect, remotectrlpara.calldly,
				remotectrlpara.actiondly, remotectrlpara.funSelect);
		
	memset(comm0_Tx->Union.remoteCtrl.reserve, 0x00, 12);
	
	*comm0_lenTx = sizeof(Comm0_Tx_RemoteCtrl_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_TimerOpenCtrl	
 * ������������ʱ����
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_TimerOpenCtrl(void)	// 0xA5
{	
	if ((comm0_Rx->Union.timerOpenCtrl.useMask) & 0x01)
	{
		memcpy(&timeropenpara, &(comm0_Rx->Union.timerOpenCtrl), sizeof(TimerOpenParaStruct));
		LiftTimerOpenCtrl(timeropenpara.floorSelect, timeropenpara.time);
	}
	
	memset(comm0_Tx->Union.timerOpenCtrl.reserve, 0x00, 12);	
	
	*comm0_lenTx = sizeof(Comm0_Tx_TimerOpenCtrl_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_CircleOpenCtrl	
 * ����������ѭ������
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_CircleOpenCtrl(void)	// 0xA6
{
	u8 flag;
	
	CircleOpenCount = 0;
	
	if ((comm0_Rx->Union.circleOpenCtrl.useMask) & 0x80)
	{
		LiftControlCloseAll(CIRCLECTRL);

		memcpy(&circleopenPara, &(comm0_Rx->Union.circleOpenCtrl), sizeof(CircleOpenParaStruct));
		
		
		flag = isTimeEffective(circleopenPara.useMask, 
				circleopenPara.startDate, circleopenPara.endDate, 
				circleopenPara.weekLimit, circleopenPara.timeSlot[0].startTime);

		if (flag == 1)
			LiftControlOpenByArray(CIRCLECTRL, circleopenPara.floorSelect, FUNXUANCENG, 1000);
	}
	else
	{
		circleopenPara.useMask &= ~(1<<7);
		liftctrlflag &= ~(3 << (2*CIRCLECTRL));
		
		LiftControlCloseAll(CIRCLECTRL);
		memset(circleopenPara.floorSelect, 0x00, 5);
	}
	
	memset(comm0_Tx->Union.circleOpenCtrl.reserve, 0x00, 12);	
	
	*comm0_lenTx = sizeof(Comm0_Tx_CircleOpenCtrl_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_Guashi	
 * ������������ʧ
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_Guashi(void)	// 0xA7
{
	u8   sn[32][4];
	u8   sncmp[4] = {0x00, 0x00, 0x00, 0x00};
	u8   num;					// ��ҵĿ�����
	u8   i;
	
	num = comm0_Rx->Union.guashi.num;
	memcpy(sn[0], comm0_Rx->Union.guashi.sn[0], num * 4);

	for (i = 0; i < num; i++)
	{
		if (memcmp(sncmp, sn[i], 4) != 0)
			writeBlackCardInfo(sn[i], 0xaa);	// д0xaa ���ʧ���ſ�	
	}

	memset(comm0_Tx->Union.guashi.reserve, 0x00, 12);	
	
	*comm0_lenTx = sizeof(Comm0_Tx_Guashi_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_Jiegua
 * �������������
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 *****************************************************************************************
 */
static void comm0_Jiegua(void)	// 0xA8
{
	u8   sn[32][4];
	u8   sncmp[4] = {0x00, 0x00, 0x00, 0x00};
	u8   num;				// ��ҵĿ�����
	u8   i;
	
	num = comm0_Rx->Union.jiegua.num;
	memcpy(sn[0], comm0_Rx->Union.jiegua.sn[0], num * 4);

	for (i = 0; i < num; i++)
	{
		if (memcmp(sncmp, sn[i], 4) != 0)
			writeBlackCardInfo(sn[i], 0x55);	// д0x55 �������ſ�		
	}

	memset(comm0_Tx->Union.jiegua.reserve, 0x00, 12);	
	
	*comm0_lenTx = sizeof(Comm0_Tx_Jiegua_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_GetRecordNum
 * ������������ȡˢ����¼����
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_GetRecordNum(void)	// 0xA9
{
	Date_Struct begindate, enddate;
	u16 recordnum = 0, recordnumtemp = 0;
	
	memcpy(&begindate, &(comm0_Rx->Union.getRecordNum.startDate), 3);
	memcpy(&enddate, &(comm0_Rx->Union.getRecordNum.endDate), 3);
	
	dealRecord_GetRecordNum(&begindate, &enddate, &recordnum);
	
	// 16λ ���ֽ�����ֽ�˳��һ��
	recordnumtemp = ((recordnum & 0xff) << 8) + (recordnum>>8);
	
	comm0_Tx->Union.getRecordNum.recordnum = recordnumtemp;
	
	memset(comm0_Tx->Union.getRecordNum.reserve, 0x00, 10);	
	
	
	*comm0_lenTx = sizeof(Comm0_Tx_GetRecordNum_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_GetRecord
 * ������������ȡˢ����¼
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_GetRecord(void)	// 0xAA
{
	DealRecord_Struct record;

	dealRecord_ReadOne(&record);
	memcpy(&(comm0_Tx->Union.getRecord.record), &record, DEALRECORD_SIZE);

	*comm0_lenTx = sizeof(Comm0_Tx_GetRecord_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_GetRecordAck
 * ������������ȡˢ����¼Ӧ��
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_GetRecordAck(void)	// 0xAB
{
	u16 dealnum;
	
	dealnum = comm0_Rx->Union.getRecordAck.dealNum;
	dealRecord_ReadOneSuccess(dealnum);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_RemoveRecord
 * ����������ɾ��ˢ����¼
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_RemoveRecord(void)	// 0xAC
{
	// ˢ����¼��գ�������������ˢ����¼�� ��������  ���һ������������
	recordRemoveFlag = 1;
	
	memset(comm0_Tx->Union.removeRecord.reserve, 0x00, 12);	
	
	*comm0_lenTx = sizeof(Comm0_Tx_RemoveRecord_Struct);	
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_Restore
 * �����������ָ���������
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_Restore(void)	// 0xAD
{
	liftRecoverFlag = 1;
	
	memset(comm0_Tx->Union.restore.reserve, 0x00, 12);	
	
	// �ڰ��������
	*comm0_lenTx = sizeof(Comm0_Tx_Restore_Struct);	
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_Test		
 * �������������ڲ���
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_Test(void)	// 0xC0
{ 	
	LiftControlOpenByArray(NORMALCTRL, comm0_Rx->Union.test.floorArray, (comm0_Rx->Union.test.funselect) & 0x01, 1000);

	memcpy(comm0_Tx->Union.test.rlyArray, relayArrayValue, 5);
	
	*comm0_lenTx = sizeof(Comm0_Tx_Test_Struct);
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_UpdateProgram		
 * ������������ʼ���³���
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_UpdateProgram(void)	// 0x52
{ 	
	updateInform_JumpBoot();
 	
	comm0_Tx->Union.updateProgram.okFlag = 0x55;
 	*comm0_lenTx = sizeof(Comm0_Tx_UpdateProgram_Struct);
}






/*
 ****************************************************************************************
 * �������ƣ�comm0_FunctionHandle
 * ����������ͨѶ�������
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
static void comm0_FunctionHandle(void)
{
	u32 i;
	u16 LRC;
	
	for (i = 0; i < COMM0_MAX_FUNC; i ++)
	{
		/* Ѱ�ҹ��ܴ�����ȵĴ�����                  */
		if (comm0_Rx->functionNum == comm0_Control[i].functionId)
		{
			comm0_Control[i].Function();
			
			if (*comm0_lenTx > 0)
			{
				comm0_Tx->functionNum = comm0_Rx->functionNum;
				comm0_Tx->deviceNum	 = comm0_Rx->deviceNum;
				*comm0_lenTx += 2;
				LRC = check_LRC16(comm0_Tx, *comm0_lenTx);
				comm0_Tx->Union.buf[*comm0_lenTx -2] = (u8)(LRC>>8);
				comm0_Tx->Union.buf[*comm0_lenTx -1] = (u8)LRC;
				*comm0_lenTx += 2;
			}
			break;
		}
	}
}


/*
 ****************************************************************************************
 * �������ƣ�comm0_Analyse
 * ����������ͨѶЭ�����������
 * �����������
 * �����������
 * ���ز�������
 * ����˵����
 ****************************************************************************************
 */
void comm0_Analyse(void *rxBuff, u16 *rxLength, void *txBuff, u16 *txLength)
{
	u16 LRC;
	
	comm0_Rx = rxBuff;
	comm0_Tx = txBuff;

	comm0_lenRx = rxLength;
	comm0_lenTx = txLength;
	
	if (*comm0_lenRx >= 4)
	{
		LRC = ((comm0_Rx->Union.buf[*comm0_lenRx - 4] << 8) | comm0_Rx->Union.buf[*comm0_lenRx - 3]);
		if (LRC == check_LRC16(comm0_Rx, *comm0_lenRx - 2))
		{
			comm0_FunctionHandle();
		}
		else
		{
			uart0_Rx.count = 0;
		}
	}
}



/********************************************************************************************************
* End Of File                  
********************************************************************************************************/
