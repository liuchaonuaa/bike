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
extern OS_EVENT	*CommSem1;

UART1_RX uart1_Rx;
UART1_TX uart1_Tx;

u8 aa[13] = {0};

/*
 ***************************************************************
 * ����: uart1_init 
 * ����: ����1��ʼ��
 * ����: NULL
 * ���: NULL 
 * ����: NULL
 * ����: ��
 ***************************************************************
 */
void uart1_init(void)
{
	UARTInit(UART1, 38400, UART_IT_RXDE, TRI_1BYTE);
	GPIOSetDir(PORT2, 5, GPIO_OUT);
	GPIOSetValue(PORT2, 5, 0);	
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
void Uart1RcvData(u8 dat)
{
    uart1_Rx.buf[uart1_Rx.count] = dat;				// ���ݴ���
  
	if (++(uart1_Rx.count) >= UART1_RX_BUFLEN)		// �����ж�
    {
        uart1_Rx.count = 0;
    }
	
	switch (uart1_Rx.count)
	{
		case 1:
			if (uart1_Rx.buf[0] != 0x5a)
			{
				uart1_Rx.count = 0;
			}
			break;

		case 2:
			switch (uart1_Rx.buf[1])
			{			
				case IDKEYCODE:
					uart1_Rx.len = 8;
					break;					
				case IDUSRCARDINFO:
					uart1_Rx.len = 88;
					break;					
				case IDCFGCARDINFO:
					uart1_Rx.len = 56;
					break;	
				case IDGUASHICARDINFO:
					uart1_Rx.len = 152;
					break;	
				case IDJIEGUACARDINFO:
					uart1_Rx.len = 152;
					break;	
				case IDSUPERUSRCARDINFO:
					uart1_Rx.len = 88;
					break;	
				case IDOPENSLOTCARDINFO:
					uart1_Rx.len = 56;
					break;	
				case IDRATECARDINFO:
					uart1_Rx.len = 56;
					break;	
				case IDTIMECFGCARDINFO:
					uart1_Rx.len = 24;
					break;	
				case IDCHECKCARDINFO:
					uart1_Rx.len = 24;
					break;	
				case IDCODECFGCARDINFO:
					uart1_Rx.len = 152;
					break;	
				
				default:
					uart1_Rx.len = 0;
					break;					
			}
			break;
			
		default:
			if ((uart1_Rx.count == uart1_Rx.len) && uart1_Rx.len)
			{
				OSSemPost(CommSem1);										
				uart1_Rx.count = 0;
			}
			else if (uart1_Rx.len == 0)
			{
				uart1_Rx.count = 0;
			}
			break;
	}
	
	if ((uart1_Rx.count > uart1_Rx.len) && uart1_Rx.len)
	{
		uart1_Rx.count = 0;
		uart1_Rx.len = 0;
	}			
}


/*
 ***************************************************************
 * ����: RcvBuf_Clear 
 * ����: �����
 * ����: NULL
 * ���: *prmt
 * ����: NULL
 * ����: ��
 ***************************************************************
 */
void Rcv1Buf_Clear(UART1_RX *prmt)
{
    memset(prmt->buf, 0, UART1_RX_BUFLEN);
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
void Rcv1BufClearAll(UART1_RX *prmt)
{
    memset(prmt->buf, 0, UART1_RX_BUFLEN);
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
void Rcv1BufInit(void)
{
    Rcv1BufClearAll(&uart1_Rx);
}


/*
 ***************************************************************
 * ����: Uart1SendDat 
 * ����: ����0��ʼ��
 * ����: 1. *pStr Ҫ��������ָ��ĵ�ַ
 *       2. len Ҫ�������ݵĳ���
 * ���: NULL 
 * ����: NULL
 * ����: ��
 ***************************************************************
 */
void Uart1SendDat(u8 *pStr, u16 len)
{
	GPIOSetValue(PORT2, 5, 1);	

	UARTSendMultiByte(UART1, pStr, len);	
	
	GPIOSetValue(PORT2, 5, 0);		
}




void rlyarrayvaluesend(void)
{
	aa[0]  = 0x55;
	aa[1]  = 0xaa;
	aa[2]  = lift_ctrl_para.liftAddr;
	memcpy(&aa[3], relayArrayValue, 5);
	aa[12] = 0x5a;
	
	Uart0SendDat(aa, 13);
}


/********************************************************************************************************
* End Of File                  
********************************************************************************************************/
