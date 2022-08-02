#include "mb.h"
#include "mbport.h"


// ʮ·����Ĵ���
#define REG_INPUT_SIZE  10
uint16_t REG_INPUT_BUF[REG_INPUT_SIZE];


// ʮ·���ּĴ���
#define REG_HOLD_SIZE   10
uint16_t REG_HOLD_BUF[REG_HOLD_SIZE];


// ʮ·��Ȧ
#define REG_COILS_SIZE 10
uint8_t REG_COILS_BUF[REG_COILS_SIZE];


// ʮ·��ɢ��
#define REG_DISC_SIZE  10
uint8_t REG_DISC_BUF[10];


/// CMD4
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    USHORT usRegIndex = usAddress - 1; 

    // �Ƿ����
    if((usRegIndex + usNRegs) > REG_INPUT_SIZE)
    {
        return MB_ENOREG;
    }

    // ѭ����ȡ
    while( usNRegs > 0 )
    {
        *pucRegBuffer++ = ( unsigned char )( REG_INPUT_BUF[usRegIndex] >> 8 );
        *pucRegBuffer++ = ( unsigned char )( REG_INPUT_BUF[usRegIndex] & 0xFF );
        usRegIndex++;
        usNRegs--;
    }

    // ģ������Ĵ������ı�
    for(usRegIndex = 0; usRegIndex < REG_INPUT_SIZE; usRegIndex++)
    {
        REG_INPUT_BUF[usRegIndex]++;
    }

    return MB_ENOERR;
}

/// CMD6��3��16
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    USHORT usRegIndex = usAddress - 1;  

    // �Ƿ����
    if((usRegIndex + usNRegs) > REG_HOLD_SIZE)
    {
        return MB_ENOREG;
    }

	// д�Ĵ���
    if(eMode == MB_REG_WRITE)
    {
        while( usNRegs > 0 )
        {
			REG_HOLD_BUF[usRegIndex] = (pucRegBuffer[0] << 8) | pucRegBuffer[1];
			pucRegBuffer += 2;
            usRegIndex++;
            usNRegs--;
        }
    }
	
	// ���Ĵ���
    else
    {
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( REG_HOLD_BUF[usRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( REG_HOLD_BUF[usRegIndex] & 0xFF );
            usRegIndex++;
            usNRegs--;
        }
    }

    return MB_ENOERR;
}

/// CMD1��5��15
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    USHORT usRegIndex   = usAddress - 1;
    USHORT usCoilGroups = ((usNCoils - 1) / 8 + 1);
    UCHAR  ucStatus     = 0;
    UCHAR  ucBits       = 0;
    UCHAR  ucDisp       = 0;

    // �Ƿ����
    if((usRegIndex + usNCoils) > REG_COILS_SIZE)
    {
        return MB_ENOREG;
    }

    // д��Ȧ
    if(eMode == MB_REG_WRITE)
    {
        while(usCoilGroups--)
        {
            ucStatus = *pucRegBuffer++;
            ucBits   = 8;
            while((usNCoils--) != 0 && (ucBits--) != 0)
            {
                REG_COILS_BUF[usRegIndex++] = ucStatus & 0X01;
                ucStatus >>= 1;
            }
        }
    }

    // ����Ȧ
    else
    {
        while(usCoilGroups--)
        {
            ucDisp = 0;
            ucBits = 8;
            while((usNCoils--) != 0 && (ucBits--) != 0)
            {
                ucStatus |= (REG_COILS_BUF[usRegIndex++] << (ucDisp++));
            }
            *pucRegBuffer++ = ucStatus;
        }
    }
    return MB_ENOERR;
}


/// CMD4
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    USHORT usRegIndex   = usAddress - 1;
    USHORT usCoilGroups = ((usNDiscrete - 1) / 8 + 1);
    UCHAR  ucStatus     = 0;
    UCHAR  ucBits       = 0;
    UCHAR  ucDisp       = 0;

    // �Ƿ����
    if((usRegIndex + usNDiscrete) > REG_DISC_SIZE)
    {
        return MB_ENOREG;
    }

	// ����ɢ����
	while(usCoilGroups--)
	{
		ucDisp = 0;
		ucBits = 8;
		while((usNDiscrete--) != 0 && (ucBits--) != 0)
		{
			if(REG_DISC_BUF[usRegIndex])
			{
				ucStatus |= (1 << ucDisp);
			}
			ucDisp++;
		}
		*pucRegBuffer++ = ucStatus;
	}

    // ģ��ı�
    for(usRegIndex = 0; usRegIndex < REG_DISC_SIZE; usRegIndex++)
    {
        REG_DISC_BUF[usRegIndex] = !REG_DISC_BUF[usRegIndex];
    }

    return MB_ENOERR;
}

