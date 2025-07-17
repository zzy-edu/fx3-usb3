#ifndef _APP_VIRTUAL_UART_H_
#define _APP_VIRTUAL_UART_H_

#include "cyu3types.h"
#include "stddef.h"
#include "cyu3usb.h"
#include "cyu3dma.h"


extern CyBool_t globUartConfig;
extern CyBool_t glIsCdcInActive;

// Endpoint and socket definitions for the virtual COM port
#define CY_FX_EP_PRODUCER_CDC           	(0x02)	// EP 2 OUT
#define CY_FX_EP_CONSUMER_CDC           	(0x82)	// EP 2 IN
#define CY_FX_EP_INTERRUPT_CDC          	(0x83)	// EP 3 INTR
#define CY_FX_EP_PRODUCER_CDC_SOCKET    	(CY_U3P_UIB_SOCKET_PROD_2)
#define CY_FX_EP_CONSUMER_CDC_SOCKET    	(CY_U3P_UIB_SOCKET_CONS_2)
#define CY_FX_EP_INTERRUPT_CDC_SOCKET   	(CY_U3P_UIB_SOCKET_CONS_3)
#define CY_FX_USBUART_THREAD_PRIORITY		(8)
#define CY_FX_USBUART_THREAD_STACK			(0x1000)


/* CDC通道
 * glChHandleUarttoUart cdc ep producer 和  cdc ep consumer 之间的透传
 * glChHandleUarttoUsb	cdc ep producer 和 CY_U3P_LPP_SOCKET_UART_CONS 之间的透传
 * glChHandleUsbtoUart	CY_U3P_LPP_SOCKET_UART_PROD 和 cdc ep consumer 之间的透传
 * 现在只用了glChHandleUarttoUart
 */
CyU3PDmaChannel   glChHandleUsbtoUart;          /* DMA AUTO (USB TO UART) channel handle.*/
CyU3PDmaChannel   glChHandleUarttoUsb;          /* DMA AUTO_SIG(UART TO USB) channel handle.*/
CyU3PDmaChannel   glChHandleUarttoUart;         /* only test DMA AUTO (UART TO UART) channel handle.*/


/* CDC Class specific requests to be handled by this application. */
#define SET_LINE_CODING        0x20
#define GET_LINE_CODING        0x21
#define SET_CONTROL_LINE_STATE 0x22


void CyFxUSBUARTAppStop (void);
void CyFxUSBUARTAppStart(void);

void CdcChannelTryStop(void);

void DebugInitUsingCDC(void);
void DebugDeInitStartCDC(void);

#endif //_APP_VIRTUAL_UART_H_
