/*file
********************************************************************************
<PRE>
ģ����       : 
�ļ���       : 
����ļ�     : 
�ļ�ʵ�ֹ��� : �ɼ�ϵͳ�Ĳ����洢�����ù���
����         : 

--------------------------------------------------------------------------------
��ע         : 

		�ⲿ���÷���
--------------------------------------------------------------------------------
�޸ļ�¼ : 
�� ��:      2025.6.24
�汾 :		v0.0
�޸���:
�޸����� :

</PRE>
********************************************************************************

* ��Ȩ����(c) , <>, ��������Ȩ��
*******************************************************************************/

#include "app_grab_cfg.h"
#include "mcu_spi.h"

// ȫ��ʹ�õĲɼ�����
tag_grab_config grabconfParam = GRAB_PARAM_DEFAULT_VALUE;
uint32_t grabsysStatus = 0;


/*function
********************************************************************************
<PRE>
������   :
����     : �洢/��ȡ�û����������ݣ�����Ĭ�ϼ����û����������ĸ�
����     :
	tagCameraParam *pParam �����洢�Ľṹ�嶨��
		nIndex  �û������Ĵ洢��0~..
����ֵ   :
�׳��쳣 :
--------------------------------------------------------------------------------
��ע     :
�����÷� :
--------------------------------------------------------------------------------
����     :
</PRE>
*******************************************************************************/
CyBool_t GrabWriteUserParam(tag_grab_config *pParam)//�洢��ǰ�������û���
{
	int nIndex = pParam->n_device_type;
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * ����ģʽ�ĸ����Ĵ����߼�
		 */
		return CyFalse;
	}

	pParam->header = PARAM_VALID_HCODE;
	pParam->tailer = PARAM_VALID_TCODE;
	if(CyFalse == MCUFlashEraseAndWrite(0,(uint8_t*)pParam,GRAB_MODE1_PARAM_ADDR(nIndex),sizeof(tag_grab_config)))
	{
		return CyFalse;
	}
	return CyTrue;
}

CyBool_t GrabSetDefaultUserParam( int nIndex )//����Ĭ�ϴ洢��
{
	return CyTrue;
}

CyBool_t GrabReadUserParam(tag_grab_config *pParam)//��ȡ��ǰ�û�������
{
	uint64_t header = 0;
	uint8_t nIndex = grabconfParam.n_device_type;
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * ����ģʽ�ĸ����Ĵ����߼�
		 */
		return CyFalse;
	}
	if(CyFalse == MCUSpiFlashRead(0,GRAB_MODE1_PARAM_ADDR(nIndex),(uint8_t*)&header,8))
	{
		return CyFalse;
	}

	if(header != PARAM_VALID_HCODE)
	{
		CyU3PDebugPrint(4,"\nheader error, return Default");
		CyU3PMemCopy((uint8_t*)pParam,(uint8_t*)(&grabconfParam),sizeof(tag_grab_config)-8);
		return CyTrue;
	}
	else
	{
		if(CyTrue == MCUSpiFlashRead(0,GRAB_MODE1_PARAM_ADDR(nIndex),(uint8_t*)pParam,sizeof(tag_grab_config)))
		{
			if(pParam->tailer != PARAM_VALID_TCODE)
			{
				CyU3PDebugPrint(4,"\ntailer error, return Default");
				CyU3PMemCopy((uint8_t*)pParam,(uint8_t*)(&grabconfParam),sizeof(tag_grab_config)-8);
				return CyTrue;
			}
		}
		else
		{
			return CyFalse;
		}
	}
	return CyTrue;
}
