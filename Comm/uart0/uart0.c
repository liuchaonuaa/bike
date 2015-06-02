/******************************************************************************************************** 
*                             
*                                    Address
*  
*                    (c) Copyright 20xx, Company Name, City, State
*                               All Rights Reserved
*
*
* FileName       : uart1.c
* Version        : V1.0
* Programmer(s)  : liuchao
* Date           : 2014/12
* Parameters     : LPC1700 12.000MHz
* DebugTools     : Realview MDK V4.72  JLINK V8(SWD)
* Description    :
*
********************************************************************************************************/

/********************************************************************************************************
* HeadeHCiles                            
********************************************************************************************************/ 
#include "../UserCode/source/config.h"    			 // 头文件


/********************************************************************************************************
* Variable definition                            
********************************************************************************************************/
extern OS_EVENT	*CommSem0;

UART0_RX uart0_Rx;
UART0_TX uart0_Tx;
u8 recmod = 0;

/*
 ***************************************************************
 * 名称: uart0_init 
 * 功能: 串口1初始化
 * 输入: NULL
 * 输出: NULL 
 * 返回: NULL
 * 描述: 无
 ***************************************************************
 */
void uart0_init(void)
{
	UARTInit(UART0, 115200, UART_IT_RXDE, TRI_1BYTE);
	GPIOSetDir(PORT0, 1, GPIO_OUT);
	GPIOSetValue(PORT0, 1, 0);	
}


/*
 ***************************************************************
 * 名称: Uart1RcvData 
 * 功能: 串口1数据接收
 * 输入: 1. dat 数据接收
 * 输出: NULL 
 * 返回: NULL
 * 描述: 无
 ***************************************************************
 */
void Uart0RcvData(u8 dat)
{
    uart0_Rx.buf[uart0_Rx.count] = dat;				// 数据存入
  
	if ((uart0_Rx.count) >= UART0_RX_BUFLEN)		// 缓冲判断
    {
        uart0_Rx.count = 0;
    }
	
	switch (recmod)
	{
		case 0:
			if (uart0_Rx.buf[0] == lift_ctrl_para.liftAddr)
			{
				recmod = 1;
				uart0_Rx.count++;
			}
			else
			{
				recmod = 0;
				uart0_Rx.count = 0;
			}
			break;
		
		case 1:
			switch (uart0_Rx.buf[1])
			{
				case IDSETLIFTPARA:
					uart0_Rx.len = 24;
					break;
				case IDGETLIFTPARA:
					uart0_Rx.len = 16;
					break;
				case IDSETLIFTRATE:
					uart0_Rx.len = 37;
					break;
				case IDGETLIFTRATE:
					uart0_Rx.len = 16;
					break;
				case IDREMOTECTRL:
					uart0_Rx.len = 16;
					break;
				case IDTIMEROPEN:
					uart0_Rx.len = 16;
					break;
				case IDCIRCLEOPEN:
					uart0_Rx.len = 29;
					break;
				case IDGUASHI:
					uart0_Rx.len = 133;
					break;
				case IDJIEGUA:
					uart0_Rx.len = 133;
					break;
				case IDGETRECORDNUM:
					uart0_Rx.len = 16;
					break;
				case IDGETRECORD:
					uart0_Rx.len = 16;
					break;
				case IDGETRECORDACK:
					uart0_Rx.len = 16;
					break;
				case IDREMOVERECORD:
					uart0_Rx.len = 16;
					break;
				case IDRESTORE:
					uart0_Rx.len = 16;
					break;				
				case IDCOMMTEST:
					uart0_Rx.len = 16;
					break;					
				case IDUPDATEPROGRAM:
					uart0_Rx.len = 8;
					break;				
				default:
					uart0_Rx.len = 0;
					break;	
			}
			
			if (uart0_Rx.len != 0)
			{
				recmod = 2;
				uart0_Rx.count++;
			}
			else
			{
				uart0_Rx.count = 0;
				recmod = 0;
			}	
			break;
		
		case 2:
			uart0_Rx.count++;
			break;
		
		default:
			recmod = 0;
			uart0_Rx.count = 0;			
			break;
	}
	
	
	if ((uart0_Rx.count >= uart0_Rx.len) && uart0_Rx.len && (recmod == 2))
	{
		recmod = 0;
		uart0_Rx.count = 0;
		OSSemPost(CommSem0);
	}	
	
}


/*
 ***************************************************************
 * 名称: Rcv1Buf_Clear 
 * 功能: 清零参
 * 输入: NULL
 * 输出: *prmt
 * 返回: NULL
 * 描述: 无
 ***************************************************************
 */
void Rcv0Buf_Clear(UART0_RX *prmt)
{
    memset(prmt->buf, 0, UART0_RX_BUFLEN);
	prmt->count = 0;
}


/*
 ***************************************************************
 * 名称: Rcv1BufClearAll 
 * 功能: 清零参
 * 输入: NULL
 * 输出: *prmt
 * 返回: NULL
 * 描述: 无
 ***************************************************************
 */
void Rcv0BufClearAll(UART0_RX *prmt)
{
    memset(prmt->buf, 0, UART0_RX_BUFLEN);
	prmt->count = 0;
	prmt->len   = 0;
}


/*
 ***************************************************************
 * 名称: RcvBufInit 
 * 功能: 初始化
 * 输入: NULL
 * 输出: NULL
 * 返回: NULL
 * 描述: 无
 ***************************************************************
 */
void Rcv0BufInit(void)
{
    Rcv0BufClearAll(&uart0_Rx);
}


/*
 ***************************************************************
 * 名称: Uart1SendDat 
 * 功能: 串口1初始化
 * 输入: 1. *pStr 要发的数据指向的地址
 *       2. len 要发的数据的长度
 * 输出: NULL 
 * 返回: NULL
 * 描述: 无
 ***************************************************************
 */
void Uart0SendDat(u8 *pStr, u16 len)
{
	GPIOSetValue(PORT0, 1, 1);	

	UARTSendMultiByte(UART0, pStr, len);	
	
	GPIOSetValue(PORT0, 1, 0);		
}


/********************************************************************************************************
* End Of File                  
********************************************************************************************************/
