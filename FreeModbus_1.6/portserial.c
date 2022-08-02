/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "main.h"
extern UART_HandleTypeDef huart2;

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if(xRxEnable)
    {
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);		// ʹ�ܽ��շǿ��ж�
    }
    else
    {
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);		// ���ܽ��շǿ��ж�
    }

    if(xTxEnable)
    {
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);			// ʹ�ܷ���Ϊ���ж�
    }
    else
    {
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);		// ���ܷ���Ϊ���ж�
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = ulBaudRate;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    switch(eParity)
    {
	// ��У��
    case MB_PAR_ODD:
        huart2.Init.Parity = UART_PARITY_ODD;
        huart2.Init.WordLength = UART_WORDLENGTH_9B;			// ����żУ������λΪ9bits
        break;
	
	// żУ��
    case MB_PAR_EVEN:
        huart2.Init.Parity = UART_PARITY_EVEN;
        huart2.Init.WordLength = UART_WORDLENGTH_9B;			// ����żУ������λΪ9bits
        break;
	
	// ��У��
    default:
        huart2.Init.Parity = UART_PARITY_NONE;
        huart2.Init.WordLength = UART_WORDLENGTH_8B;			// ����żУ������λΪ8bits
        break;
    }
    return HAL_UART_Init(&huart2) == HAL_OK ? TRUE : FALSE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    USART2->DR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = (USART2->DR & (uint16_t)0x00FF);
	return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

void USART2_IRQHandler(void)
{
    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE))			// ���շǿ��жϱ�Ǳ���λ
    {
        __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);			// ����жϱ��
        prvvUARTRxISR();										// ֪ͨmodbus�����ݵ���
    }

    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE))				// ����Ϊ���жϱ�Ǳ���λ
    {
        __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_TXE);			// ����жϱ��
        prvvUARTTxReadyISR();									// ֪ͨmodbus���ݿ��Է���
    }
}
