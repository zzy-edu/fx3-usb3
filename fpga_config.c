#include "fpga_config.h"
#include "fx3_pin_define.h"
#include "fx3_spi.h"
#include "mcu_spi.h"
#include "app_grab_cfg.h"
#include <cyu3gpio.h>
#include <cyu3os.h>

CyU3PMutex  fpga_spi_lock;


/*
 * fpga spi ���ĳ�ʼ��
 */
void fpga_locked_init(void)
{
	CyU3PMutexCreate(&fpga_spi_lock, CYU3P_NO_INHERIT);
}


CyBool_t fpga_locked_Deinit(void)
{
    CyU3PMutexDestroy(&fpga_spi_lock);
}

/*function
********************************************************************************
<PRE>
������   :
����     :fpga��ͨѶ��ʼ��
����     :
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
CyBool_t fpga_reg_init(void)
{
    uint16_t i = 0, datalow, datahigh;
    while (i++ < 50)
    {
//        CyU3PGpioSetValue(FPGA_RESET_PIN, CyTrue);
//        CyU3PGpioSetValue(FPGA_RESET_PIN, CyFalse);
        datalow = 1;
        datahigh = 1;
        CyU3PThreadSleep(100);
        fpga_reg_read(0x0000, &datalow, 1); // ���汾��
        fpga_reg_read(0x0001, &datahigh, 1);
        if ((datalow != 0 && datalow != 0xFFFF) || (datahigh != 0 && datahigh != 0xFFFF))
        {
            return CyTrue;
        }
    }
    return CyFalse;
}

CyBool_t fpga_reg_test(void)
{
    uint16_t i = 0, data1 = 0, data;
    while (i++ < 20)
    {
        data = 0x86;
        CyU3PThreadSleep(100);
        fpga_reg_write(0x3000,&data,1);

        fpga_reg_read(0x3000, &data1, 1); // DDR�ɹ���ʼ��
        if (data == data1)
        {
            return CyTrue;
        }
    }
    return CyFalse;
}

// todo fpga�Ĵ�����д���ԣ�fpgaͳ��ֵ����
CyBool_t fpga_init(void)
{
    uint16_t i = 0;
    uint8_t wrBuffer[1];
    uint16_t clearvalue=0xFFFF;
    wrBuffer[0] = 0xab;
    while(i++ < 10)
    {

        CyU3PGpioSetValue(FPGA_N_CONFIG_PIN, CyTrue);
        CyU3PThreadSleep(100);
        CyU3PGpioSetValue(FPGA_N_CONFIG_PIN, CyFalse);
        CyU3PThreadSleep(1);
        CyU3PGpioSetValue(FPGA_N_CONFIG_PIN, CyTrue);


        if(fpga_reg_init() == CyFalse)
        {
            continue;
        }
        else if(fpga_reg_test() == CyFalse)
        {
            continue;
        }
        else
        {
    		CyU3PThreadSleep(800);
    		MCUSpiWriteRead(wrBuffer, 1,NULL,0,0);
    		fpga_locked_init();
    		//���һЩ����ֵ
    		fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&clearvalue,1);
    		CyU3PThreadSleep(1);
    		clearvalue = 0x0;
    		fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&clearvalue,1);
        	if(GrabGetDefaultUserParam() == CyFalse){CyU3PDebugPrint(4,"\nUSE DEFALUT PARAM");}
            return CyTrue;
        }
    }


    return CyFalse;
}

CyBool_t fpga_Reinit(void)
{
	fpga_locked_Deinit();
	if(CyFalse == fpga_init())
	{
		return CyFalse;
	}
	return CyTrue;
}

/*function
********************************************************************************
<PRE>
������   :
����     :��ȡfpga�ڲ��Ĵ�����ֵ
����     :
        nAddr �Ĵ�����ַ15bit��Ч��ַ,���bit : 1 д0��
        pData 16bit��Ч����ֵָ��
        nLen  ���ݳ��ȣ�16bitΪһ����Ԫ
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
void fpga_reg_read(uint16_t startAddr, uint16_t *pData, uint16_t len)
{
    int i;
    uint8_t *ptmp;
    uint8_t Buffer[4];

    CyU3PMutexGet(&fpga_spi_lock,CYU3P_WAIT_FOREVER);
    for (i = 0; i < len; i++)
    {
        Buffer[0] = ((startAddr + i) >> 8) & 0xFF; //��λ��ַ
        Buffer[0] |= 0x80; //д��־
        Buffer[1] = (startAddr + i) & 0xFF;//��λ��ַ
        FxIOSpiWriteRead(Buffer, 2, &Buffer[2], 2, 2);
        ptmp = (uint8_t *)(&pData[i]); //pData[i],i+1��ַƫ��16λ
        ptmp[0] = Buffer[3];
        ptmp[1] = Buffer[2];
    }
    CyU3PMutexPut(&fpga_spi_lock);
}

/*function
********************************************************************************
<PRE>
������   :
����     :д��fpga�ڲ��Ĵ�����ֵ
����     :
        nAddr �Ĵ�����ַ15bit��Ч��ַ,���bit : 1 д0��
        pData 16bit��Ч����ֵָ��
        nLen  ���ݳ��ȣ�16bitΪһ����Ԫ
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
void fpga_reg_write(uint16_t startAddr, uint16_t *pData, uint16_t len)
{
    uint8_t Buffer[4];
    int i;

    CyU3PMutexGet(&fpga_spi_lock,CYU3P_WAIT_FOREVER);
    for (i = 0; i < len; i++)
    {
        Buffer[0] = ((startAddr + i) >> 8) & 0xFF;
        Buffer[0] &= 0x7F;
        Buffer[1] = (startAddr + i) & 0xFF;
        Buffer[2] = (pData[i] >> 8) & 0xFF;
        Buffer[3] = pData[i] & 0xFF;
        FxIOSpiWriteRead(Buffer, 4, NULL, 0, 0);
    }
    CyU3PMutexPut(&fpga_spi_lock);
}
