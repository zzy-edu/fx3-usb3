/*
 ## Cypress USB 3.0 Platform header file (cyfxslfifosync.h)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2010-2011,
 ##  All Rights Reserved
 ##  UNPUBLISHED, LICENSED SOFTWARE.
 ##
 ##  CONFIDENTIAL AND PROPRIETARY INFORMATION
 ##  WHICH IS THE PROPERTY OF CYPRESS.
 ##
 ##  Use of this file is governed
 ##  by the license agreement included in the file
 ##
 ##     <install>/license/license.txt
 ##
 ##  where <install> is the Cypress software
 ##  installation root directory path.
 ##
 ## ===========================
*/

/* This file contains the constants and definitions used by the Slave FIFO application example */

#ifndef _INCLUDED_CYFXSLFIFOASYNC_H_
#define _INCLUDED_CYFXSLFIFOASYNC_H_

#include "cyu3externcstart.h"
#include "cyu3types.h"
#include "cyu3usbconst.h"
#include "cyu3uart.h"

#define cdc

#define CY_FX_SLFIFO_DMA_TX_SIZE        (0)	                  /* DMA transfer size is set to infinite */
#define CY_FX_SLFIFO_DMA_RX_SIZE        (0)	                  /* DMA transfer size is set to infinite */
#define CY_FX_SLFIFO_THREAD_STACK       (0x1000)              /* Slave FIFO application thread stack size */
#define CY_FX_SLFIFO_THREAD_PRIORITY    (8)                   /* Slave FIFO application thread priority */

/* Endpoint and socket definitions for the Slave FIFO application */

/* To change the Producer and Consumer EP enter the appropriate EP numbers for the #defines.
 * In the case of IN endpoints enter EP number along with the direction bit.
 * For eg. EP 6 IN endpoint is 0x86
 *     and EP 6 OUT endpoint is 0x06.
 * To change sockets mention the appropriate socket number in the #defines. */

/* Note: For USB 2.0 the endpoints and corresponding sockets are one-to-one mapped
         i.e. EP 1 is mapped to UIB socket 1 and EP 2 to socket 2 so on */

#define CY_FX_EP_CONSUMER				0x81   /* EP 1 IN */

#define CY_FX_EP_CONSUMER_SOCKET        CY_U3P_UIB_SOCKET_CONS_1    /* Socket 1 is consumer */

/* Used with FX3 Silicon. */
#define CY_FX_PRODUCER_PPORT_0_SOCKET    CY_U3P_PIB_SOCKET_0    /* GPIF Socket 1 is producer. */

//ͻ��ģʽ����:�����ڳ�������.���ͻ��ģʽ��������USB����������,���ֵΪ16,��СֵΪ1.

/* Burst length in 1 KB packets. Only applicable to USB 3.0. */
#ifndef CY_FX_EP_BURST_LENGTH

/* TODO: ͻ��ģʽ���ȿ���Ϊ1~16.ֵԽСUSB����������Ҳ��ԽС.�ı�ͻ������Ҳ��ͬʱ���Ļ�������С */
#define CY_FX_EP_BURST_LENGTH           (8)
#endif

/*TODO: ����DMA����ϵ����ı仺������С*/
#define CY_FX_DMA_MULTIPLIER			(4)

/* DMA��������С,����ֵΪ2���Ķ˵�ͻ����С. */
#ifndef CY_FX_BULKSRCSINK_DMA_BUF_SIZE
#define CY_FX_BULKSRCSINK_DMA_BUF_SIZE  (CY_FX_DMA_MULTIPLIER * CY_FX_EP_BURST_LENGTH * 1024)
#endif

/* DMAͨ����ʹ�õĻ���������.  */
/* ֱ�ӿ����������͵Ĵ��������͵����ݻ�浽��������Ҳ��Ӧ�������ĸ���  */
#ifndef CY_FX_BULKSRCSINK_DMA_BUF_COUNT
#define CY_FX_BULKSRCSINK_DMA_BUF_COUNT (2)
#endif

/* Extern definitions for the USB Descriptors */
extern const uint8_t CyFxUSB20DeviceDscr[];
extern const uint8_t CyFxUSB30DeviceDscr[];
extern const uint8_t CyFxUSBDeviceQualDscr[];
extern const uint8_t CyFxUSBFSConfigDscr[];
extern const uint8_t CyFxUSBHSConfigDscr[];
extern const uint8_t CyFxUSBBOSDscr[];
extern const uint8_t CyFxUSBSSConfigDscr[];
extern const uint8_t CyFxUSBStringLangIDDscr[];
extern const uint8_t CyFxUSBManufactureDscr[];
extern const uint8_t CyFxUSBProductDscr[];

extern CyU3PUartConfig_t glUartConfig;
/* ��������Ϊ�˵��Ե�ʱ��ͨ��sfr �ֶ�����ͨ�� ���������ɾ��*/
void CyFxSlFifoApplnStart(void);
void CyFxSlFifoApplnStop(void);
CyBool_t TryResetChannel(void);

/* �������������������豸 */
void RestartDevice(void);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYFXSLFIFOASYNC_H_ */

/*[]*/
