#include "mcu_spi.h"
#include "fx3_pin_define.h"
#include <cyu3gpio.h>
#include <cyu3spi.h>

/*! @function
********************************************************************************
<PRE>
函数名  : MCUSpiWriteRead
功能     : GOIO模拟SPI读写
参数     :		outBuffer: 要写入数据的存放内存地址
			outCnt:写入的字节数
			inBuffer:读出数据的存放内存地址
			inOffset:读出的数据存放内存地址的偏移值
			inCnt:要读出的字节数
返回值   : 	CY_U3P_ERROR_BAD_POINTER 写入或读出数据的地址都为NULL
		CY_U3P_SUCCESS 写入或读出成功
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/

CyBool_t MCUSpiWriteRead(uint8_t *outBuffer, uint32_t outCnt, uint8_t *inBuffer, uint32_t inOffset, uint32_t inCnt)
{
    uint32_t i;
    uint8_t j;
    uint32_t byteCnt;
    uint8_t outTmp = 0;
    uint8_t inTmp = 0;
    CyBool_t getTmp = 0;
    if (NULL == outBuffer && NULL != inBuffer)
    {
        byteCnt = inOffset + inCnt;
    }
    else if (NULL == inBuffer && NULL != outBuffer)
    {
        byteCnt = outCnt;
    }
    else if (NULL != inBuffer && NULL != outBuffer)
    {
        if (inOffset + inCnt > outCnt)
        {
            byteCnt = inOffset + inCnt;
        }
        else
        {
            byteCnt = outCnt;
        }
    }
    else
    {
        return CyFalse;
    }

    CyU3PGpioSetValue(MCU_SPI_SS2, CyFalse);
    for (i = 0; i < byteCnt; i++)
    {
        if (NULL != outBuffer && i < outCnt)
        {
            outTmp = outBuffer[i];
        }
        else
        {
            outTmp = 0;
        }
        for (j = 0; j < 8; j++)
        {
            inTmp <<= 1;
            CyU3PGpioSetValue(MCU_SPI_SDO2, (outTmp >> 7) & 1);
            CyU3PGpioSetValue(MCU_SPI_SCK2, CyTrue);
            CyU3PGpioGetValue(MCU_SPI_SDI2, &getTmp);
            CyU3PGpioSetValue(MCU_SPI_SCK2, CyFalse);
            inTmp += getTmp;
            outTmp <<= 1;
        }
        if (i >= inOffset && i < inCnt + inOffset && NULL != inBuffer)
        {
            inBuffer[i - inOffset] = inTmp;
        }
    }
    CyU3PGpioSetValue(MCU_SPI_SS2, CyTrue);

	return CyTrue;
}

/*! @function
********************************************************************************
<PRE>
函数名  : MCUSpiWaitForStatus
功能     : 判断flash是否繁忙
参数     :
            nFlashNumber: 设备中的flash编号，目前只能是0或者1
返回值   : CyTrue 忙，CyFalse 空闲
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/

static CyBool_t Fx3IOFlashIsBusy(void)
{
	uint8_t wrBuffer[1];
	uint8_t rdBuffer[1];
	rdBuffer[0] = 0x01;
	wrBuffer[0] = 0x05;// read status1 ,bit0 is busy

	if(!MCUSpiWriteRead(wrBuffer, 1, rdBuffer,1, 1))
		return CyFalse;

	if((rdBuffer[0] & 1) ==1)
	{
		return CyTrue;
	}
	else return CyFalse;
}


/*! @function
********************************************************************************
<PRE>
函数名  : MCUSpiFlashWrite
功能     : 向flash写入数据
参数     : dst指向flash中的地址
            pSrc写入的数据缓冲区的首地址
            nlen数据长度,不能超过256,而且要在一个sector内部的地址
            nFlashNumber: 设备中的flash编号，目前只能是0或者1
返回值   :	CyTrue 成功
		CyFalse 失败
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t MCUSpiFlashWrite(uint8_t nFlashNumber, uint8_t *pSrc, uint32_t dst, uint32_t nLen)
{
    uint32_t cnt;
    uint8_t location[4 + 256];
    uint32_t nLenTmp[2], dstTmp[2], iTimes, i;
    uint8_t *pSrcTmp[2];
    int iCount;
    int nLenOnce;

    if ((dst + nLen) > FX3_FLASH_LEN)
    {
        return CyFalse;
    }

    iCount = nLen;

    while (iCount > 0)
    {
        if (iCount > 32)
        {
            nLen = 32;
            nLenOnce = 32;
        }
        else
        {
            nLen = iCount;
            nLenOnce = iCount;
        }

        // 默认存储一次
        nLenTmp[0] = nLen;
        dstTmp[0] = dst;
        iTimes = 1;
        pSrcTmp[0] = pSrc;

        for (i = 1; i < nLen; i++)
        {
            if (((dst + i) & 0xff) == 0)
            {
                iTimes = 2;
                nLenTmp[0] = i;
                nLenTmp[1] = nLen - i;

                dstTmp[0] = dst;
                dstTmp[1] = dst + i;

                pSrcTmp[0] = pSrc;
                pSrcTmp[1] = pSrc + i;
                break;
            }
        }

        for (i = 0; i < iTimes; i++)
        {
            nLen = nLenTmp[i];
            dst = dstTmp[i];
            pSrc = pSrcTmp[i];

            // -- 等待flash空闲----
			cnt = 50000;
			while(cnt-- > 1)
			{
				CyFx3BusyWait(10);
				if(!Fx3IOFlashIsBusy()) break;
			}
			if(cnt<=2) // time is too long
			{
				return CyFalse;
			}

			CyFx3BusyWait(10);
			// 写入write enable
			location[0] = CMD_WRITE_ENABLE;// enable
			if(!MCUSpiWriteRead(location, 1, NULL,0, 0)){
		    	return CyFalse;
			}

			CyFx3BusyWait(10);

            location[0] = CMD_PROGRAM_SECTOR;
            location[1] = (dst >> 16) & 0xFF;
            location[2] = (dst >> 8) & 0xFF;
            location[3] = dst & 0xFF;
            CyU3PMemCopy(&location[4], pSrc, nLen);
            if(!MCUSpiWriteRead(location, 4 + nLen,NULL,0,0))
            {
                return CyFalse;
            }
            CyFx3BusyWait(10);
        } // while 存储

        //这里的作用是回读校验，保证写入flash的数据是正确的
        //这是第一种实现的方式， 一边写一边校验
        //第二种实现方式是写完后重新读一遍，算一个校验码，两边只比较一个校验码就可以
        if (!MCUSpiFlashRead(0, dst, location, nLen))
        {
            return CyFalse;
        }
        for (cnt = 0; cnt < nLen; cnt++)
        {
            if (pSrc[cnt] != location[cnt])
            {
                return CyFalse;
            }
        }

        iCount -= nLenOnce;
        pSrc += nLenOnce;
        dst += nLenOnce;
    } // while
    return CyTrue;
}
/*! @function
********************************************************************************
<PRE>
函数名  : MCUSpiFlashRead
功能     : 从flash中读取数据
参数     : pdst指向本地的缓冲区地址
            pSrc欲读取的flash内部地址
            nlen数据长度
            nFlashNumber: 设备中的flash编号，目前只能是0或者1
返回值   :	CyTrue 成功
		CyFalse 失败
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t MCUSpiFlashRead(uint8_t nFlashNumber, uint32_t pSrc, uint8_t *pDst, uint32_t nLen)
{
    uint8_t location[4];
    if ((pSrc + nLen) > FX3_FLASH_LEN)
    {
        return CyFalse;
    }

	// -- 等待flash空闲----
	uint32_t cnt = 5000;
	while(cnt -->1)
	{
		CyFx3BusyWait(10);
		if(!Fx3IOFlashIsBusy()) break;
	}
	if(cnt<=2) // time is too long
	{
		return CyFalse;
	}

	cnt = 0;
	int nReadOnce = 0;
	while(cnt<nLen)
	{
		if(nLen-cnt>32)
		{
			nReadOnce=32;
		}
		else
		{
			nReadOnce = nLen-cnt;
		}

	    location[0] = CMD_READ_FLASH;
	    location[1] = (pSrc >> 16) & 0xFF;
	    location[2] = (pSrc >> 8) & 0xFF;
	    location[3] = pSrc & 0xFF;

		if(!MCUSpiWriteRead(location, 4, pDst+cnt,4, nReadOnce)){
	    	return CyFalse;
		}


		cnt+=nReadOnce;
		pSrc+=nReadOnce;

	}
    return CyTrue;
}


/*! @function
********************************************************************************
<PRE>
函数名   :
功能     : 擦除指定区域的内容
参数     : n指定的第n个扇区,从0开始算第一个扇区
            nFlashNumber: 设备中的flash编号，目前只能是0或者1
返回值   :
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t MCUFlashEraseSector(uint8_t nFlashNumber, uint32_t n)
{
	uint32_t cnt = 0;
    uint32_t temp = 0;
    uint8_t location[32];

    if (n > (FX3_FLASH_LEN / FX3_FLASH_SECTOR_SIZE))
    {
        return CyFalse;
    }


    cnt = 50000;
	while(cnt -->1)
	{
		CyFx3BusyWait(10);
		if(!Fx3IOFlashIsBusy()) break;
	}

	if(cnt<=2) // time is too long
	{
		return CyFalse;
	}

    location[0] = CMD_WRITE_ENABLE;
    if(!MCUSpiWriteRead(location, 1,NULL,0,0))
    {
        return CyFalse;
    }

    CyFx3BusyWait(1000);

    temp = FX3_FLASH_SECTOR_SIZE * n;
    location[0] = CMD_ERASE_SECTOR;
    location[1] = (temp >> 16) & 0xFF;
    location[2] = (temp >> 8) & 0xFF;
    location[3] = temp & 0xFF;

    if(!MCUSpiWriteRead(location, 4,NULL,0,0))
    {
        return CyFalse;
    }

	cnt = 50000;
	while(cnt-- >1)
	{
		CyFx3BusyWait(10);
		if(!Fx3IOFlashIsBusy()) break;
	}
	if(cnt<=2) // time is too long
	{
		return CyFalse;
	}

    if (!MCUSpiFlashRead(0, FX3_FLASH_SECTOR_SIZE * n, location, 16))
    {
        return CyFalse;
    }
    if (!MCUSpiFlashRead(0, FX3_FLASH_SECTOR_SIZE * (n + 1) - 16, &location[16], 16))
    {
        return CyFalse;
    }

    for (temp = 0; temp < 32; temp++)
    {
        if (0xFF != location[temp])
        {
            return CyFalse;
        }
    }

    return CyTrue;
}


/*! @function
********************************************************************************
<PRE>
函数名   :
功能     : 向flash中擦除并写入数据
参数     :
    nFlashNumber: 设备中的flash编号，目前只能是0或者1
    Uint32 nStartAddr,写入的开始flash地址0开始
    Uint8* pBuffer, 写入的缓冲区首地址
    int nLen//写入的数据长度

返回值   :success or not
抛出异常 :
--------------------------------------------------------------------------------
备注     :
典型用法 :
--------------------------------------------------------------------------------
作者     :
</PRE>
*******************************************************************************/
CyBool_t MCUFlashEraseAndWrite(uint8_t nFlashNumber, uint8_t *pBuffer, uint32_t nStartAddr, uint32_t nLen)
{
    uint32_t nStartSector; //要擦除的开始sector
    uint32_t nEndSector;   //

    //计算sector数量

    int i;

    nStartSector = nStartAddr / FX3_FLASH_SECTOR_SIZE;
    nEndSector = (nStartAddr + nLen - 1) / FX3_FLASH_SECTOR_SIZE;

    for (i = 0; i < nEndSector - nStartSector + 1; i++)
    {
        if (CyFalse == MCUFlashEraseSector(0, nStartSector + i))
        {
            return CyFalse;
        }
    }
    i = 0;
    while (nLen >= FX3_FLASH_WRITE_MAX_LEN)
    {
        if (CyFalse == MCUSpiFlashWrite(nFlashNumber, &pBuffer[i * FX3_FLASH_WRITE_MAX_LEN],
                                      nStartAddr + (i * FX3_FLASH_WRITE_MAX_LEN), FX3_FLASH_WRITE_MAX_LEN))
        {
        	return CyFalse;
        }


        nLen -= FX3_FLASH_WRITE_MAX_LEN;
        i++;
    }
    if (nLen <= 0)
    {
        return CyTrue;
    }

    if (CyFalse == MCUSpiFlashWrite(nFlashNumber, &pBuffer[i * FX3_FLASH_WRITE_MAX_LEN],
                                  nStartAddr + (i * FX3_FLASH_WRITE_MAX_LEN), nLen))
        return CyFalse;

    return CyTrue;
}


CyBool_t MCUFlashReadProtectID(uint8_t *dst)
{
    uint8_t tmp[4];
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
    uint8_t wrBuffer[1];

	// -- 等待flash空闲----
	uint32_t cnt = 5000;
	while(cnt -->1)
	{
		CyFx3BusyWait(10);
		if(!Fx3IOFlashIsBusy()) break;
	}
	if(cnt<=2) // time is too long
	{
		return CyFalse;
	}

    wrBuffer[0] = CMD_READ_ID;
    if(CyFalse == MCUSpiWriteRead(wrBuffer,1,dst,4,12))
    	return CyFalse;

    return CyTrue;
}
