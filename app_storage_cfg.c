/*file
********************************************************************************
<PRE>
ģ����       : 
�ļ���       : 
����ļ�     : 
�ļ�ʵ�ֹ��� : ϵͳ�Ĳ����洢�����ù���
����         : 

--------------------------------------------------------------------------------
��ע         : 

		�ⲿ���÷���
--------------------------------------------------------------------------------
�޸ļ�¼ : 
�� ��:      2012.4.9
�汾 :		v1.0
�޸���:
�޸����� :

</PRE>
********************************************************************************

* ��Ȩ����(c) , <>, ��������Ȩ��
*******************************************************************************/

#include "app_storage_cfg.h"
#include "version.h"

const uint32_t SOFTWARE_VERSION = SOFT_UPDATE_DATE;
tagCameraParam glbCamParam = CAM_PARAM_DEFAULT_VALUE;

//ȫ��ʹ�õĳ����̶�����
tagSystemContent glbSystem = SYSTEM_CONTENT_DEFAULT_VALUE;
CyBool_t StorageSetDefaultUserParam(int nIndex) // ����Ĭ�ϲ�����
{
	uint32_t t[2];
	t[0] = SYSTEM_PARAM_VALID_CODE;
	t[1] = nIndex;
	if (!MCUFlashEraseAndWrite(0, (uint8_t *)t, CAM_PARAM_DEFAULT_USER_INDEX, 8))
		return CyFalse;
	return CyTrue;
}
/*function
********************************************************************************
<PRE>
������   : 
����     : �洢/��ȡϵͳ�����Ĺ̶�����
����     : 
		pContent   �������ݵ��׵�ַ
����ֵ   :  
�׳��쳣 : 
--------------------------------------------------------------------------------
��ע     : 
�����÷� : 
--------------------------------------------------------------------------------
����     : 
</PRE>
*******************************************************************************/
CyBool_t StorageSaveSystemContent(tagSystemContent *pContent)
{
	return MCUFlashEraseAndWrite(0, (uint8_t *)pContent, (FLASH_START_SECTOR + 4) * FX3_FLASH_SECTOR_SIZE, sizeof(tagSystemContent));
}
CyBool_t StorageReadSystemContent(tagSystemContent *pContent)
{
	char cTitle[4];
	MCUSpiFlashRead(0, (FLASH_START_SECTOR + 4) * FX3_FLASH_SECTOR_SIZE, (uint8_t *)cTitle, 4);
	if ((cTitle[0] != 'm') || (cTitle[1] != 'o') || (cTitle[2] != 'r') || (cTitle[3] != 'e'))
		return CyFalse;

	return MCUSpiFlashRead(0, (FLASH_START_SECTOR + 4) * FX3_FLASH_SECTOR_SIZE, (uint8_t *)pContent, sizeof(tagSystemContent));
}
