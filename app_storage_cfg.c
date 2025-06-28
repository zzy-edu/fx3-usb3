/*file
********************************************************************************
<PRE>
模块名       : 
文件名       : 
相关文件     : 
文件实现功能 : 系统的参数存储的配置管理
作者         : 

--------------------------------------------------------------------------------
备注         : 

		外部调用方法
--------------------------------------------------------------------------------
修改记录 : 
日 期:      2012.4.9
版本 :		v1.0
修改人:
修改内容 :

</PRE>
********************************************************************************

* 版权所有(c) , <>, 保留所有权利
*******************************************************************************/

#include "app_storage_cfg.h"
#include "version.h"

const uint32_t SOFTWARE_VERSION = SOFT_UPDATE_DATE;
tagCameraParam glbCamParam = CAM_PARAM_DEFAULT_VALUE;

//全局使用的出厂固定参数
tagSystemContent glbSystem = SYSTEM_CONTENT_DEFAULT_VALUE;
CyBool_t StorageSetDefaultUserParam(int nIndex) // 设置默认参数区
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
函数名   : 
功能     : 存储/获取系统出厂的固定参数
参数     : 
		pContent   参数数据的首地址
返回值   :  
抛出异常 : 
--------------------------------------------------------------------------------
备注     : 
典型用法 : 
--------------------------------------------------------------------------------
作者     : 
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
