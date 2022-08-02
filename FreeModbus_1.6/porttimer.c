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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "main.h"
extern TIM_HandleTypeDef htim4;

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 3599;								// 50us��һ����
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = usTim1Timerout50us - 1;					// usTim1Timerout50us * 50��Ϊ��ʱ�����ʱ��
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        return FALSE;
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
    {
        return FALSE;
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        return FALSE;
    }

    __HAL_TIM_ENABLE_IT(&htim4, TIM_IT_UPDATE);					// ʹ�ܶ�ʱ�������ж�
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    __HAL_TIM_SET_COUNTER(&htim4, 0);		// ��ռ�����
    __HAL_TIM_ENABLE(&htim4);				// ʹ�ܶ�ʱ��
}

inline void
vMBPortTimersDisable(  )
{
     __HAL_TIM_DISABLE(&htim4);				// ���ܶ�ʱ��
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}


/// ��ʱ��4�жϷ������
void TIM4_IRQHandler(void)
{
    if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE))			// �����жϱ�Ǳ���λ
    {
        __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_UPDATE);		// ����жϱ��
        prvvTIMERExpiredISR();								// ֪ͨmodbus3.5���ַ��ȴ�ʱ�䵽
    }
}
