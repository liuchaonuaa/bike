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

#include "../UserCode/source/config.h"    			 // 头文件

static Comm1_Rx_Struct	*comm1_Rx;
static Comm1_Tx_Struct	*comm1_Tx;

static	u16 *comm1_lenRx;
static	u16 *comm1_lenTx;

u8 usrcardflag = 0;
u8 cfgcardflag = 0;
u8 superusrcardflag = 0;

static void comm1_KeycodeDeal(void);		
static void comm1_UsrCardDeal(void);		
static void comm1_CfgCardDeal(void);		
static void comm1_GuashiCardDeal(void);		
static void comm1_JieguaCardDeal(void);		
static void comm1_SuperusrCardDeal(void);
static void comm1_OpenslotCardDeal(void);		
static void comm1_RateCardDeal(void);		
static void comm1_TimecfgCardDeal(void);		
static void comm1_CheckCardDeal(void);		
static void comm1_CodecfgCardDeal(void);


static Comm1_Control_Struct comm1_Control[] = {
	{ IDKEYCODE,       	   comm1_KeycodeDeal       },
	{ IDUSRCARDINFO,       comm1_UsrCardDeal       },
	{ IDCFGCARDINFO,       comm1_CfgCardDeal       },
	{ IDGUASHICARDINFO,    comm1_GuashiCardDeal    },
	{ IDJIEGUACARDINFO,    comm1_JieguaCardDeal    },
	{ IDSUPERUSRCARDINFO,  comm1_SuperusrCardDeal  },
	{ IDOPENSLOTCARDINFO,  comm1_OpenslotCardDeal  },
	{ IDRATECARDINFO,      comm1_RateCardDeal      },
	{ IDTIMECFGCARDINFO,   comm1_TimecfgCardDeal   },
	{ IDCHECKCARDINFO,     comm1_CheckCardDeal     },
	{ IDCODECFGCARDINFO,   comm1_CodecfgCardDeal   },
};

#define COMM1_MAX_FUNC   (sizeof(comm1_Control) / sizeof(comm1_Control[0]))


Comm1_Rx_KeyCode_Struct           R_keycodeInfo;
Comm1_Rx_UsrCardInfo_Struct       R_usrcardInfo;
Comm1_Rx_CfgCardInfo_Struct       R_cfgcardInfo;
Comm1_Rx_GuashiCardInfo_Struct    R_guashicardInfo;
Comm1_Rx_JieguaCardInfo_Struct    R_jieguacardInfo;
Comm1_Rx_SuperusrCardInfo_Struct  R_superusrcardInfo;
Comm1_Rx_OpenSlotCardInfo_Struct  R_openslotcardInfo;
Comm1_Rx_RateCardInfo_Struct 	  R_ratecardInfo;
Comm1_Rx_TimeCfgCardInfo_Struct   R_timecfgcardInfo;
Comm1_Rx_CheckCardInfo_Struct     R_checkcardInfo;
Comm1_Rx_CodecfgCardInfo_Struct   R_codecfgcardInfo;


Comm1_Tx_CheckCardInfo_Struct     S_checkcardInfo;


u8 keycodeCommDeal(void)
{
	u8 floor;
	u8 keycode[3];
	u8 i;
	
	floor = R_keycodeInfo.floorselect;
	
	if (floor)
	{
		for (i = 0; i < 2; i++)
		{
			memcpy(keycode, liftctrl_keycode.keycode[(floor-1)*2 + i], 3);
			
			if (memcmp(keycode, R_keycodeInfo.keycode, 3) == 0)	
			{
				SFT_RlyGrp2_Open(floor-1, NORMALCTRL, 1000);	
				return 1;
			}			
		}
	}

	return 0;
}



u8 usrcardCommDeal(void)
{
	DealRecord_Struct record;
	u8  sncmp[4] = {0x00, 0x00, 0x00, 0x00};
	u8  i;
	u8  flag;
	u16 count;

	memcpy(SerialID, R_usrcardInfo.cardSN, 4);
	cardType = R_usrcardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}	
	
	if (R_usrcardInfo.useMask & (1<<MASK_GUASHI))
	{
		if (memcmp(sncmp, R_usrcardInfo.sn, 4) != 0)
			writeBlackCardInfo(R_usrcardInfo.sn, 0xaa);	
		
		usrcardflag |= (1<<0);
	}
		
	if (memcmp(lift_ctrl_para.projPswd, R_usrcardInfo.projPswd, 2) == 0)
	{
		for (i = 0; i < 8; i++)
		{
			if ((lift_ctrl_para.liftAddr)
				&& (lift_ctrl_para.liftAddr == R_usrcardInfo.liftAddr[i]))
			{
					
				flag = isTimeEffective(R_usrcardInfo.useMask, 
									R_usrcardInfo.startDate, R_usrcardInfo.endDate, 
									R_usrcardInfo.weekLimit, R_usrcardInfo.timeSlot[0].startTime);

				if (flag)
				{
					if (R_usrcardInfo.useMask & (1<<MASK_COUNT)) // 是否是次数限制
					{
						count = (R_usrcardInfo.countLimit[0] << 8) | R_usrcardInfo.countLimit[1];
						
						if (count == 0) // 次数用完了
						{
							DispError(ErrUsedout);		
							return 0;
						}

						usrcardflag |= (1<<1);
					}

					LiftControlOpenByFloor(NORMALCTRL, R_usrcardInfo.floorSelect[i], ((R_usrcardInfo.funSelect)>>i) & 0x01);	
					return 1;
				}
			}			
		}
		DispError(ErrAddrerr);
	}
	else
	{
		DispError(ErrYonghuma);	
	}
	
	return 0;	
}


u8 cfgcardCommDeal(void)
{
	DealRecord_Struct record;
	LiftCtrlPara para;
	u16 temp;

	memcpy(SerialID, R_cfgcardInfo.cardSN, 4);
	cardType = R_cfgcardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}		
	
	if (isIn12hours(R_cfgcardInfo.time))
	{
		if (R_cfgcardInfo.count == 0) // 次数用完了
		{
			DispError(ErrUsedout);		
			return 0;
		}
		
		cfgcardflag |= (1<<0);
		
		memcpy(&(para.flag), &(lift_ctrl_para.flag), sizeof(LiftCtrlPara));

		para.liftAddr    = R_cfgcardInfo.liftAddr;
		para.liftSN      = R_cfgcardInfo.liftSN;
		para.startFloor  = R_cfgcardInfo.startFloor;
		para.minMoney    = R_cfgcardInfo.minMoney;
		para.minDays     = R_cfgcardInfo.minDays;
		para.projPswd[0] = R_cfgcardInfo.projPswd[0];
		para.projPswd[1] = R_cfgcardInfo.projPswd[1];
		
		temp = (R_cfgcardInfo.actiondly[0] << 8) | R_cfgcardInfo.actiondly[1];
		if ((temp < 600) && (temp > 0))
			para.actionDly = temp;  

		temp = (R_cfgcardInfo.calldly[0] << 8) | R_cfgcardInfo.calldly[1];
		if ((temp < 600) && (temp > 0))
			para.callDly = temp;  

		if (R_cfgcardInfo.useMask & (1<<MASK_SECCHANGE))
		{
			memcpy(para.sector, R_cfgcardInfo.sector, 3);
			liftCtrlSetPassword(para.sector);
			cfgcardflag |= (1<<1);
		}

		if (R_cfgcardInfo.useMask & (1<<MASK_ALLOWPAY))
		{
			para.liftFlag |= 1<<0;	
		}
		else
		{
			para.liftFlag &= ~(1<<0);	
		}

		writeLiftCtrlPara(para);
		readLiftCtrlPara();		
	}
	else
	{
		DispError(ErrNotIntime);
	}
	
	return 0;
}



u8 guashicardCommDeal(void)
{
	DealRecord_Struct record;
	u8 sn[4] = {0x00, 0x00, 0x00, 0x00};
	u8 i;

	memcpy(SerialID, R_guashicardInfo.cardSN, 4);
	cardType = R_guashicardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}
	
	if (memcmp(lift_ctrl_para.projPswd, R_guashicardInfo.projPswd, 2) == 0)
	{
		if (isIn12hours(R_guashicardInfo.time))
		{
			for (i = 0; i < R_guashicardInfo.num; i++)
			{
				if (memcmp(sn, R_guashicardInfo.sn[i], 4) != 0)
				{
					writeBlackCardInfo(R_guashicardInfo.sn[i], 0xaa);
				}
			}
			
			return 1;
		}
		else
		{
			DispError(ErrNotIntime);
		}	
	}
	else
	{
		DispError(ErrYonghuma);
	}
		
	return 0;
}


u8 jieguacardCommDeal(void)
{
	DealRecord_Struct record;
	u8 sn[4] = {0x00, 0x00, 0x00, 0x00};
	u8 i;

	memcpy(SerialID, R_jieguacardInfo.cardSN, 4);
	cardType = R_jieguacardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}
	
	if (memcmp(lift_ctrl_para.projPswd, R_jieguacardInfo.projPswd, 2) == 0)
	{
		if (isIn12hours(R_jieguacardInfo.time))
		{
			for (i = 0; i < R_jieguacardInfo.num; i++)
			{
				if (memcmp(sn, R_jieguacardInfo.sn[i], 4) != 0)
				{
					writeBlackCardInfo(R_jieguacardInfo.sn[i], 0x55);
				}
			}
			
			return 1;
		}
		else
		{
			DispError(ErrNotIntime);
		}	
	}
	else
	{
		DispError(ErrYonghuma);
	}
		
	return 0;
}


u8 superUsrcardCommDeal(void)
{
	DealRecord_Struct record;
	u8  addrflag;
	u8  floorbuf[5];
	u16 count;
	u8  flag = 0;
	u8  i;
	
	memcpy(SerialID, R_superusrcardInfo.cardSN, 4);
	cardType = R_superusrcardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}
	
	if (memcmp(lift_ctrl_para.projPswd, R_superusrcardInfo.projPswd, 2) == 0)
	{
		flag = isTimeEffective(R_superusrcardInfo.useMask, 
					R_superusrcardInfo.startDate, R_superusrcardInfo.endDate, 
					R_superusrcardInfo.weekLimit, R_superusrcardInfo.timeSlot[0].startTime);
				
		if (flag)
		{
			if (R_superusrcardInfo.isForAll == 0x01)	// 适用所有楼层
			{
				memcpy(floorbuf, R_superusrcardInfo.floorSelect[0], 5);
				addrflag = 1;
			}
			else if (R_superusrcardInfo.isForAll == 0x00) // 只适用于特定电梯	
			{
				for (i = 0; i < 4; i++)
				{
					if ((lift_ctrl_para.liftAddr)
						&& (lift_ctrl_para.liftAddr == R_superusrcardInfo.liftAddr[i]))
					{
						memcpy(floorbuf, R_superusrcardInfo.floorSelect[i], 5);
						addrflag = 1;
						break;
					}
				}
			}
			
			if (addrflag == 1)
			{
				if (R_superusrcardInfo.useMask & (1 << MASK_COUNT))
				{
					count = (R_superusrcardInfo.countLimit[0]<<8) | R_superusrcardInfo.countLimit[1];				
					
					if (count == 0)
					{	
						DispError(ErrUsedout);	
						return 0;
					}
					
					superusrcardflag |= (1<<0);
				}
				
				LiftControlOpenByArray(NORMALCTRL, floorbuf, FUNXUANCENG, 1000);
				
				return 1;
			}
			else
			{
				DispError(ErrAddrerr);
			}
		}
		else
		{
			DispError(ErrNotIntime);	
		}
	}
	else
	{
		DispError(ErrYonghuma);	
	}

	return 0;
}



u8 openslotcardCommDeal(void)
{
	DealRecord_Struct record;
	u8  flag = 0;
	
	memcpy(SerialID, R_openslotcardInfo.cardSN, 4);
	cardType = R_openslotcardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}
	
	if (memcmp(lift_ctrl_para.projPswd, R_openslotcardInfo.projPswd, 2) == 0)
	{
		if (isIn12hours(R_openslotcardInfo.time)) // 12小时内
		{
			switch (R_openslotcardInfo.modeSelect)
			{
				case TIMEROPEN:		// 计时开放
					if (R_openslotcardInfo.useMask & (1<<MASK_TIMERSLOT))
					{
						memcpy(timeropenpara.floorSelect, R_openslotcardInfo.floorSelect, 5);
						memcpy(timeropenpara.time, R_openslotcardInfo.timerTime, 3);
						LiftTimerOpenCtrl(timeropenpara.floorSelect, timeropenpara.time);
					}	
					break;
				
				case CIRCLEOPEN:	// 循环开放
					CircleOpenCount = 0;

					if ((R_openslotcardInfo.useMask & (3<<MASK_CIRCLESLOT)) == (1<<MASK_CIRCLESLOT))	// 启用循环开放
					{
						LiftControlCloseAll(CIRCLECTRL);

						circleopenPara.useMask = (R_openslotcardInfo.circleUseMask) & 0x1f;
						circleopenPara.useMask |= (1<<7);

						memcpy(circleopenPara.floorSelect, R_openslotcardInfo.floorSelect, 5);
						memcpy(circleopenPara.startDate, R_openslotcardInfo.circleStartDate, 3);
						memcpy(circleopenPara.endDate, R_openslotcardInfo.circleEndDate, 3);
						circleopenPara.weekLimit = R_openslotcardInfo.circleWeekLimit;
						memcpy(circleopenPara.timeSlot, R_openslotcardInfo.circleTimeSlot, 12);
						
						flag = isTimeEffective(circleopenPara.useMask, 
										circleopenPara.startDate, circleopenPara.endDate, 
										circleopenPara.weekLimit, circleopenPara.timeSlot[0].startTime);

						if (flag == 0)
						{
							return 0;
						}
						
						LiftControlOpenByArray(CIRCLECTRL, circleopenPara.floorSelect, FUNXUANCENG, 1000);		
					}
					else if ((R_openslotcardInfo.useMask & (3<<MASK_CIRCLESLOT)) == (3<<MASK_CIRCLESLOT))// 保持循环开放
					{
						circleopenPara.useMask |= (1<<7);
					}
					else	// 撤销循环开放
					{
						circleopenPara.useMask &= ~(1<<7);
						liftctrlflag &= ~(3 << (2*CIRCLECTRL));
						LiftControlCloseAll(CIRCLECTRL);
						memset(circleopenPara.floorSelect, 0x00, 5);
					}
					break;
				
				default:
					break;
			}

			return 1;		
		}
		else
		{
			DispError(ErrNotIntime);
		}
	}
	else
	{
		DispError(ErrYonghuma);	
	}

	return 0;
}


u8 ratecardCommDeal(void)
{
	DealRecord_Struct record;
	u8  i, j;
	LiftCtrlPara para;

	
	memcpy(SerialID, R_ratecardInfo.cardSN, 4);
	cardType = R_ratecardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}
	
	if (memcmp(lift_ctrl_para.projPswd, R_ratecardInfo.projPswd, 2) == 0)
	{
		if (isIn12hours(R_ratecardInfo.time)) // 12小时内
		{
			memcpy(&(para.flag), &(lift_ctrl_para.flag), sizeof(LiftCtrlPara));
			memset(para.rate, 0x02, sizeof(lift_ctrl_para.rate));	
			
			for (i = 0; i < 8; i++)
			{
				if (R_ratecardInfo.select & (1<<i))
				{
					if ((R_ratecardInfo.rateStruct[i].startFloor) && 
						(R_ratecardInfo.rateStruct[i].endFloor < 41) &&
						(R_ratecardInfo.rateStruct[i].startFloor < R_ratecardInfo.rateStruct[i].endFloor))
					{
						for (j = (R_ratecardInfo.rateStruct[i].startFloor)-1; j < (R_ratecardInfo.rateStruct[i].endFloor)-1; j++)
						{
							para.rate[j] = R_ratecardInfo.rateStruct[i].rateMoney;
						}
					}
				}
			}
			
			writeLiftCtrlPara(para);
			readLiftCtrlPara();
			
			return 1;	
		}
		else
		{
			DispError(ErrNotIntime);
		}
	}
	else
	{
		DispError(ErrYonghuma);	
	}

	return 0;
}


u8 timecfgcardCommDeal(void)
{
	DealRecord_Struct record;
	
	memcpy(SerialID, R_timecfgcardInfo.cardSN, 4);
	cardType = R_timecfgcardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}
	
	if (((lift_ctrl_para.projPswd[0] == 0xff) && (lift_ctrl_para.projPswd[1] == 0xff)) ||
		((R_timecfgcardInfo.projPswd[0] == 0x00) && (R_timecfgcardInfo.projPswd[1] == 0x00)) ||
		(memcmp(lift_ctrl_para.projPswd, R_timecfgcardInfo.projPswd, 2) == 0))
	{	
		
		RTCSet.year	  = R_timecfgcardInfo.timeSet[0]; 
		RTCSet.month  = R_timecfgcardInfo.timeSet[1];    
		RTCSet.day    = R_timecfgcardInfo.timeSet[2];   
		RTCSet.week   = GetWeek(RTCSet.year, RTCSet.month, RTCSet.day);    
		RTCSet.hour   = R_timecfgcardInfo.timeSet[3];   
		RTCSet.minute = R_timecfgcardInfo.timeSet[4];   
		RTCSet.second = R_timecfgcardInfo.timeSet[5];    
		
		RTCSetTime(RTCSet);		
		CurrentTime = RTCGetTime();

		return 1;
	}
	else
	{
		DispError(ErrYonghuma);		
	}
	
	return 0;
}


u8 checkcardCommDeal(void)
{
	DealRecord_Struct record;
	
	memcpy(SerialID, R_checkcardInfo.cardSN, 4);
	cardType = R_checkcardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}	
	
	if (R_checkcardInfo.count == 0)
	{
		DispError(ErrUsedout);			
		return 0;
	}	
	
	S_checkcardInfo.cardType = R_checkcardInfo.cardType;
	S_checkcardInfo.count    = --R_checkcardInfo.count;
	memcpy(&(S_checkcardInfo.time[0]), &CurrentTime.year, 3);
	memcpy(&(S_checkcardInfo.time[3]), &CurrentTime.hour, 3);
	
	S_checkcardInfo.liftAddr     = lift_ctrl_para.liftAddr;  
	S_checkcardInfo.projPswd[0]  = lift_ctrl_para.projPswd[0];  
	S_checkcardInfo.projPswd[1]  = lift_ctrl_para.projPswd[1];  
	S_checkcardInfo.actiondly[0] = lift_ctrl_para.actionDly >> 8; 
	S_checkcardInfo.actiondly[1] = lift_ctrl_para.actionDly & 0xff;
	S_checkcardInfo.calldly[0]   = lift_ctrl_para.callDly >> 8; 
	S_checkcardInfo.calldly[1]   = lift_ctrl_para.callDly & 0xff;
	memcpy(S_checkcardInfo.sector, lift_ctrl_para.sector, 3);
							
	S_checkcardInfo.startFloor   = lift_ctrl_para.startFloor;  
	S_checkcardInfo.liftSN       = lift_ctrl_para.liftSN;  
	S_checkcardInfo.minMoney     = lift_ctrl_para.minMoney;  
	S_checkcardInfo.minDays      = lift_ctrl_para.minDays;  
						
	return 1;
}


u8 codecfgcardCommDeal(void)
{
	DealRecord_Struct record;
	LiftCtrlKeycode   code;
	u8  i;
	
	memcpy(SerialID, R_codecfgcardInfo.cardSN, 4);
	cardType = R_codecfgcardInfo.cardType;
	CurrentTime = RTCGetTime();
	
	if (checkCardInfo(SerialID) != 1)	// 是否是挂失的卡
	{
		dealRecord_Info(&record, SerialID, cardType, STATUS_GUASHI);
		dealRecord_StoreOne(&record);
		DispError(ErrGuashi);
						
		return 0;
	}
	
	if (memcmp(lift_ctrl_para.projPswd, R_codecfgcardInfo.projPswd, 2) == 0)
	{
		if (isIn12hours(R_codecfgcardInfo.time)) // 12小时内
		{
			memcpy(&(code.flag), &(liftctrl_keycode.flag), sizeof(LiftCtrlKeycode));

			for (i = 0; i < 40; i++)
			{
				if (R_codecfgcardInfo.codeselect[i/8] & (1<<(i%8)))
					memcpy(code.keycode[i], R_codecfgcardInfo.keycode[i/8].code[i%8], 3);
			}
			
			writeLiftCtrlKeycode(code);
			readLiftCtrlKeycode();
			
			return 1;	
		}
		else
		{
			DispError(ErrNotIntime);
		}
	}
	else
	{
		DispError(ErrYonghuma);	
	}

	return 0;
}



/*
 ****************************************************************************************
 * 函数名称：comm1_KeycodeDeal	
 * 功能描述：按键密码处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_KeycodeDeal(void)
{
	memcpy(&R_keycodeInfo, &(comm1_Rx->Union.r_keycodeInfo), sizeof(Comm1_Rx_KeyCode_Struct));
	keycodeCommDeal();
}


/*
 ****************************************************************************************
 * 函数名称：comm1_UsrCardDeal	
 * 功能描述：用户卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_UsrCardDeal(void)	// 0x41
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_usrcardInfo, &(comm1_Rx->Union.r_usrcardInfo), sizeof(Comm1_Rx_UsrCardInfo_Struct));
		
	flag = usrcardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	

	comm1_Tx->Union.s_usrcardInfo.changeFlag = usrcardflag;
	usrcardflag = 0;
	
	*comm1_lenTx = sizeof(Comm1_Tx_UsrCardInfo_Struct);	
}


/*
 ****************************************************************************************
 * 函数名称：comm1_CfgCardDeal	
 * 功能描述：设置卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_CfgCardDeal(void)	// 0x42
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_cfgcardInfo, &(comm1_Rx->Union.r_cfgcardInfo), sizeof(Comm1_Rx_CfgCardInfo_Struct));
		
	flag = cfgcardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	

	comm1_Tx->Union.s_cfgcardInfo.changeFlag = cfgcardflag;
	cfgcardflag = 0;
	
	*comm1_lenTx = sizeof(Comm1_Tx_UsrCardInfo_Struct);	
}

/*
 ****************************************************************************************
 * 函数名称：comm1_GuashiCardDeal	
 * 功能描述：挂失卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_GuashiCardDeal(void)	// 0x43
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_guashicardInfo, &(comm1_Rx->Union.r_guashicardInfo), sizeof(Comm1_Rx_GuashiCardInfo_Struct));
		
	flag = guashicardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	
}


/*
 ****************************************************************************************
 * 函数名称：comm1_GuashiCardDeal	
 * 功能描述：解挂卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_JieguaCardDeal(void)	// 0x43
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_jieguacardInfo, &(comm1_Rx->Union.r_jieguacardInfo), sizeof(Comm1_Rx_JieguaCardInfo_Struct));
		
	flag = jieguacardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	
}



/*
 ****************************************************************************************
 * 函数名称：comm1_SuperusrCardDeal	
 * 功能描述：超级用户卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_SuperusrCardDeal(void)	// 0x42
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_superusrcardInfo, &(comm1_Rx->Union.r_superusrcardInfo), sizeof(Comm1_Rx_SuperusrCardInfo_Struct));
		
	flag = superUsrcardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	

	comm1_Tx->Union.s_superusrcardInfo.changeFlag = superusrcardflag;
	superusrcardflag = 0;
	
	*comm1_lenTx = sizeof(Comm1_Tx_SuperusrCardInfo_Struct);	
}


/*
 ****************************************************************************************
 * 函数名称：comm1_OpenslotCardDeal	
 * 功能描述：开放时段卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_OpenslotCardDeal(void)
{	
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_openslotcardInfo, &(comm1_Rx->Union.r_openslotcardInfo), sizeof(Comm1_Rx_OpenSlotCardInfo_Struct));
		
	flag = openslotcardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	

	
	*comm1_lenTx = sizeof(Comm1_Tx_OpenSlotCardInfo_Struct);	
}


/*
 ****************************************************************************************
 * 函数名称：comm1_RateCardDeal	
 * 功能描述：费率设置卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_RateCardDeal(void)
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_ratecardInfo, &(comm1_Rx->Union.r_ratecardInfo), sizeof(Comm1_Rx_RateCardInfo_Struct));
		
	flag = ratecardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	

	*comm1_lenTx = sizeof(Comm1_Tx_RateCardInfo_Struct);	
}	


/*
 ****************************************************************************************
 * 函数名称：comm1_TimecfgCardDeal	
 * 功能描述：时间设置卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_TimecfgCardDeal(void)
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_timecfgcardInfo, &(comm1_Rx->Union.r_timecfgcardInfo), sizeof(Comm1_Rx_TimeCfgCardInfo_Struct));
		
	flag = timecfgcardCommDeal();

	if (flag == 1)	
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);

	dealRecord_StoreOne(&record);	

	*comm1_lenTx = sizeof(Comm1_Tx_TimeCfgCardInfo_Struct);	
}	


/*
 ****************************************************************************************
 * 函数名称：comm1_CheckCardDeal	
 * 功能描述：查看卡处理
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_CheckCardDeal(void)
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_checkcardInfo, &(comm1_Rx->Union.r_checkcardInfo), sizeof(Comm1_Rx_CheckCardInfo_Struct));
		
	flag = checkcardCommDeal();

	if (flag == 1)
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);
	
	dealRecord_StoreOne(&record);	
	
	memcpy(&(comm1_Tx->Union.s_checkcardInfo), &S_checkcardInfo, sizeof(Comm1_Tx_CheckCardInfo_Struct));	
	
	*comm1_lenTx = sizeof(Comm1_Tx_CheckCardInfo_Struct);	
}	


/*
 ****************************************************************************************
 * 函数名称：comm1_CodecfgCardDeal	
 * 功能描述：按键密码设置卡
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_CodecfgCardDeal(void)
{
	u8 flag;
	DealRecord_Struct record;
	
	memcpy(&R_codecfgcardInfo, &(comm1_Rx->Union.r_codecfgcardInfo), sizeof(Comm1_Rx_CodecfgCardInfo_Struct));
		
	flag = codecfgcardCommDeal();

	if (flag == 1)
		dealRecord_Info(&record, SerialID, cardType, STATUS_OK);
	else if (flag == 0)
		dealRecord_Info(&record, SerialID, cardType, STATUS_ERROR);
	
	dealRecord_StoreOne(&record);		
}


/*
 ****************************************************************************************
 * 函数名称：comm1_FunctionHandle
 * 功能描述：通讯命令处理函数
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
static void comm1_FunctionHandle(void)
{
	u32 i;
	u16 LRC;
	
	for (i = 0; i < COMM1_MAX_FUNC; i ++)
	{
		/* 寻找功能代码相等的处理函数                  */
		if (comm1_Rx->functionNum == comm1_Control[i].functionId)
		{
			comm1_Control[i].Function();
			
			if (*comm1_lenTx > 0)
			{
				comm1_Tx->flag = 0xa5;
				comm1_Tx->functionNum = comm1_Rx->functionNum;
				*comm1_lenTx += 2;
				LRC = check_LRC16(comm1_Tx, *comm1_lenTx);
				comm1_Tx->Union.buf[*comm1_lenTx - 2] = (u8)(LRC>>8);
				comm1_Tx->Union.buf[*comm1_lenTx - 1] = (u8)LRC;
				*comm1_lenTx += 2;
			}
			break;
		}
	}
}


/*
 ****************************************************************************************
 * 函数名称：comm2_Analyse
 * 功能描述：通讯协议解析服务函数
 * 输入参数：无
 * 输出参数：无
 * 返回参数：无
 * 补充说明：
 ****************************************************************************************
 */
void comm1_Analyse(void *rxBuff, u16 *rxLength, void *txBuff, u16 *txLength)
{
	u16 LRC;
	
	comm1_Rx = rxBuff;
	comm1_Tx = txBuff;

	comm1_lenRx = rxLength;
	comm1_lenTx = txLength;
	
	if (*comm1_lenRx >= 4)
	{
		LRC = ((comm1_Rx->Union.buf[*comm1_lenRx - 4] << 8) | comm1_Rx->Union.buf[*comm1_lenRx - 3]);
		if (LRC == check_LRC16(comm1_Rx, *comm1_lenRx - 2))
		{
			comm1_FunctionHandle();
		}
		else
		{
			uart1_Rx.count = 0;
		}
	}
}



/********************************************************************************************************
* End Of File                  
********************************************************************************************************/
