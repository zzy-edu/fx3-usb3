#ifndef _INCLUDED_FX3_SPI_H_
#define _INCLUDED_FX3_SPI_H_
#include <cyu3error.h>
/* 等效频率482.2 kHz */
CyU3PReturnStatus_t CyFxSpiInit(void);
CyBool_t FxIOSpiWriteRead(uint8_t *outBuffer, uint32_t outCnt, uint8_t *inBuffer, uint32_t inOffset, uint32_t inCnt);
#endif
