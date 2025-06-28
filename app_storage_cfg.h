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

#ifndef _APP_STORAGE_CFG_H_
#define _APP_STORAGE_CFG_H_
#include "mcu_spi.h"

#define SYSTEM_PARAM_VALID_CODE 0x44504300 // ���洢����ʼΪ��code��ʱ���ʾ��Ƭcode��Ч

#define PARAM_ERROR_CODE 0XEFEF // �������ݣ��ж�Ϊ�����

/*
flash  �洢�ṹ���趨Ϊ

ÿ��sector  4k

sector 1 : CameraParam 1 	����������
sector 2 : CameraParam 2	�û�������1
sector 3 : CameraParam 3	�û�������2
sector 4 : CameraParam 4	�û�������3

// 1���û�ʹ�õ�gamma���
sector 5: gamma LUT 

// ��ͨ��ƽ����ұ�
��32KB
sector 13~20

*/
#define CAM_PARAM_DEFAULT_USER_INDEX (FLASH_START_SECTOR * FX3_FLASH_SECTOR_SIZE)

typedef struct
{
	uint32_t codeValid;	  // ��ʾ��ǰ��������Ƿ���Ч ֵΪ0x44504300��ʱ���ʾ��Ч
	uint32_t nMcuVersion; // �����汾���ϵ��ʼ��ʱ�����룬���ݹ̶��������
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
// ϵͳ��¼�����ݳ����󲻱�
typedef struct
{
	char arTitle[4]; //�̶��ļ�¼����ͷ
	uint8_t nPassword[8];
	char arDeviceSubName[20]; //�豸�Ķ�������
	uint32_t serialNumber;	  // ���к�
	uint16_t nDeviceGrade;	  // �豸�ļ����0 :����1: ��ҵ2: ר��
	uint16_t nProductDate[3]; // �豸��������/��/��/��
	uint32_t n485ID;		  //���кŽ���485ͨ�ŵ�ʱ�򣬴�id��Ϊ�豸��id�š�����Ϊ1-254��0Ϊ�㲥id��255ΪȫԱ����id
	uint8_t nDeviceUniqID[8]; //�豸��ΨһID
	uint8_t nisDevicePassOK;  //�Ƿ��豸idУ��ɹ�
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
CyBool_t StorageSetDefaultUserParam(int nIndex); // ����Ĭ�ϲ�����
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
CyBool_t StorageSaveSystemContent(tagSystemContent *pContent);
CyBool_t StorageReadSystemContent(tagSystemContent *pContent);
#endif
