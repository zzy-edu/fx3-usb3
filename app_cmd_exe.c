#include "app_cmd_exe.h"
#include "app_storage_cfg.h"
#include "fpga_config.h"
#include <cyu3gpio.h>
#include <cyu3system.h>
#include "fx3_pin_define.h"
#include "mcu_spi.h"
#include "app_grab_cfg.h"
#include "app_virtual_uart.h"

static CyBool_t inline app_cmd_Str_Cmp(const uint8_t *s1, const uint8_t *s2)
{
    return (s1[0] == s2[0] && s1[1] == s2[1] && s1[2] == s2[2] && s1[3] == s2[3]);
}

static void inline fill_command_char(tagCmdFormatterContent *cmd, uint8_t t1, uint8_t t2, uint8_t t3, uint8_t t4)
{
    cmd->Command_char[0] = t1;
    cmd->Command_char[1] = t2;
    cmd->Command_char[2] = t3;
    cmd->Command_char[3] = t4;
    return;
}

// ����ָ��,������������Ϊ�ؼ���
CyBool_t exe_h(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'h', 0, 0, 0);
    cmdSend->Param_Num = 0;
    return CyTrue;
}

// ����ϵͳ
CyBool_t exe_boot(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'b', 'o', 'o', 't');
    cmdSend->Param_Num = 0;
    CyU3PDeviceReset(CyFalse);
    return CyTrue;
}

// �õ�FPGA����汾��:�����ո�ʽ
CyBool_t exe_get_FPGA_version(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'g', 'f', 'v', 0);
    cmdSend->Param_Num = 1;
    // TODO : fill FPGA_REG_FPGA_VERSION1_ADDR
    fpga_reg_read(FPGA_VERSION1_REG_ADDRESS, (uint16_t *)&cmdSend->Params[0], 2);
    return CyTrue;
}

// �õ�MCU�汾��:�����ո�ʽ
CyBool_t exe_get_software_version(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'g', 'm', 'v', 0);
    cmdSend->Param_Num = 1;
    cmdSend->Params[0] = SOFTWARE_VERSION;
    return CyTrue;
}

// �趨����ͨ�ŵĶ���rs485ͨ����ֻ����һ����豸:  [1] MCU �� DSP
CyBool_t exe_set_comm_obj(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    // TODO :
    fill_command_char(cmdSend, 'c', 'o', 'm', 'm');
    if(cmdRecv->Param_Num < 1)
    {
    	return CyFalse;
    }
    else if(cmdRecv->Param_Num == 1)
    {
    	globUartConfig = ((uint8_t)cmdRecv->Params[0] == 1 ? CyTrue : CyFalse);
    	if(globUartConfig == CyTrue)
    	{
    		DebugInitUsingCDC();
    		CyU3PDebugPrint(4,"\nDebug Channel init ...");
    		CyU3PDebugPrint(4,"\nDebug Channel ok ...");
    	}
    	else
    	{
    		CyU3PDebugPrint(4,"\nStart CDC Channel ...");
    		CyU3PDebugPrint(4,"\nStart CDC Channel OK, Please Restart Device ...");
    		CyU3PThreadSleep(10);
    		/* Ŀǰû��ͨ�������л���CDC����ͨ�� */
    		DebugDeInitStartCDC();
    	}
    	return CyTrue;
    }
    else if(cmdRecv->Param_Num == 2)
    {
    	GrabGetFpgaLedStatus((uint8_t)cmdRecv->Params[0]&0xff,(uint8_t)cmdRecv->Params[1]&0xff);
    	return CyTrue;
    }
    return CyFalse;
}

// �趨����ͨ�ŵĲ����ʺ���żУ��
CyBool_t exe_set_comm_para(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    // TODO :
    fill_command_char(cmdSend, 'c', 'o', 'm', 'p');
    return CyTrue;
}

// ��ȡϵͳΨһID,���ô�ID�����з�����Ӧ�ü���,����8���ֽ�
CyBool_t exe_read_system_ID(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    uint8_t tmp[8] = {0};
    int i;
    fill_command_char(cmdSend, 'r', 's', 'i', 'd');
    if (MCUFlashReadProtectID(tmp) == CyFalse)
    {
        return CyFalse;
    }
    cmdSend->Param_Num = 8;
    for (i = 0; i < 8; i++)
    {
        cmdSend->Params[i] = tmp[i];
    }
    return CyTrue;
}

// ����ϵͳ�Ľ������벻����: д��8���ֽڵļ�������
CyBool_t exe_set_system_password(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    int i;
    fill_command_char(cmdSend, 's', 's', 'p', 'd');
    if (cmdRecv->Param_Num != 8)
    {
        return CyFalse;
    }
    for (i = 0; i < 8; i++)
    {
         glbSystem.nPassword[i] = cmdRecv->Params[i] & 0xFF;
    }
    return CyTrue;
}

// ��ȡϵͳ����,����ϵͳ�����ṹ������
CyBool_t exe_read_para(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    uint32_t n;
    fill_command_char(cmdSend, 'r', 'p', 0, 0);
    if (cmdRecv->Param_Num <= 2)
    {
        // TODO : test
        if (MCUSpiFlashRead(0, (FLASH_START_SECTOR + cmdRecv->Params[0] + 400) * FX3_FLASH_SECTOR_SIZE, (uint8_t *)&n, 4) == CyFalse)
        {
            return CyFalse;
        }
        if (n != SYSTEM_PARAM_VALID_CODE)
        {
            return CyFalse;
        }
        // TODO : test
        if (MCUSpiFlashRead(0, (FLASH_START_SECTOR + cmdRecv->Params[0] + 400) * FX3_FLASH_SECTOR_SIZE,
                          (uint8_t *)&glbCamParam, sizeof(glbCamParam)) == CyFalse)
        {
            return CyFalse;
        }
        if (cmdRecv->Param_Num <= 1)
        {
            // TODO :
            // if(!StorageWriteUserParam(&glbCamParam,2))
            //     return CyFalse;
            // if(!StorageSetDefaultUserParam(2))
            //     return CyFalse;
        }
        else
        {
            if (cmdRecv->Params[1] > 2)
            {
                cmdRecv->Params[1] = 2;
            }
            // TODO :
            // if(!StorageWriteUserParam(&glbCamParam,cmdRecv->Params[1]))
            //     return CyFalse;
            // if(!StorageSetDefaultUserParam(cmdRecv->Params[1]))
            //     return CyFalse;
        }
        // FpgaRegConfigAll();
        // ComosRegConfigAll();
        // TaskImageReadyStartUp();
    }
    else
    {
        return CyFalse;
    }
    return CyTrue;
}

// д��ϵͳ����,���������б���Ϊϵͳ�����ṹ������
CyBool_t exe_write_para(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'w', 'p', 0, 0);
    if (cmdRecv->Param_Num <= 1)
    {
        // TODO : test
        if (MCUFlashEraseAndWrite(0, (uint8_t *)&glbCamParam, (FLASH_START_SECTOR + cmdRecv->Params[0] + 400) * FX3_FLASH_SECTOR_SIZE,
                                   sizeof(tagCmdFormatterContent)) == CyFalse)
        {
            return CyFalse;
        }
    }
    else
    {
        return CyFalse;
    }
    return CyTrue;
}

// �õ�ϵͳ״̬,����0��ʾ״̬����,����Ϊ����
CyBool_t exe_get_status(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'g', 's', 0, 0);
    // ���� grabsysStatus
    cmdSend->Param_Num = 1;
    CyU3PMemCopy((uint8_t*)(&cmdSend->Params[0]),(uint8_t*)(&grabsysStatus),4);
    return CyTrue;
}

// ͣ����ʱһ��ʱ��,��λms���趨���ڷ�������ʱ:  [1] ͣ��ʱ����
CyBool_t exe_hold(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'h', 'o', 'l', 'd');
    if (cmdRecv->Param_Num == 1)
    {
        // TODO :
        // glbCamParam.nCommAckDelayMs = cmdRecv->Params[0];
    }
    else if (cmdRecv->Param_Num == 0)
    {
        cmdSend->Param_Num = 1;
        // cmdSend->Params[0] = glbCamParam.nCommAckDelayMs;
    }
    return CyTrue;
}

// NOTE : flash
int flashSelect = 1; // Ĭ��ѡ���flashƫ�Ƶ�ַ��ʼ,���ѡ��Ϊ0���flash��ʼ����

// ����flash  [1] secor ��Ŵ�0��ʼ����
CyBool_t exe_erase_flash_sector(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'f', 'e', 'r', 0);

    //fpga������ַ�����²���
    if((cmdRecv->Params[0] >= (FX3_FLASH_LEN*2/FX3_FLASH_SECTOR_SIZE)) && (cmdRecv->Params[0] < (FX3_FLASH_LEN*3/FX3_FLASH_SECTOR_SIZE)))
    {
    	flashSelect = 0;
    	cmdRecv->Params[0] -= ((FX3_FLASH_LEN*2)/FX3_FLASH_SECTOR_SIZE);
    }
    else if((cmdRecv->Params[0] >= (FX3_FLASH_LEN*3/FX3_FLASH_SECTOR_SIZE)) && (cmdRecv->Params[0] < (FX3_FLASH_LEN*4/FX3_FLASH_SECTOR_SIZE)))
    {
    	flashSelect = 0;
    	cmdRecv->Params[0] -= ((FX3_FLASH_LEN*3)/FX3_FLASH_SECTOR_SIZE);
    }
    else if((cmdRecv->Params[0] >= (FX3_FLASH_LEN*4/FX3_FLASH_SECTOR_SIZE)) && (cmdRecv->Params[0] < (FX3_FLASH_LEN*5/FX3_FLASH_SECTOR_SIZE)))
    {
    	flashSelect = 0;
    	cmdRecv->Params[0] -= ((FX3_FLASH_LEN*4)/FX3_FLASH_SECTOR_SIZE);
    }
    else
    {
    	flashSelect = 1;
    }
    if (MCUFlashEraseSector(0, FLASH_START_SECTOR * flashSelect + cmdRecv->Params[0]) == CyTrue)
    {
        return CyTrue;
    }
    else
    {
        return CyFalse;
    }
}

// ��дflash: [1] ��ʼ��ַ [2] �ֽڳ��� [3]... ����д����ֽ�
CyBool_t exe_program_flash(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'f', 'p', 'r', 0);
    if (cmdRecv->Params[1] > MAX_COMMAND_PARAM_NUM * 4 - 3 * 4)
    {
        return CyFalse;
    }

    //fpga������ַ�����²���
    if( (cmdRecv->Params[0] >= (FX3_FLASH_LEN*2)) && (cmdRecv->Params[0] < (FX3_FLASH_LEN*3)))
    {
    	flashSelect = 0;
    	cmdRecv->Params[0] -= ((FX3_FLASH_LEN*2));
    }
    else if((cmdRecv->Params[0] >= (FX3_FLASH_LEN*3)) && (cmdRecv->Params[0] < (FX3_FLASH_LEN*4)))
    {
    	flashSelect = 0;
    	cmdRecv->Params[0] -= ((FX3_FLASH_LEN*3));
    }
    else if((cmdRecv->Params[0] >= (FX3_FLASH_LEN*4)) && (cmdRecv->Params[0] < (FX3_FLASH_LEN*5)))
    {
    	flashSelect = 0;
    	cmdRecv->Params[0] -= ((FX3_FLASH_LEN*4));
    }
    else
    {
    	flashSelect = 1;
    }
    if (MCUSpiFlashWrite(0, (uint8_t *)&cmdRecv->Params[2],
                       FLASH_START_SECTOR * FX3_FLASH_SECTOR_SIZE * flashSelect + cmdRecv->Params[0],
                       cmdRecv->Params[1]) == CyFalse)
    {
        return CyFalse;
    }
    return CyTrue;
}

// ��ȡflash: [1] ��ʼ��ַ [2] �ֽڳ���, ��������ǰ��������һ��,����Ϊʵ������
CyBool_t exe_read_flash(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'f', 'r', 'e', 0);
    if (cmdRecv->Params[1] > MAX_COMMAND_PARAM_NUM * 4 - 3 * 4)
    {
        return CyFalse;
    }
    cmdSend->Params[0] = cmdRecv->Params[0];
    cmdSend->Params[1] = cmdRecv->Params[1];
    if (MCUSpiFlashRead(0, FLASH_START_SECTOR * FX3_FLASH_SECTOR_SIZE * flashSelect + cmdRecv->Params[0],
                      (uint8_t *)&cmdSend->Params[2], cmdRecv->Params[1]) == CyFalse)
    {
        fill_command_char(cmdSend, 'E', 'R', '\r', '\n');
        return CyFalse;
    }
    else
    {
        cmdSend->Param_Num = cmdRecv->Param_Num + (cmdRecv->Params[1] >> 2);
    }
    return CyTrue;
}

// TODO : ???��ȡflash: [1] ��ʼ��ַ [2] �ֽڳ���, ��������ǰ��������һ��,����Ϊʵ������
CyBool_t exe_flash_select(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'f', 's', 'e', 'l');
    if (cmdRecv->Param_Num == 1)
    {
		if (cmdRecv->Params[0] == 0)
		{
//			CyU3PGpioSetValue(FPGA_NCE_PIN, CyTrue);
			CyU3PGpioSetValue(FPGA_N_CONFIG_PIN, CyFalse);
		}
        flashSelect = cmdRecv->Params[0];
    }
    else if (cmdRecv->Param_Num == 0)
    {
        cmdSend->Param_Num = 1;
        cmdSend->Params[0] = flashSelect;
    }
    else
    {
        return CyFalse;
    }
    return CyTrue;
}

// ���浱ǰ�趨�������û�������: [1] ���������0-3(user param save)
CyBool_t exe_save_user_para(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 's', 'v', 'u', 0);
    uint8_t nIndex = 0;
    //TODO �����û�����
    if(cmdRecv->Param_Num == 0)
    {
    	nIndex = grabconfParam.n_device_type;
    	CyU3PDebugPrint(4,"\n nIndex = %d",nIndex);
    	if(CyFalse == GrabWriteUserParam(&grabconfParam,nIndex))
    	{
    		return CyFalse;
    	}
    }
    else if(cmdRecv->Param_Num == 1)
    {
    	nIndex = *(uint8_t*)(&cmdRecv->Params[0]);
    	CyU3PDebugPrint(4,"\n nIndex = %d",nIndex);
    	if(CyFalse == GrabWriteUserParam(&grabconfParam,nIndex))
    	{
    		return CyFalse;
    	}
    }
    else
    {
    	return CyFalse;
    }
    cmdSend->Param_Num = 0;
    return CyTrue;
}

// ���õ�ǰĬ���������صĲ��������ĸ�
CyBool_t exe_set_default_user(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 's', 'd', 'u', 0);
    if (cmdRecv->Param_Num == 1)
    {
        if (cmdRecv->Params[0] > /*CAM_PARAM_USER_NUMBER*/ 3)
        {
            return CyFalse;
        }
        // if (glbCamParam.nSaveArea == cmdRecv->Params[0])
        // {
        //     return CyTrue;
        // }
        if (!StorageSetDefaultUserParam(cmdRecv->Params[0]))
        {
            return CyFalse;
        }
        // glbCamParam.nSaveArea = cmdRecv->Params[0];
        CyU3PDeviceReset(CyFalse);
    }
    else if (cmdRecv->Param_Num == 0)
    {
        cmdSend->Param_Num = 1;
        // cmdSend->Params[0] = glbCamParam.nSaveArea;
        cmdSend->Params[0] = 0;
    }
    return CyTrue;
}

// ������������Լ��û�����
CyBool_t exe_clear_factory_and_user_para(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    int i;
    fill_command_char(cmdSend, 'c', 'f', 'a', 'u');
    if ((cmdRecv->Param_Num == 1) && (cmdRecv->Params[0] == 'u'))
    {
        for (i = 0; i < /*CAM_PARAM_USER_NUMBER*/ 3; i++)
        {
            if (MCUFlashEraseSector(0, FLASH_START_SECTOR + i + 1) == CyFalse)
            {
                return CyFalse;
            }
        }
    }
    if ((cmdRecv->Param_Num == 1) && (cmdRecv->Params[0] == 'f'))
    {
        if (MCUFlashEraseSector(0, FLASH_START_SECTOR + 50))
        {
            return CyFalse;
        }
    }
    if (cmdRecv->Param_Num == 0)
    {
        if (StorageSetDefaultUserParam(3) == CyFalse)
        {
            return CyFalse;
        }
        else
        {
            CyU3PDeviceReset(CyFalse);
        }
    }
    return CyTrue;
}

// �����豸��Ϣ,����Ϊ��,���߲����ṹ������
CyBool_t exe_save_dev_info(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    uint8_t nPass[8];
    fill_command_char(cmdSend, 's', 'd', 'i', 'f');
    CyU3PMemCopy(nPass, glbSystem.nPassword, 8);
    if (cmdRecv->Param_Num == 0)
    {
        if (!StorageSaveSystemContent(&glbSystem))
        {
            return CyFalse;
        }
    }
    else
    {
        if (glbSystem.nisDevicePassOK == CyTrue)
        {
            CyU3PMemCopy((uint8_t *)&glbSystem, (uint8_t *)&cmdRecv->Params[0], sizeof(glbSystem));
            CyU3PMemCopy((uint8_t *)glbSystem.nPassword, (uint8_t *)nPass, 8);
        }
        else
        {
            CyU3PMemCopy((uint8_t *)&glbSystem, (uint8_t *)&cmdRecv->Params[0], sizeof(glbSystem));
        }
        if (!StorageSaveSystemContent(&glbSystem))
        {
            return CyFalse;
        }
    }
    return CyTrue;
}

// �����豸������,�Ӷ��豸�����������س���
CyBool_t exe_save_dev_password(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    int i;
    fill_command_char(cmdSend, 's', 'v', 'd', 'p');
    if (cmdRecv->Param_Num != 8)
    {
        return CyFalse;
    }
    for (i = 0; i < 8; i++)
    {
        glbSystem.nPassword[i] = cmdRecv->Params[i];
    }
    if (!StorageSaveSystemContent(&glbSystem))
    {
        return CyFalse;
    }
    return CyTrue;
}

// �豸�ĵȼ�
CyBool_t exe_dev_grade(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'd', 'v', 'g', 'd');
    if (cmdRecv->Param_Num == 0)
    {
        cmdSend->Params[0] = glbSystem.n485ID;
        cmdSend->Param_Num = 1;
    }
    else if (cmdRecv->Param_Num == 1)
    {
        if (cmdRecv->Params[0] > 254)
        {
            return CyFalse;
        }
        glbSystem.n485ID = cmdRecv->Params[0];
        if (!StorageSaveSystemContent(&glbSystem))
        {
            return CyFalse;
        }
    }
    return CyTrue;
}

// ��ȡ�û����������ݣ� Params[0] nIndex Params[1] ��������
CyBool_t exe_save_dev_sn(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 's', 'd', 's', 'n');
    if (cmdRecv->Param_Num == 0)
    {
        cmdSend->Params[0] = glbSystem.serialNumber;
        cmdSend->Param_Num = 1;
    }
    else
    {
        glbSystem.serialNumber = cmdRecv->Params[0];
        if (!StorageSaveSystemContent(&glbSystem))
        {
            return CyFalse;
        }
    }
    return CyTrue;
}

// NOTE : fpga reg
// ����fpga�Ĵ���ֵ: [1] ��ַ [2] ֵ (����������Ų����Ļ�,˳��������ȥ,һ�����������Ĵ���)
CyBool_t exe_set_FPGA_reg(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    int i;
    uint8_t *pBuffer;
    fill_command_char(cmdSend, 's', 'f', 'r', 0);
    if (cmdRecv->Param_Num < 2)
    {
    	//Debug +++
    	Debug_manul_reset();
    	return CyTrue;
//        return CyFalse;
    }

    if (cmdRecv->Param_Num == 2)
    {
        fpga_reg_write((uint16_t)cmdRecv->Params[0], (uint16_t *)&cmdRecv->Params[1], 1);
    }
    else
    {
        pBuffer = (uint8_t *)&cmdRecv->Params[1];
        for (i = 0; i < 2 * (cmdRecv->Param_Num - 1); i++)
        {
            fpga_reg_write((uint16_t)cmdRecv->Params[0] + i, (uint16_t *)(pBuffer + i * 2), 1);
        }
    }
    return CyTrue;
}

// ����sensor ���� fpga�Ĵ���ֵ: [1] ��ַ [2] ֵ (0x1000����Ϊfpga reg,����Ϊsensor reg,���Ա���Ĳ���ֵ)
CyBool_t exe_sensor_or_fpga_reg(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    int i;
    char *pByte = (char *)cmdRecv->Params;
    fill_command_char(cmdSend, 's', 'o', 'f', 'r');
    if (pByte[0] == 'a' && pByte[1] == 'l' && pByte[2] == 'l')
    {
        cmdSend->Param_Num = 0;
        for (i = 0; i < 198; i += 2)
        {
            // TODO :
            // if (glbCamParam.arSpecialSensorOrFpagRegSddrAdnValue[i] == 0)
            // {
            return CyTrue;
            // }
            // cmdSend->Params[cmdSend->Param_Num] = *((uint32_t *)&glbCamParam.arSpecialSensorOrFpagRegSddrAdnValue[i]);
            // cmdSend->Param_Num++;
        }
    }
    if (cmdRecv->Param_Num == 1)
    {
        for (i = 0; i < 100; i += 2)
        {
            // if (cmdRecv->Params[0] == glbCamParam.arSpecialSensorOrFpagRegSddrAdnValue[i])
            // {
            //     cmdSend->Param_Num = 1;
            //     cmdSend->Params[0] = glbCamParam.arSpecialSensorOrFpagRegSddrAdnValue[i + 1];
            return CyTrue;
            // }
        }
        return CyFalse;
    }
    else if (cmdRecv->Param_Num == 2)
    {
        // return FpgaOrSensorRegWrite(cmdRecv->Params[0], cmdRecv->Params[1]);
    }
    return CyTrue;
}

// �õ�fpga�Ĵ���ֵ: [1] ��ַ [2] ���� [3] ���� [4] ƫ����
CyBool_t exe_get_FPGA_reg(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    int i;
    uint16_t *pBuffer16;
    fill_command_char(cmdSend, 'g', 'f', 'r', 0);
    // �������gfr���������õ��궨��ͳ�ƽ��
    if ((cmdRecv->Param_Num == 1) && (cmdRecv->Params[0] == 0xFFFF))
    {
        cmdSend->Param_Num = 6;
        pBuffer16 = (uint16_t *)(&cmdSend->Params[0]);
        for (i = 0; i < 6; i++)
        {
            // TODO : fill FPGA_REG_AVG_YUV_AFT_WB_ADDR
            // fpga_reg_read(FPGA_REG_AVG_YUV_AFT_WB_ADDR + (16 + 7) * 3 + i * 48, &pBuffer16[i * 2], 1);
            // fpga_reg_read(FPGA_REG_AVG_YUV_AFT_WB_ADDR + (16 + 7) * 3 + i * 48 + 3, &pBuffer16[i * 2 + 1], 1);
        }
        return CyTrue;
    }
    if ((cmdRecv->Param_Num != 1) && (cmdRecv->Param_Num != 2) && (cmdRecv->Param_Num != 4))
    {

        return CyFalse;
    }
    if (cmdRecv->Param_Num == 1)
    {
        cmdSend->Param_Num = 2;
        cmdSend->Params[0] = cmdRecv->Params[0];
        fpga_reg_read((uint16_t)cmdRecv->Params[0], (uint16_t *)&cmdSend->Params[1], 1);
        CyU3PDebugPrint(4,"\nreg:%4x,value:%4x",(uint16_t)cmdRecv->Params[0],(uint16_t)cmdSend->Params[1]);
    }
    else if (cmdRecv->Param_Num == 2)
    {
        if (cmdRecv->Params[1] > 256)
        {
            return CyFalse;
        }
        cmdSend->Param_Num = 2 + (cmdRecv->Params[1] >> 1);
        cmdSend->Params[0] = cmdRecv->Params[0];
        cmdSend->Params[1] = cmdRecv->Params[1];
        fpga_reg_read((uint16_t)cmdRecv->Params[0], (uint16_t *)&cmdSend->Params[2], cmdRecv->Params[1]);
    }
    else if (cmdRecv->Param_Num == 4)
    {
        if (cmdRecv->Params[1] * cmdRecv->Params[2] > 256)
        {
            return CyFalse;
        }
        if (cmdRecv->Params[1] > 128)
        {
            return CyFalse;
        }
        if (cmdRecv->Params[2] > 128)
        {
            return CyFalse;
        }
        if (cmdRecv->Params[3] > 1024)
        {
            return CyFalse;
        }
        cmdSend->Param_Num = 2 + (cmdRecv->Params[1] >> 1) * cmdRecv->Params[2];
        cmdSend->Params[0] = cmdRecv->Params[0];
        cmdSend->Params[1] = cmdRecv->Params[1];
        for (i = 0; i < cmdRecv->Params[2]; i++)
        {
            fpga_reg_read((uint16_t)cmdRecv->Params[0] + i * cmdRecv->Params[3],
                          ((uint16_t *)&cmdSend->Params[2]) + i * cmdRecv->Params[1], cmdRecv->Params[1]);
        }
    }
    return CyTrue;
}

CyBool_t exe_set_io(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 's', 'i', 'o', 0);
    cmdSend->Param_Num = 0;
	if (cmdRecv->Param_Num != 2 || cmdRecv->Params[1] > 1)
	{
		return CyFalse;
	}

	if (cmdRecv->Param_Num == 2)
	{
        if(CyU3PGpioSetValue((uint8_t)cmdRecv->Params[0], cmdRecv->Params[1]))
        {
        	return CyFalse;
        }
	}
    return CyTrue;
}

CyBool_t exe_get_io(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'g', 'i', 'o', 0);
    cmdSend->Param_Num = 1;
    int ret_io = 0;

    if (cmdRecv->Param_Num != 1)
	{
		return CyFalse;
	}
    if (cmdRecv->Param_Num == 1)
	{
		if(CyU3PGpioGetValue((uint8_t)cmdRecv->Params[0], &ret_io))
		{
			return CyFalse;
		}
	}
    cmdSend->Params[0] = ret_io;
    return CyTrue;
}

CyBool_t exe_set_type_io(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 's', 't', 'i', 'o');
    uint16_t *pBuffer;
    cmdSend->Param_Num = 0;
    pBuffer = (uint16_t*)&cmdRecv->Params[1];
    if (cmdRecv->Param_Num == 6 && ((pBuffer[0]|pBuffer[1]|pBuffer[2]|pBuffer[3]) < 2)){
    	CyU3PGpioSimpleConfig_t con_gpioConfig;
    	CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
		con_gpioConfig.inputEn = pBuffer[0]&0x1;
		con_gpioConfig.driveLowEn = pBuffer[1]&0x1;
		con_gpioConfig.driveHighEn = pBuffer[2]&0x1;
		con_gpioConfig.outValue = pBuffer[3]&0x1;
		con_gpioConfig.intrMode = pBuffer[4];
		apiRetStatus =CyU3PGpioSetSimpleConfig((uint8_t)cmdRecv->Params[0], &con_gpioConfig);
		if (apiRetStatus  == CY_U3P_SUCCESS)
		{
			return CyTrue;
		}
    }
    return CyFalse;
}


extern uint32_t recv_cnt;
CyBool_t exe_fpga_reset(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    fill_command_char(cmdSend, 'f', 'r', 's', 't');
    cmdSend->Param_Num = 0;
    if(!fpga_init()){
    	return CyFalse;
    }
    return CyTrue;
}

///Note: +++++

CyBool_t exe_rdwr_grab_param(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
	fill_command_char(cmdSend,'e', 'e', 'm', 'b');
	int extraParamNum = (sizeof(tag_grab_config)-8) >> 2; // -8 ����ΪFX3��ͷ����һ��8�ֽڵ�У��β������Ҫ�ϴ� >> 2����Ϊһ������������4���ֽ�
	tag_grab_config *PcParam = NULL;
	//��ȡ��ǰģʽ�û�����
	if(cmdRecv->Param_Num == 0)//������
	{
		//�ظ�֮ǰ�ȸ���״̬
		// �Ƶ��߳��У�1s����1��flc �ڸ���,���Ҫ��ֱ�ӿ���
//		GrabParamUpdate();
		cmdSend->Param_Num = extraParamNum;
		CyU3PMemCopy((uint8_t*)(&cmdSend->Params[0]),(uint8_t*)(&grabconfParam),sizeof(tag_grab_config)-8);
	}
	else if(cmdRecv->Param_Num == extraParamNum) //д����
	{
			PcParam = ((tag_grab_config*)(&cmdRecv->Params[0]));
	        cmdSend->Param_Num = 0;
	        if(glbSystem.nisDevicePassOK == CyTrue)
	        {if(CyFalse == GrabParamCompareandSet(PcParam)) return CyFalse;}
	        else {CyU3PDebugPrint(4,"\n passwd wrong");}
	}
	else
	{
		return CyFalse;
	}

	return CyTrue;
}

/* ����ֵ���㣬param[0] 0x0001 ֡������㣬 0x0002 ccl����������� , 0x0004 cl1_clk_pll 0x0008 cl2_clk_pll 0x0010 cl3_clk_pll 0x0020 flc���㣬 0x0040 flc��������� 0x8000 ddr��λ */
CyBool_t exe_clear_count_num(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
	fill_command_char(cmdSend, 'e', 'a', 'i', '\0');
	uint16_t mainFuncRegValue = 0;
	//TODO ����ֵ����
	if(cmdRecv->Param_Num == 1)
	{
		CyU3PDebugPrint(4,"\nexe_clear_count_1 happened");
		mainFuncRegValue |= (uint16_t)(cmdRecv->Params[0]);
		fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&mainFuncRegValue,1);
		//��ʱ1ms,���Ĵ�����λ
		CyU3PThreadSleep(1);
		mainFuncRegValue &= (~((uint16_t)(cmdRecv->Params[0])));
		fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&mainFuncRegValue,1);
	}
	// ��������λ��߲���������Ϊ1,����ȫ��
	else
	{
		CyU3PDebugPrint(4,"\nexe_clear_count_all happened");
		mainFuncRegValue = 0x807F;
		fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&mainFuncRegValue,1);
		//��ʱ1ms,���Ĵ�����λ
		CyU3PThreadSleep(1);
		mainFuncRegValue = 0;
		fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&mainFuncRegValue,1);
	}
	cmdSend->Param_Num = 0;

	return CyTrue;
}

/*2����ͼ����ͷ�� 1 ����ͼ����ͷ��   0ʵ��ͼ*/
CyBool_t exe_set_test_mode(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
	fill_command_char(cmdSend,'s', 't', 'm', 'd');
	if(cmdRecv->Param_Num == 0)
	{
		fpga_reg_read(TEST_PATTERN_REG_ADDRESS,(uint16_t *)(&cmdSend->Params[0]),1);
		cmdSend->Param_Num = 1;
		return CyTrue;
	}
	else if(cmdRecv->Param_Num == 1)
	{
		fpga_reg_write(TEST_PATTERN_REG_ADDRESS,(uint16_t *)(&cmdRecv->Params[0]),1);
		cmdSend->Param_Num = 0;
		return CyTrue;
	}
	CyU3PDebugPrint(4,"\nParam_Num error");
	return CyFalse;
}


/* ccl ���� ֹͣ����*/
CyBool_t exe_set_trigger(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
	fill_command_char(cmdSend, 's', 't', 'r', 'i');
	//todo ��������Ϊ1 ֱ��д�Ĵ�������
	if(cmdRecv->Param_Num == 1)
	{
		cmdSend->Param_Num = 0;
		fpga_reg_write(CC1_EN_ADDRESS,(uint16_t *)(&cmdRecv->Params[0]),1);
	}
	else
	{
		return CyFalse;
	}
	return CyTrue;
}

/* �ر�/�� ���fpga */
CyBool_t exe_start(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
	fill_command_char(cmdSend,'s','t','a','t');
	if(cmdRecv->Param_Num == 0)
	{
		if(glbCheckDogEnable == CyTrue)glbCheckDogEnable = CyFalse;
		return CyTrue;
	}
	return CyFalse;
}
CyBool_t exe_stop(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
	fill_command_char(cmdSend,'s','t','o','p');
	if(cmdRecv->Param_Num == 0)
	{
		if(glbCheckDogEnable == CyFalse)glbCheckDogEnable = CyTrue;
		return CyTrue;
	}
	return CyFalse;
}


cmd_tag_t cmd_tag[] __attribute__((aligned(32))) =
    {
        // NOTE : system
        {0, {'h', 0, 0, 0}},        // ����ָ��,������������Ϊ�ؼ���
        {1, {'b', 'o', 'o', 't'}},  // ����ϵͳ
        {2, {'g', 'f', 'v', 0}},    // �õ�FPGA����汾��:�����ո�ʽ
        {3, {'g', 'm', 'v', 0}},    // �õ�MCU�汾��:�����ո�ʽ
        {4, {'c', 'o', 'm', 'm'}},  // �趨����ͨ�ŵĶ���rs485ͨ����ֻ����һ����豸:  [1] MCU �� DSP
        {5, {'c', 'o', 'm', 'p'}},  // �趨����ͨ�ŵĲ����ʺ���żУ��
        {6, {'r', 's', 'i', 'd'}},  // ��ȡϵͳΨһID,���ô�ID�����з�����Ӧ�ü���,����8���ֽ�
        {7, {'s', 's', 'p', 'd'}},  // ����ϵͳ�Ľ������벻����: д��8���ֽڵļ�������
        {8, {'r', 'p', 0, 0}},      // ��ȡϵͳ����,����ϵͳ�����ṹ������
        {9, {'w', 'p', 0, 0}},      // д��ϵͳ����,���������б���Ϊϵͳ�����ṹ������
        {10, {'g', 's', 0, 0}},     // �õ�ϵͳ״̬,����0��ʾ״̬����,����Ϊ����
        {11, {'h', 'o', 'l', 'd'}}, // ͣ����ʱһ��ʱ��,��λms���趨���ڷ�������ʱ:  [1] ͣ��ʱ����

        // NOTE : flash��д
        {12, {'f', 'e', 'r', 0}},   // ����flash  [1] secor ��Ŵ�0��ʼ����  ����: 0 = param, 5 = gamma,
                                    //      10 = ch-lut, 51 = start cmd, 55 = lut-gain & offset, (60 = temp&gain)
        {13, {'f', 'p', 'r', 0}},   // ��дflash: [1] ��ʼ��ַ [2] �ֽڳ��� [3]... ����д����ֽ�
        {14, {'f', 'r', 'e', 0}},   // ��ȡflash: [1] ��ʼ��ַ [2] �ֽڳ���, ��������ǰ��������һ��,����Ϊʵ������
        {15, {'f', 's', 'e', 'l'}}, // flash����ѡ��,1Ϊ�����洢flash,0Ϊfpga����洢flash��
        {16, {'s', 'v', 'u', 0}},   // ���浱ǰ�趨�������û�������: [1] ���������0-2(user param save)
        {17, {'s', 'd', 'u', 0}},   // ���õ�ǰĬ���������صĲ��������ĸ�
        {18, {'c', 'f', 'a', 'u'}}, // ������������Լ��û�����
        {19, {'s', 'd', 'i', 'f'}}, // �����豸��Ϣ,����Ϊ��,���߲����ṹ������
        {20, {'s', 'v', 'd', 'p'}}, // �����豸������,�Ӷ��豸�����������س���
        {21, {'d', 'v', 'g', 'd'}}, // �豸�ĵȼ�
        {22, {'s', 'd', 's', 'n'}}, // "��ȡ�豸��Ϣ����ȡ�����ṹ������(read  device info system)"

        //NOTE : +++
        {38, {'e', 'e', 'm', 'b'}},	//����/��ȡ �û�������Ϣ
        {42, {'e', 'a', 'i', '\0'}}, //����ֵ����

        // NOTE : fpga reg
        {55, {'s', 'f', 'r', 0}},   // ����fpga�Ĵ���ֵ: [1] ��ַ [2] ֵ (����������Ų����Ļ�,˳��������ȥ,һ�����������Ĵ���)
        {56, {'s', 'o', 'f', 'r'}}, // ����sensor ���� fpga�Ĵ���ֵ: [1] ��ַ [2] ֵ (0x1000����Ϊfpga reg,����Ϊsensor reg,���Ա���Ĳ���ֵ)
        {57, {'g', 'f', 'r', 0}},   // �õ�fpga�Ĵ���ֵ: [1] ��ַ [2] ���� [3] ���� [4] ƫ����

        // NOTE : io
//        {58, {'s', 'i', 'o', 0}},   //����io�ܽŵ�ֵ
//        {59, {'s', 't', 'i', 'o'}},   //��ʼ��io�ܽŵ�����
        {58,{'s','t','o','p'}},
        {59,{'s','t','a','t'}},
        {60, {'g', 'i', 'o', 0}},   //��ȡio�ܽŵ�ֵ
        // NOTE: +++
        {80,{'s','t','r','i'}},
        //NOTE : +++
        {91, {'s', 't', 'm', 'd'}}, //����ͼģʽ

        {99, {'f', 'r', 's', 't'}}, // fpga reset
        };  

CyBool_t CmdHexExecute(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend, uint32_t hex_code)
{
	/*���ڿصƣ���λ��û����ʱ������*/
	if(first_cmd == 1)
	{
		first_cmd = 0;
		qtConnectedState = CyTrue;
	}
    switch (hex_code)
    {
    case 0:
    {
        // ����ָ��,������������Ϊ�ؼ���
        return exe_h(cmdRecv, cmdSend);
    }
    case 1:
    {
        // ����ϵͳ
        return exe_boot(cmdRecv, cmdSend);
    }
    case 2:
    {
        // �õ�FPGA����汾��:�����ո�ʽ
        return exe_get_FPGA_version(cmdRecv, cmdSend);
    }
    case 3:
    {
        // �õ�MCU�汾��:�����ո�ʽ
        return exe_get_software_version(cmdRecv, cmdSend);
    }
    case 4:
    {
        // �趨����ͨ�ŵĶ���rs485ͨ����ֻ����һ����豸:  [1] MCU �� DSP
        return exe_set_comm_obj(cmdRecv, cmdSend);
    }
    case 5:
    {
        // �趨����ͨ�ŵĲ����ʺ���żУ��
        return exe_set_comm_para(cmdRecv, cmdSend);
    }
    case 6:
    {
        // ��ȡϵͳΨһID,���ô�ID�����з�����Ӧ�ü���,����8���ֽ�
        return exe_read_system_ID(cmdRecv, cmdSend);
    }
    case 7:
    {
        // ����ϵͳ�Ľ������벻����: д��8���ֽڵļ�������
        return exe_set_system_password(cmdRecv, cmdSend);
    }
    case 8:
    {
        // ��ȡϵͳ����,����ϵͳ�����ṹ������
        return exe_read_para(cmdRecv, cmdSend);
    }
    case 9:
    {
        // д��ϵͳ����,���������б���Ϊϵͳ�����ṹ������
        return exe_write_para(cmdRecv, cmdSend);
    }
    case 10:
    {
        // �õ�ϵͳ״̬,����0��ʾ״̬����,����Ϊ����
        qtDisconnectCount++;
        return exe_get_status(cmdRecv, cmdSend);
    }
    case 11:
    {
        // ͣ����ʱһ��ʱ��,��λms���趨���ڷ�������ʱ:  [1] ͣ��ʱ����
        return exe_hold(cmdRecv, cmdSend);
    }
    case 12:
    {
        // ����flash  [1] secor ��Ŵ�0��ʼ����
        return exe_erase_flash_sector(cmdRecv, cmdSend);
    }
    case 13:
    {
        // ��дflash: [1] ��ʼ��ַ [2] �ֽڳ��� [3]... ����д����ֽ�
        return exe_program_flash(cmdRecv, cmdSend);
    }
    case 14:
    {
        // ��ȡflash: [1] ��ʼ��ַ [2] �ֽڳ���, ��������ǰ��������һ��,����Ϊʵ������
        return exe_read_flash(cmdRecv, cmdSend);
    }
    case 15:
    {
        // flash����ѡ��, 1Ϊ�����洢flash, 0Ϊfpga����洢flash��
        return exe_flash_select(cmdRecv, cmdSend);
    }
    case 16:
    {
        // ���浱ǰ�趨�������û�������: [1] ���������0-2(user param save)
        return exe_save_user_para(cmdRecv, cmdSend);
    }
    case 17:
    {
        // ���õ�ǰĬ���������صĲ��������ĸ�
        return exe_set_default_user(cmdRecv, cmdSend);
    }
    case 18:
    {
        // ������������Լ��û�����
        return exe_clear_factory_and_user_para(cmdRecv, cmdSend);
    }
    case 19:
    {
        // �����豸��Ϣ,����Ϊ��,���߲����ṹ������
        return exe_save_dev_info(cmdRecv, cmdSend);
    }
    case 20:
    {
        // �����豸������,�Ӷ��豸�����������س���
        return exe_save_dev_password(cmdRecv, cmdSend);
    }
    case 21:
    {
        // �豸�ĵȼ�
        return exe_dev_grade(cmdRecv, cmdSend);
    }
    case 22:
    {
        // ��ȡ�豸��Ϣ,��ȡ�����ṹ������
        return exe_save_dev_sn(cmdRecv, cmdSend);
    }

    case 38:
    {
    	//��/д �û����ò���
    	return exe_rdwr_grab_param(cmdRecv,cmdSend);
    }

    case 42:
    {
    	//����ֵ����
    	return exe_clear_count_num(cmdRecv,cmdSend);
    }

    case 55:
    {
        // ����fpga�Ĵ���ֵ: [1] ��ַ [2] ֵ (����������Ų����Ļ�,˳��������ȥ,һ�����������Ĵ���)
        return exe_set_FPGA_reg(cmdRecv, cmdSend);
    }
    case 56:
    {
        // ����sensor ���� fpga�Ĵ���ֵ: [1] ��ַ [2] ֵ (0x1000����Ϊfpga reg,����Ϊsensor reg,���Ա���Ĳ���ֵ)
        return exe_sensor_or_fpga_reg(cmdRecv, cmdSend);
    }
    case 57:
    {
        // �õ�fpga�Ĵ���ֵ: [1] ��ַ [2] ���� [3] ���� [4] ƫ����
        return exe_get_FPGA_reg(cmdRecv, cmdSend);
    }
    case 58:
	{
		// ����io�ܽŵ�ֵ
//		return exe_set_io(cmdRecv, cmdSend);
		return exe_stop(cmdRecv,cmdSend);
	}
    case 59:
	{
		// ��ʼ��io�ܽŵ�����
//		return exe_set_type_io(cmdRecv, cmdSend);
		return exe_start(cmdRecv,cmdSend);
	}
    case 60:
	{
		// ��ȡio�ܽŵ�ֵ
		return exe_get_io(cmdRecv, cmdSend);
	}
    case 80:
    {
    	// ����/ֹͣ ccl����
    	return exe_set_trigger(cmdRecv,cmdSend);
    }
    case 91:
    {
    	//����ͼģʽ
    	return exe_set_test_mode(cmdRecv, cmdSend);
    }

    case 99: 
    {
        // fpga reset
        return exe_fpga_reset(cmdRecv, cmdSend);
    }
    default:
    {
        return CyFalse;
    }
    }
}

CyBool_t CmdFrameExecute(tagCmdFormatterContent *cmdRecv, tagCmdFormatterContent *cmdSend)
{
    uint32_t i;
    uint8_t cnt = sizeof(cmd_tag) / sizeof(cmd_tag[0]);
    for (i = 0; i < cnt; i++)
    {
        if (app_cmd_Str_Cmp(cmdRecv->Command_char, cmd_tag[i].asc_code) == CyTrue)
        {
            return CmdHexExecute(cmdRecv, cmdSend, cmd_tag[i].hex_code);
        }
    }
    return CyFalse;
}
