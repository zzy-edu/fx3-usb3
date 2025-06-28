#include "fx3_spi.h"
#include "fx3_pin_define.h"
#include <cyu3gpio.h>
#include <cyu3spi.h>
//CyU3PReturnStatus_t CyFxSpiInit(void)
//{
//    CyU3PSpiConfig_t spiConfig;
//    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
//
//    status = CyU3PSpiInit();
//    if (status != CY_U3P_SUCCESS)
//    {
//        return status;
//    }
//
//    CyU3PMemSet((uint8_t *)&spiConfig, 0, sizeof(spiConfig));
//    spiConfig.isLsbFirst = CyFalse;
//    spiConfig.cpol = CyTrue;
//    spiConfig.ssnPol = CyFalse;
//    spiConfig.cpha = CyTrue;
//    spiConfig.leadTime = CY_U3P_SPI_SSN_LAG_LEAD_HALF_CLK;
//    spiConfig.lagTime = CY_U3P_SPI_SSN_LAG_LEAD_HALF_CLK;
//    spiConfig.ssnCtrl = CY_U3P_SPI_SSN_CTRL_FW;
//    spiConfig.clock = 8000000;
//    spiConfig.wordLen = 8;
//
//    status = CyU3PSpiSetConfig(&spiConfig, NULL);
//
//    return status;
//}

CyBool_t FxIOSpiWriteRead(uint8_t *outBuffer, uint32_t outCnt, uint8_t *inBuffer, uint32_t inOffset, uint32_t inCnt)
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
    CyU3PGpioSetValue(FPGA_SPI_CS, CyFalse);
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
            CyU3PGpioSetValue(FPGA_SPI_SIMO, (outTmp >> 7) & 1);
            CyU3PGpioSetValue(FPGA_SPI_CLK, CyFalse);
            CyU3PGpioGetValue(FPGA_SPI_SOMI, &getTmp);
            CyU3PGpioSetValue(FPGA_SPI_CLK, CyTrue);
            inTmp += getTmp;
            outTmp <<= 1;
        }
        if (i >= inOffset && i < inCnt + inOffset && NULL != inBuffer)
        {
            inBuffer[i - inOffset] = inTmp;
        }
    }
    CyU3PGpioSetValue(FPGA_SPI_CS, CyTrue);
    return CyTrue;
}
