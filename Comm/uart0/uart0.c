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
#include "../UserCode/source/config.h"    			 // ͷ�ļ�


/********************************************************************************************************
* Variable definition                            
********************************************************************************************************/
extern OS_EVENT	*CommSem0;

UART0_RX uart0_Rx;
UART0_TX uart0_Tx;
u8 recmod = 0;

/*
 ***************************************************************
 * ����: uart0_init 
 * ����: ����1��ʼ��
 * ����: NULL
 * ���: NULL 
 * ����: NULL
 * ����: ��
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
 * ����: Uart1RcvData 
 * ����: ����1���ݽ���
 * ����: 1. dat ���ݽ���
 * ���: NULL 
 * ����: NULL
 * ����: ��
 ***************************************************************
 */
void Uart0RcvData(u8 dat)
{
    uart0_Rx.buf[uart0_Rx.count] = dat;				// ���ݴ���
  
	if ((uart0_Rx.count) >= UART0_RX_BUFLEN)		// �����ж�
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
 * ����: Rcv1Buf_Clear 
 * ����: �����
 * ����: NULL
 * ���: *prmt
 * ����: NULL
 * ����: ��
 ***************************************************************
 */
void Rcv0Buf_Clear(UART0_RX *prmt)
{
    memset(prmt->buf, 0, UART0_RX_BUFLEN);
	prmt->count = 0;
}


/*
 ***************************************************************
 * ����: Rcv1BufClearAll 
 * ����: �����
 * ����: NULL
 * ���: *prmt
 * ����: NULL
 * ����: ��
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
 * ����: RcvBufInit 
 * ����: ��ʼ��
 * ����: NULL
 * ���: NULL
 * ����: NULL
 * ����: ��
 ***************************************************************
 */
void Rcv0BufInit(void)
{
    Rcv0BufClearAll(&uart0_Rx);
}


/*
 ***************************************************************
 * ����: Uart1SendDat 
 * ����: ����1��ʼ��
 * ����: 1. *pStr Ҫ��������ָ��ĵ�ַ
 *       2. len Ҫ�������ݵĳ���
 * ���: NULL 
 * ����: NULL
 * ����: ��
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
