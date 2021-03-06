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

#ifndef __UART0_H 
#define __UART0_H

/********************************************************************************************************
* Pin Definition                                                 
********************************************************************************************************/

/********************************************************************************************************
* Macro Definition                                                
********************************************************************************************************/

extern UART0_RX uart0_Rx;
extern UART0_TX uart0_Tx;

/********************************************************************************************************
* External Function                                                  
********************************************************************************************************/

/********************************************************************************************************
* Internal Function                                                  
********************************************************************************************************/

/********************************************************************************************************
* Global Function                                                 
********************************************************************************************************/

void Rcv0BufInit(void);
void Uart0RcvData(u8 dat);
void Uart0SendDat(u8 *pStr, u16 len);

extern void uart0_init(void);

#endif

/********************************************************************************************************
* End Of File         
********************************************************************************************************/
