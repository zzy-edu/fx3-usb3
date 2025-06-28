#ifndef _INCLUDED_MCU_SPI_H_
#define _INCLUDED_MCU_SPI_H_
#include <cyu3error.h>
#include "flash_map.h"


#define	CMD_READ_ID         (0x4B)
#define	CMD_WRITE_ENABLE    (0x06)
#define	CMD_PROGRAM_SECTOR  (0x02)
#define	CMD_ERASE_SECTOR    (0x20)
#define	CMD_ERASE_CHIP      (0x60)
#define	CMD_READ_FLASH      (0x03)
#define	CMD_READ_STATUS     (0x05)


CyBool_t MCUFlashEraseAndWrite(uint8_t nFlashNumber, uint8_t *pBuffer, uint32_t nStartAddr, uint32_t nLen);
CyBool_t MCUSpiWriteRead(uint8_t *outBuffer, uint32_t outCnt, uint8_t *inBuffer, uint32_t inOffset, uint32_t inCnt);
CyBool_t MCUSpiFlashWrite(uint8_t nFlashNumber, uint8_t *pSrc, uint32_t dst, uint32_t nLen);
CyBool_t MCUSpiFlashRead(uint8_t nFlashNumber, uint32_t pSrc, uint8_t *pDst, uint32_t nLen);
CyBool_t MCUFlashEraseSector(uint8_t nFlashNumber, uint32_t n);
CyBool_t MCUFlashReadProtectID(uint8_t *dst);

#endif
