/*file
********************************************************************************
<PRE>
模块名       : 
文件名       : 
相关文件     : 
文件实现功能 : 采集系统的参数存储的配置管理
作者         : 

--------------------------------------------------------------------------------
备注         : 

		外部调用方法
--------------------------------------------------------------------------------
修改记录 : 
日 期:      2025.6.24
版本 :		v0.0
修改人:
修改内容 :

</PRE>
********************************************************************************

* 版权所有(c) , <>, 保留所有权利
*******************************************************************************/

#include "app_grab_cfg.h"
#include "mcu_spi.h"

// 全局使用的采集参数
tag_grab_config grabconfParam = GRAB_PARAM_DEFAULT_VALUE;
uint32_t grabsysStatus = 0;


/*function
********************************************************************************
<PRE>
函数名   :
功能     : 存储/读取用户参数区内容，设置默认加载用户参数区是哪个
参数     :
	tagCameraParam *pParam 参数存储的结构体定义
		nIndex  用户参数的存储区0~..
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t GrabWriteUserParam(tag_grab_config *pParam)//存储当前参数到用户区
{
	int nIndex = pParam->n_device_type;
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * 大于模式的个数的处理逻辑
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

CyBool_t GrabSetDefaultUserParam( int nIndex )//设置默认存储区
{
	return CyTrue;
}

CyBool_t GrabReadUserParam(tag_grab_config *pParam)//读取当前用户区参数
{
	uint64_t header = 0;
	uint8_t nIndex = grabconfParam.n_device_type;
	if(nIndex >= GRAB_SYSTEM_MODE_NUMBER)
	{
		/*
		 * 大于模式的个数的处理逻辑
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
