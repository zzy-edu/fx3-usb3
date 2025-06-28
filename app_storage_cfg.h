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

#ifndef _APP_STORAGE_CFG_H_
#define _APP_STORAGE_CFG_H_
#include "mcu_spi.h"

#define SYSTEM_PARAM_VALID_CODE 0x44504300 // 当存储区开始为此code的时候表示该片code有效

#define PARAM_ERROR_CODE 0XEFEF // 填充的内容，判断为错误的

/*
flash  存储结构的设定为

每个sector  4k

sector 1 : CameraParam 1 	出厂参数组
sector 2 : CameraParam 2	用户参数组1
sector 3 : CameraParam 3	用户参数组2
sector 4 : CameraParam 4	用户参数组3

// 1组用户使用的gamma表格
sector 5: gamma LUT 

// 多通道平衡查找表
共32KB
sector 13~20

*/
#define CAM_PARAM_DEFAULT_USER_INDEX (FLASH_START_SECTOR * FX3_FLASH_SECTOR_SIZE)

typedef struct
{
	uint32_t codeValid;	  // 表示当前这组参数是否有效 值为0x44504300的时候表示有效
	uint32_t nMcuVersion; // 两个版本号上电初始化时候填入，依据固定程序而来
	uint32_t nFpgaVersion;
} __attribute__((aligned(32))) tagCameraParam;

#define CAM_PARAM_DEFAULT_VALUE \
	{                           \
		SYSTEM_PARAM_VALID_CODE,       \
			0x20210810,         \
			0x00fc08,           \
	}
extern tagCameraParam glbCamParam;
extern const uint32_t SOFTWARE_VERSION;
// 系统记录的内容出厂后不变
typedef struct
{
	char arTitle[4]; //固定的记录区域头
	uint8_t nPassword[8];
	char arDeviceSubName[20]; //设备的二级名称
	uint32_t serialNumber;	  // 序列号
	uint16_t nDeviceGrade;	  // 设备的级别号0 :民用1: 工业2: 专用
	uint16_t nProductDate[3]; // 设备生产日期/年/月/日
	uint32_t n485ID;		  //序列号进行485通信的时候，此id号为设备的id号。定义为1-254，0为广播id，255为全员禁声id
	uint8_t nDeviceUniqID[8]; //设备的唯一ID
	uint8_t nisDevicePassOK;  //是否设备id校验成功
	uint8_t nReserved[71];
} __attribute__((aligned(32))) tagSystemContent;
#define SYSTEM_CONTENT_DEFAULT_VALUE \
	{                                \
		{'m', 'o', 'r', 'e'},        \
		{0},                     	 \
		{0},                     	 \
		1,                       	 \
		2,                       	 \
		{0x2021, 0x08, 0x10},    	 \
		0,                       	 \
		{0},                     	 \
		0,                       	 \
		{0}                          \
	}
extern tagSystemContent glbSystem;
CyBool_t StorageSetDefaultUserParam(int nIndex); // 设置默认参数区
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
CyBool_t StorageSaveSystemContent(tagSystemContent *pContent);
CyBool_t StorageReadSystemContent(tagSystemContent *pContent);
#endif
