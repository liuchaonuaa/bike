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
*
********************************************************************************************************/

#ifndef __UART1_H 
#define __UART1_H

/********************************************************************************************************
* Pin Definition                                                 
********************************************************************************************************/

/********************************************************************************************************
* Macro Definition                                                
********************************************************************************************************/

extern UART1_RX uart1_Rx;
extern UART1_TX uart1_Tx;

/********************************************************************************************************
* External Function                                                  
********************************************************************************************************/

/********************************************************************************************************
* Internal Function                                                  
********************************************************************************************************/

/********************************************************************************************************
* Global Function                                                 
********************************************************************************************************/

void Rcv1BufInit(void);
void Uart1RcvData(u8 dat);
void Uart1SendDat(u8 *pStr, u16 len);

extern void uart1_init(void);
void rlyarrayvaluesend(void);

#endif

/********************************************************************************************************
* End Of File         
********************************************************************************************************/