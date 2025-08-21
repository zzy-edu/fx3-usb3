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

void StorageGetHard_ID( uint8_t* nBuffer)
{
	MCUFlashReadProtectID(nBuffer);
	return;
}




/*function
********************************************************************************
<PRE>
函数名   :
功能     : md5加密算法
参数     :
		nlen:欲加密的数据长度，不能超过448个
		pbuff:欲加的数据缓冲区首地址
		presult:加密后数据输出需要预先分配4字节缓冲
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :

典型用法 : 实时调整
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define RL(x, y) (((x) << (y)) | ((x) >> (32 - (y))))  //x向左循环移y位

#define FF(a, b, c, d, x, s, ac) a = b + (RL((a + F(b,c,d) + x + ac),s))
#define GG(a, b, c, d, x, s, ac) a = b + (RL((a + G(b,c,d) + x + ac),s))
#define HH(a, b, c, d, x, s, ac) a = b + (RL((a + H(b,c,d) + x + ac),s))
#define II(a, b, c, d, x, s, ac) a = b + (RL((a + I(b,c,d) + x + ac),s))
/*
static UINT32 II(UINT32 a,UINT32  b, UINT32 c,UINT32  d,UINT32  x, UINT32 s, UINT32 ac)
{
	return b + (RL((a + I(b,c,d) + x + ac),s));
}
*/
CyBool_t md5_ES(uint32_t nLen,uint8_t *pBuff,uint8_t *pResult)
{


	uint8_t buff[128];
	uint32_t A,B,C,D,a,b,c,d,i,x[16];   //
	if(nLen>=128-64)	return CyFalse;
	//????????
	for(i=0;i<nLen;i++)
	{
		buff[i]=pBuff[i];
	}
	buff[nLen]=1;
	for(i=nLen+1;i<128-4;i++)buff[i]=0;
	*(uint32_t *)(&buff[128-4])=nLen;
	//?????
    	A=0x67452301,B=0xefcdab89,C=0x98badcfe,D=0x10325476; //????????????
	for(i=0;i<128/64;i++)
	{
		a=A,b=B,c=C,d=D;
		memcpy((void *)x,(void *)(buff+i*64),64);
		//   **** Round 1 ****
		FF (a, b, c, d, x[ 0],  7, 0xd76aa478); //   **** 1 ****
		FF (d, a, b, c, x[ 1], 12, 0xe8c7b756); //   **** 2 ****
		FF (c, d, a, b, x[ 2], 17, 0x242070db); //   **** 3 ****
		FF (b, c, d, a, x[ 3], 22, 0xc1bdceee); //   **** 4 ****
		FF (a, b, c, d, x[ 4],  7, 0xf57c0faf); //   **** 5 ****
		FF (d, a, b, c, x[ 5], 12, 0x4787c62a); //   **** 6 ****
		FF (c, d, a, b, x[ 6], 17, 0xa8304613); //   **** 7 ****
		FF (b, c, d, a, x[ 7], 22, 0xfd469501); //   **** 8 ****
		FF (a, b, c, d, x[ 8],  7, 0x698098d8); //   **** 9 ****
		FF (d, a, b, c, x[ 9], 12, 0x8b44f7af); //   **** 10 ****
		FF (c, d, a, b, x[10], 17, 0xffff5bb1); //   **** 11 ****
		FF (b, c, d, a, x[11], 22, 0x895cd7be); //   **** 12 ****
		FF (a, b, c, d, x[12],  7, 0x6b901122); //   **** 13 ****
		FF (d, a, b, c, x[13], 12, 0xfd987193); //   **** 14 ****
		FF (c, d, a, b, x[14], 17, 0xa679438e); //   **** 15 ****
		FF (b, c, d, a, x[15], 22, 0x49b40823); //   **** 16 ****

		//   **** Round 2 ****
		GG (a, b, c, d, x[ 1],  5, 0xf61e2562); //   **** 17 ****
		GG (d, a, b, c, x[ 6],  9, 0xc040b340); //   **** 18 ****
		GG (c, d, a, b, x[11], 14, 0x265e5a51); //   **** 19 ****
		GG (b, c, d, a, x[ 0], 20, 0xe9b6c7aa); //   **** 20 ****
		GG (a, b, c, d, x[ 5],  5, 0xd62f105d); //   **** 21 ****
		GG (d, a, b, c, x[10],  9, 0x02441453); //   **** 22 ****
		GG (c, d, a, b, x[15], 14, 0xd8a1e681); //   **** 23 ****
		GG (b, c, d, a, x[ 4], 20, 0xe7d3fbc8); //   **** 24 ****
		GG (a, b, c, d, x[ 9],  5, 0x21e1cde6); //   **** 25 ****
		GG (d, a, b, c, x[14],  9, 0xc33707d6); //   **** 26 ****
		GG (c, d, a, b, x[ 3], 14, 0xf4d50d87); //   **** 27 ****
		GG (b, c, d, a, x[ 8], 20, 0x455a14ed); //   **** 28 ****
		GG (a, b, c, d, x[13],  5, 0xa9e3e905); //   **** 29 ****
		GG (d, a, b, c, x[ 2],  9, 0xfcefa3f8); //   **** 30 ****
		GG (c, d, a, b, x[ 7], 14, 0x676f02d9); //   **** 31 ****
		GG (b, c, d, a, x[12], 20, 0x8d2a4c8a); //   **** 32 ****

		//   **** Round 3 ****
		HH (a, b, c, d, x[ 5],  4, 0xfffa3942); //   **** 33 ****
		HH (d, a, b, c, x[ 8], 11, 0x8771f681); //   **** 34 ****
		HH (c, d, a, b, x[11], 16, 0x6d9d6122); //   **** 35 ****
		HH (b, c, d, a, x[14], 23, 0xfde5380c); //   **** 36 ****
		HH (a, b, c, d, x[ 1],  4, 0xa4beea44); //   **** 37 ****
		HH (d, a, b, c, x[ 4], 11, 0x4bdecfa9); //   **** 38 ****
		HH (c, d, a, b, x[ 7], 16, 0xf6bb4b60); //   **** 39 ****
		HH (b, c, d, a, x[10], 23, 0xbebfbc70); //   **** 40 ****
		HH (a, b, c, d, x[13],  4, 0x289b7ec6); //   **** 41 ****
		HH (d, a, b, c, x[ 0], 11, 0xeaa127fa); //   **** 42 ****
		HH (c, d, a, b, x[ 3], 16, 0xd4ef3085); //   **** 43 ****
		HH (b, c, d, a, x[ 6], 23, 0x04881d05); //   **** 44 ****
		HH (a, b, c, d, x[ 9],  4, 0xd9d4d039); //   **** 45 ****
		HH (d, a, b, c, x[12], 11, 0xe6db99e5); //   **** 46 ****
		HH (c, d, a, b, x[15], 16, 0x1fa27cf8); //   **** 47 ****
		HH (b, c, d, a, x[ 2], 23, 0xc4ac5665); //   **** 48 ****

		//   **** Round 4 ****
		II (a, b, c, d, x[ 0],  6, 0xf4292244); //   **** 49 ****
		II (d, a, b, c, x[ 7], 10, 0x432aff97); //   **** 50 ****
		II (c, d, a, b, x[14], 15, 0xab9423a7); //   **** 51 ****
		II (b, c, d, a, x[ 5], 21, 0xfc93a039); //   **** 52 ****
		II (a, b, c, d, x[12],  6, 0x655b59c3); //   **** 53 ****
		II (d, a, b, c, x[ 3], 10, 0x8f0ccc92); //   **** 54 ****
		II (c, d, a, b, x[10], 15, 0xffeff47d); //   **** 55 ****
		II (b, c, d, a, x[ 1], 21, 0x85845dd1); //   **** 56 ****
		II (a, b, c, d, x[ 8],  6, 0x6fa87e4f); //   **** 57 ****
		II (d, a, b, c, x[15], 10, 0xfe2ce6e0); //   **** 58 ****
		II (c, d, a, b, x[ 6], 15, 0xa3014314); //   **** 59 ****
		II (b, c, d, a, x[13], 21, 0x4e0811a1); //   **** 60 ****
		II (a, b, c, d, x[ 4],  6, 0xf7537e82); //   **** 61 ****
		II (d, a, b, c, x[11], 10, 0xbd3af235); //   **** 62 ****
		II (c, d, a, b, x[ 2], 15, 0x2ad7d2bb); //   **** 63 ****
		II (b, c, d, a, x[ 9], 21, 0xeb86d391); //   **** 64 ****
/*
		a=II (a, b, c, d, x[ 0],  6, 0xf4292244); //   **** 49 ****
		d=II (d, a, b, c, x[ 7], 10, 0x432aff97); //   **** 50 ****
		c=II (c, d, a, b, x[14], 15, 0xab9423a7); //   **** 51 ****
		b=II (b, c, d, a, x[ 5], 21, 0xfc93a039); //   **** 52 ****
		a=II (a, b, c, d, x[12],  6, 0x655b59c3); //   **** 53 ****
		d=II (d, a, b, c, x[ 3], 10, 0x8f0ccc92); //   **** 54 ****
		c=II (c, d, a, b, x[10], 15, 0xffeff47d); //   **** 55 ****
		b=II (b, c, d, a, x[ 1], 21, 0x85845dd1); //   **** 56 ****
		a=II (a, b, c, d, x[ 8],  6, 0x6fa87e4f); //   **** 57 ****
		d=II (d, a, b, c, x[15], 10, 0xfe2ce6e0); //   **** 58 ****
		c=II (c, d, a, b, x[ 6], 15, 0xa3014314); //   **** 59 ****
		b=II (b, c, d, a, x[13], 21, 0x4e0811a1); //   **** 60 ****
		a=II (a, b, c, d, x[ 4],  6, 0xf7537e82); //   **** 61 ****
		d=II (d, a, b, c, x[11], 10, 0xbd3af235); //   **** 62 ****
		c=II (c, d, a, b, x[ 2], 15, 0x2ad7d2bb); //   **** 63 ****
		b=II (b, c, d, a, x[ 9], 21, 0xeb86d391); //   **** 64 ****
*/
		A += c ;
		B += a;
		C += b;
		D += d;
	}
	*(uint32_t *)(pResult)=(A)+(B)+(C)+(D);

	return CyTrue;
}



CyBool_t StorageCheckPassword(uint8_t* id,uint8_t *pPassword)
{
	uint8_t nBuffer[8]={0,0,0,0,0,0,0,0};
	uint8_t nBuffer2[8]={0,0,0,0,0,0,0,0};
	int i;
	memcpy(nBuffer,id,8);
	//return 1;

	i = nBuffer[0];
	nBuffer[0] = nBuffer[6];
	nBuffer[6] = i&0xff;

	md5_ES(8,nBuffer,&nBuffer2[0]);

	i = nBuffer[4];
	nBuffer[4] = nBuffer[5];
	nBuffer[5] = i&0xff;

	md5_ES(8,nBuffer,&nBuffer2[4]);

	for(i = 0;i<8;i++)
	if(nBuffer2[i] != pPassword[i])
		return CyFalse;

	return CyTrue;
}
