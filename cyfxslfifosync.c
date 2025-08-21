/*
 ## Cypress USB 3.0 Platform source file (cyfxslfifosync.c)
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

/* This file illustrates the Slave FIFO Synchronous mode example */

/*
   This example comprises of two USB bulk endpoints. A bulk OUT endpoint acts as the
   producer of data from the host. A bulk IN endpoint acts as the consumer of data to
   the host. Appropriate vendor class USB enumeration descriptors with these two bulk
   endpoints are implemented.

   The GPIF configuration data for the Synchronous Slave FIFO operation is loaded onto
   the appropriate GPIF registers. The p-port data transfers are done via the producer
   p-port socket and the consumer p-port socket.

   This example implements two DMA Channels either in MANUAL mode or AUTO mode (depending
   on #define MANUAL in cyfxslfifosync.h) one for P to U data transfer and one for U to P
   data transfer.

   The U to P DMA channel connects the USB producer (OUT) endpoint to the consumer p-port
   socket. And the P to U DMA channel connects the producer p-port socket to the USB 
   consumer (IN) endpoint.

   In case of MANUAL DMA channel, Upon every reception of data in the DMA buffer from the
   host or from the p-port, the CPU is signaled using DMA callbacks. There are two DMA callback
   functions implemented each for U to P and P to U data paths. The CPU then commits the DMA buffer received so
   that the data is transferred to the consumer.

   In case of AUTO DMA channel, the data will be transferred between U and P ports automatically
   without any intervention from the CPU.

   The DMA buffer size is configured differently based on the usage. #define LOOPBACK_SHRT_ZLP in cyfxslfifosync.h
   configures the DMA buffers for doing data loopback. Also, DMA buffer for each channel is defined based on the
   USB speed. 64 bytes for full speed, 512 bytes for high speed and 1024 bytes for super speed.
   #define STREAM_IN_OUT in cyfxslfifosync.h configures the DMA buffers for higher bandwidth data transfers.
   16*64 bytes for full speed, 16*512 bytes for high speed and 16*1024 bytes for super speed.

   CY_FX_SLFIFO_DMA_BUF_COUNT_P_2_U in cyfxslfifosync.h defines the number of DMA buffers allocated for P to U data path.
   CY_FX_SLFIFO_DMA_BUF_COUNT_U_2_P in cyfxslfifosync.h defines the number of DMA buffers allocated for U to P data path.

   The constant CY_FX_SLFIFO_GPIF_16_32BIT_CONF_SELECT in the header file is used to
   select 16bit or 32bit GPIF data bus configuration.
 */
#include "fx3_pin_init.h"
#include "fpga_config.h"
#include "cyfxslfifosync.h"
#include "app_cmd_analysis.h"
#include "app_grab_cfg.h"
#include "cyu3dma.h"
#include "cyu3error.h"
#include "cyu3gpif.h"
#include "cyu3os.h"
#include "cyu3uart.h"
#include "cyu3usb.h"
#include "fx3_fifo.h"
#include "fx3_spi.h"
#include "pib_regs.h"
#include "fx3_pin_define.h"
#include "fx3_common.h"
#include "app_virtual_uart.h"
#include "app_storage_cfg.h"
#include <cyu3gpio.h>
#include <uart_regs.h>

/* This file should be included only once as it contains
 * structure definitions. Including it in multiple places
 * can result in linker error. */

/* 启动线程 */
CyU3PThread 	slFifoAppThread;          /* Slave FIFO application thread structure */
/* 串口通信线程，未使用 */
CyU3PThread     USBUARTAppThread;
/* 重启设备 */
CyU3PThread     RestartAppThread;

CyBool_t restartFlg = CyFalse;
CyBool_t restartKey = CyFalse;

CyU3PDmaChannel glChHandleSlFifoPtoU; /* DMA Channel handle for U2P transfer. */
CyBool_t glIsApplnActive = CyFalse; /* Whether the loopback application is active or not. */
uint8_t burstLength = 0;
uint16_t received_cnt = 0;
uint8_t glEp0Buffer[BUFF_SIZE] __attribute__ ((aligned(32)));
uint8_t glFifoBuffer[FIFO_DEEPTH_MAX_VALUE] __attribute__ ((aligned(32)));
uint8_t ledState = 0;
tagFifoParam glSendFifo __attribute__ ((aligned(32)));
CyBool_t isControlRun;

/*
 * 配置串口的结构体，串口助手连接虚拟串口时配置使用
 */
CyU3PUartConfig_t glUartConfig = {0};           /* Current UART configuration. */


void
CyFxUSBGPIFDmaCallback(
        CyU3PDmaChannel   *chHandle, /* Handle to the DMA channel. */
        CyU3PDmaCbType_t   type,     /* Callback type.             */
        CyU3PDmaCBInput_t *input)    /* Callback status.           */
{

    if (type == CY_U3P_DMA_CB_PROD_EVENT)
    {
    	CyU3PDebugPrint(4,"\nCY_U3P_DMA_CB_PROD_EVENT happen, buffer size=%d, buffer count=%d, status=%d",input->buffer_p.size, input->buffer_p.count,input->buffer_p.status);
        CyU3PDmaChannelCommitBuffer (chHandle, input->buffer_p.count, 0);
    }
}

/* This function starts the slave FIFO loop application. This is called
 * when a SET_CONF event is received from the USB host. The endpoints
 * are configured and the DMA pipe is setup in this function. */
void CyFxSlFifoApplnStart(
    void)
{
    uint16_t size = 0;
    CyU3PEpConfig_t epCfg;
    CyU3PDmaChannelConfig_t dmaCfg;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
    CyU3PUSBSpeed_t usbSpeed = CyU3PUsbGetSpeed();

    /* First identify the usb speed. Once that is identified,
     * create a DMA channel and start the transfer on this. */

    /* Based on the Bus Speed configure the endpoint packet size */
    switch (usbSpeed)
    {
    case CY_U3P_FULL_SPEED:
        size = 64;
        break;

    case CY_U3P_HIGH_SPEED:
        size = 512;
        burstLength = 1;
        break;

    case CY_U3P_SUPER_SPEED:
        size = 1024;
        burstLength = 16;
        break;

    default:
        CyFxAppErrorHandler(CY_U3P_ERROR_FAILURE);
        break;
    }

    CyU3PMemSet((uint8_t *)&epCfg, 0, sizeof(epCfg));
    epCfg.enable = CyTrue;
    epCfg.epType = CY_U3P_USB_EP_BULK;
    epCfg.burstLen = burstLength;
    epCfg.streams = 0;
    epCfg.pcktSize = size;

    /* Consumer endpoint configuration*/
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_CONSUMER, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Flush the Endpoint memory */
    CyU3PUsbFlushEp(CY_FX_EP_CONSUMER);


    /* Create a DMA AUTO channel for the producer socket. */
    dmaCfg.size = CY_FX_BULKSRCSINK_DMA_BUF_SIZE;
    dmaCfg.count = CY_FX_BULKSRCSINK_DMA_BUF_COUNT;
    dmaCfg.prodSckId = CY_FX_PRODUCER_PPORT_0_SOCKET;
    dmaCfg.consSckId = CY_FX_EP_CONSUMER_SOCKET;
    dmaCfg.dmaMode = CY_U3P_DMA_MODE_BYTE;
    /* No callback is required. */
    dmaCfg.notification = 0;//CY_U3P_DMA_CB_CONS_EVENT |CY_U3P_DMA_CB_PROD_EVENT
    dmaCfg.cb = NULL;//CyFxUSBGPIFDmaCallback
    dmaCfg.prodHeader = 0;
    dmaCfg.prodFooter = 0;
    dmaCfg.consHeader = 0;
    dmaCfg.prodAvailCount = 0;

    apiRetStatus = CyU3PDmaChannelCreate(&glChHandleSlFifoPtoU, CY_U3P_DMA_TYPE_AUTO, &dmaCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Set DMA channel transfer size. */
    apiRetStatus = CyU3PDmaChannelSetXfer(&glChHandleSlFifoPtoU, CY_FX_SLFIFO_DMA_TX_SIZE);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    glIsApplnActive = CyTrue;
}

/* This function stops the slave FIFO loop application. This shall be called
 * whenever a RESET or DISCONNECT event is received from the USB host. The
 * endpoints are disabled and the DMA pipe is destroyed by this function. */
void CyFxSlFifoApplnStop(
    void)
{
    CyU3PEpConfig_t epCfg;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    /* Update the flag. */
    glIsApplnActive = CyFalse;

    /* Flush the endpoint memory */
    CyU3PUsbFlushEp(CY_FX_EP_CONSUMER);


    /* Destroy the channel */
    CyU3PDmaChannelDestroy(&glChHandleSlFifoPtoU);
    /* Disable endpoints. */
    CyU3PMemSet((uint8_t *)&epCfg, 0, sizeof(epCfg));
    epCfg.enable = CyFalse;

    /* Consumer endpoint configuration*/
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_CONSUMER, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }
}

CyBool_t TryResetChannel(void)
{
	if(glIsApplnActive) return CyTrue;
	else return CyFalse;
}

/* Callback to handle the USB setup requests. */
CyBool_t
CyFxSlFifoApplnUSBSetupCB(
    uint32_t setupdat0,
    uint32_t setupdat1)
{
    /* Fast enumeration is used. Only requests addressed to the interface, class,
     * vendor and unknown control requests are received by this function.
     * This application does not support any class or vendor requests. */

    uint32_t i, len;
    uint8_t bRequest, bReqType;
    uint8_t bType, bTarget;
    uint16_t wValue, wIndex, wLength;
    uint16_t sendSize;
    CyBool_t isHandled = CyFalse;
    uint16_t readCount = 0;
    uint8_t config_data[7];
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
    CyU3PUartConfig_t uartConfig;
    uint16_t data = 1;

    /* Decode the fields from the setup request. */
    bReqType = (setupdat0 & CY_U3P_USB_REQUEST_TYPE_MASK);
    bType = (bReqType & CY_U3P_USB_TYPE_MASK);
    bTarget = (bReqType & CY_U3P_USB_TARGET_MASK);
    bRequest = ((setupdat0 & CY_U3P_USB_REQUEST_MASK) >> CY_U3P_USB_REQUEST_POS);
    wValue = ((setupdat0 & CY_U3P_USB_VALUE_MASK) >> CY_U3P_USB_VALUE_POS);
    wIndex = ((setupdat1 & CY_U3P_USB_INDEX_MASK) >> CY_U3P_USB_INDEX_POS);
    wLength = ((setupdat1 & CY_U3P_USB_LENGTH_MASK) >> CY_U3P_USB_LENGTH_POS);

    if (bType == CY_U3P_USB_STANDARD_RQT)
    {
        /* Handle SET_FEATURE(FUNCTION_SUSPEND) and CLEAR_FEATURE(FUNCTION_SUSPEND)
         * requests here. It should be allowed to pass if the device is in configured
         * state and failed otherwise. */
        if ((bTarget == CY_U3P_USB_TARGET_INTF) && ((bRequest == CY_U3P_USB_SC_SET_FEATURE) || (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE)) && (wValue == 0))
        {
            if (glIsApplnActive)
                CyU3PUsbAckSetup();
            else
                CyU3PUsbStall(0, CyTrue, CyFalse);

            isHandled = CyTrue;
        }

        /* CLEAR_FEATURE request for endpoint is always passed to the setup callback
         * regardless of the enumeration model used. When a clear feature is received,
         * the previous transfer has to be flushed and cleaned up. This is done at the
         * protocol level. Since this is just a loopback operation, there is no higher
         * level protocol. So flush the EP memory and reset the DMA channel associated
         * with it. If there are more than one EP associated with the channel reset both
         * the EPs. The endpoint stall and toggle / sequence number is also expected to be
         * reset. Return CyFalse to make the library clear the stall and reset the endpoint
         * toggle. Or invoke the CyU3PUsbStall (ep, CyFalse, CyTrue) and return CyTrue.
         * Here we are clearing the stall. */
        if ((bTarget == CY_U3P_USB_TARGET_ENDPT) && (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE)
                && (wValue == CY_U3P_USBX_FS_EP_HALT))
        {
            if (glIsApplnActive)
            {
				if (wIndex == CY_FX_EP_CONSUMER)
				{
						GrabStopFpgaWork();
						//1、拉住fpga
						CyFxSlFifoApplnStop();
						//2、清空ddr
						data = 0x8000;
						fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&data,1);
						//3、复位寄存器
						data = 1;
						fpga_reg_write(0x3340,&data,1);
						/* Give a chance for the main thread loop to run. */
						CyU3PThreadSleep(1);
						CyFxSlFifoApplnStart();
						CyU3PUsbStall(wIndex, CyFalse, CyTrue);
						CyU3PUsbAckSetup();
						isHandled = CyTrue;
						CyU3PThreadSleep(1);
						//1、复位ddr
						data = 0;
						fpga_reg_write(MAIN_FUNCTION_REG_ADDRESS,&data,1);
						//2、复位寄存器
						fpga_reg_write(0x3340,&data,1);
						//3、放开fpga
						GrabStartFpgaWork();
						CyU3PDebugPrint(4,"\nCY_U3P_USB_SC_CLEAR_FEATURE happened");
				}
            }
        }
    }

    /* Handle supported vendor requests. */
    if (bType == CY_U3P_USB_VENDOR_RQT)
    {
        isHandled = CyTrue;

        switch (bRequest)
        {
        case 0x42:
            if(isControlRun == CyFalse)
            {
                isControlRun = CyTrue;
            }
            else
            {
                break;
            }
			CyU3PMemSet(glEp0Buffer, 0, sizeof(glEp0Buffer));
            len = FifoGetDataNumber(&glSendFifo);
            sendSize = len > wLength ? wLength : len;
            for (i = 0; i < sendSize; i++)
            {
                glEp0Buffer[i] = FifoPop(&glSendFifo);
            }
            CyU3PUsbSendEP0Data(sendSize, glEp0Buffer);
            isControlRun = CyFalse;
            break;
        case 0xC2:
            if(isControlRun == CyFalse)
            {
                isControlRun = CyTrue;
            }
            else
            {
                break;
            }
            received_cnt = 0;
            status = CyU3PUsbGetEP0Data(wLength, glEp0Buffer, &received_cnt);
            if (status == CY_U3P_SUCCESS)
            {
                CmdAscAndHexRecv(glEp0Buffer, &glSendFifo);
            }
            isControlRun = CyFalse;
            break;
        default:
            /* This is unknown request. */
            isHandled = CyFalse;
            break;
        }

        /* If there was any error, return not handled so that the library will
         * stall the request. Alternatively EP0 can be stalled here and return
         * CyTrue. */
        if (status != CY_U3P_SUCCESS)
        {
            isHandled = CyFalse;
        }
    }


    #ifdef cdc
    /* Check for CDC Class Requests */
    if (bType == CY_U3P_USB_CLASS_RQT)
    {
        isHandled = CyTrue;

        /* CDC Specific Requests */
        /* set_line_coding */
        if (bRequest == SET_LINE_CODING)
        {
            status = CyU3PUsbGetEP0Data(0x07, config_data, &readCount);
            if (status != CY_U3P_SUCCESS)
            {
                CyFxAppErrorHandler(status);
            }
            if (readCount != 0x07)
            {
                CyFxAppErrorHandler(CY_U3P_ERROR_BAD_SIZE);
            }
            else
            {
                CyU3PMemSet ((uint8_t *)&uartConfig, 0, sizeof (uartConfig));
                uartConfig.baudRate = (CyU3PUartBaudrate_t)(config_data[0] | (config_data[1]<<8)|
                        (config_data[2]<<16)|(config_data[3]<<24));
                if (config_data[4] == 0)
                {
                    uartConfig.stopBit = CY_U3P_UART_ONE_STOP_BIT;
                }
                else if (config_data[4] == 2)
                {
                    uartConfig.stopBit = CY_U3P_UART_TWO_STOP_BIT;
                }
                else
                {
                    /* Give invalid value. */
                    uartConfig.stopBit = (CyU3PUartStopBit_t)0;
                }
                if (config_data[5] == 1)
                {
                    uartConfig.parity = CY_U3P_UART_ODD_PARITY;
                }
                else if (config_data[5] == 2)
                {
                    uartConfig.parity = CY_U3P_UART_EVEN_PARITY;
                }
                else
                {
                    /* 0 = no parity; any other value - invalid parity. */
                    uartConfig.parity = CY_U3P_UART_NO_PARITY;
                }

                uartConfig.txEnable = CyTrue;
                uartConfig.rxEnable = CyTrue;
                uartConfig.flowCtrl = CyFalse;
                uartConfig.isDma = CyTrue;

                /* Set the uart configuration */
                apiRetStatus = CyU3PUartSetConfig (&uartConfig, NULL);
                if (apiRetStatus == CY_U3P_SUCCESS)
                {
                    CyU3PMemCopy((uint8_t *)&glUartConfig, (uint8_t *)&uartConfig,
                            sizeof (CyU3PUartConfig_t));
                    /*
                     *	保存串口配置
                     */
                    grabconfParam.n_uart_baud = glUartConfig.baudRate;
                    grabconfParam.n_uart_stop_bit = glUartConfig.stopBit;
                    grabconfParam.n_uart_pority = glUartConfig.parity;

                }
            }
        }
        /* get_line_coding */
        else if (bRequest == GET_LINE_CODING )
        {
            /* get current uart config */
            config_data[0] = glUartConfig.baudRate&(0x000000FF);
            config_data[1] = ((glUartConfig.baudRate&(0x0000FF00))>> 8);
            config_data[2] = ((glUartConfig.baudRate&(0x00FF0000))>>16);
            config_data[3] = ((glUartConfig.baudRate&(0xFF000000))>>24);
            if (glUartConfig.stopBit == CY_U3P_UART_ONE_STOP_BIT)
            {
                config_data[4] = 1;
            }
            else /* CY_U3P_UART_TWO_STOP_BIT */
            {
                config_data[4] = 2;
            }

            if (glUartConfig.parity == CY_U3P_UART_EVEN_PARITY)
            {
                config_data[5] = 2;
            }
            else if (glUartConfig.parity == CY_U3P_UART_ODD_PARITY)
            {
                config_data[5] = 1;
            }
            else
            {
                config_data[5] = 0;
            }
            config_data[6] =  0x08;
            status = CyU3PUsbSendEP0Data( 0x07, config_data);
            if (status != CY_U3P_SUCCESS)
            {
                CyFxAppErrorHandler(status);
            }
        }
        /* SET_CONTROL_LINE_STATE */
        else if (bRequest == SET_CONTROL_LINE_STATE)
        {
            if (glIsCdcInActive)
            {
                CyU3PUsbAckSetup();
            }
            else
                CyU3PUsbStall (0, CyTrue, CyFalse);
        }
        else
        {
            status = CY_U3P_ERROR_FAILURE;
        }

        if (status != CY_U3P_SUCCESS)
        {
            isHandled = CyFalse;
        }
    }

#endif
    return isHandled;
}


/* This is the callback function to handle the USB events. */
void CyFxSlFifoApplnUSBEventCB(
    CyU3PUsbEventType_t evtype,
    uint16_t evdata)
{
    switch (evtype)
    {
    case CY_U3P_USB_EVENT_SETCONF:
        CyU3PUsbLPMDisable();
        /* Stop the application before re-starting. */
        if (glIsApplnActive)
        {
        	CyU3PDebugPrint(4,"CY_U3P_USB_EVENT_SETCONF happened");
            CyFxSlFifoApplnStop();
        }
		#ifdef cdc
		CdcChannelTryStop();
		if(globUartConfig == CyTrue)
			CyU3PDebugDeInit(); //避免CDC端点被debug占用，导致重启设备失败
		#endif
        /* Start the loop back function. */
        CyFxSlFifoApplnStart();
		#ifdef cdc
			CyFxUSBUARTAppStart();
			if(globUartConfig == CyTrue)
				DebugInitUsingCDC();
		#endif
        break;

    case CY_U3P_USB_EVENT_RESET:
    case CY_U3P_USB_EVENT_DISCONNECT:
        /* Stop the loop back function. */
        if(glIsApplnActive)
        {
        	CyU3PDebugPrint(4,"CY_U3P_USB_EVENT_DISCONNECT happened");
			CyFxSlFifoApplnStop();
        }
		#ifdef cdc
		CdcChannelTryStop();
		if(globUartConfig == CyTrue)
			CyU3PDebugDeInit();//避免CDC端点被debug占用，导致重启设备失败
		#endif

        break;

    default:
        break;
    }
}

/* Callback function to handle LPM requests from the USB 3.0 host. This function is invoked by the API
   whenever a state change from U0 -> U1 or U0 -> U2 happens. If we return CyTrue from this function, the
   FX3 device is retained in the low power state. If we return CyFalse, the FX3 device immediately tries
   to trigger an exit back to U0.

   This application does not have any state in which we should not allow U1/U2 transitions; and therefore
   the function always return CyTrue.
 */
CyBool_t
CyFxApplnLPMRqtCB(
    CyU3PUsbLinkPowerMode link_mode)
{
    return CyTrue;
}



/* This function initializes the GPIF interface and initializes
 * the USB interface. */
void CyFxSlFifoApplnInit(void)
{
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    apiRetStatus = fx3_gpio_init();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Start the USB functionality. */
    apiRetStatus = CyU3PUsbStart();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* The fast enumeration is the easiest way to setup a USB connection,
     * where all enumeration phase is handled by the library. Only the
     * class / vendor requests need to be handled by the application. */
    CyU3PUsbRegisterSetupCallback(CyFxSlFifoApplnUSBSetupCB, CyTrue);

    /* Setup the callback to handle the USB events. */
    CyU3PUsbRegisterEventCallback(CyFxSlFifoApplnUSBEventCB);

    /* Register a callback to handle LPM requests from the USB 3.0 host. */
    CyU3PUsbRegisterLPMRequestCallback(CyFxApplnLPMRqtCB);

    /* Set the USB Enumeration descriptors */

    /* Super speed device descriptor. */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB30DeviceDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB set device descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* High speed device descriptor. */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB20DeviceDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
//        CyU3PDebugPrint (4, "USB set device descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* BOS descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_BOS_DESCR, 0, (uint8_t *)CyFxUSBBOSDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB set configuration descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Device qualifier descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)CyFxUSBDeviceQualDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB set device qualifier descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Super speed configuration descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBSSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB set configuration descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* High speed configuration descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBHSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB Set Other Speed Descriptor failed, Error Code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Full speed configuration descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBFSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB Set Configuration Descriptor failed, Error Code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* String descriptor 0 */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)CyFxUSBStringLangIDDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB set string descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* String descriptor 1 */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)CyFxUSBManufactureDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB set string descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* String descriptor 2 */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)CyFxUSBProductDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB set string descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Connect the USB Pins with super speed operation enabled. */
    apiRetStatus = CyU3PConnectState(CyTrue, CyTrue);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        // CyU3PDebugPrint (4, "USB Connect failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    isControlRun = CyFalse;
}

#ifdef cdc
volatile uint16_t glPktsPending = 0;            /* Number of packets that have been committed since last check. */
void
USBUARTAppThread_Entry (
        uint32_t input)
{
    uint32_t regValueEn = 0, regValueDs = 0;

    /* UART Config Value for Enabling Rx Block */
    regValueEn = UART->lpp_uart_config;

    /* UART Config Value for Disabling the Rx Block  */
    regValueDs = UART->lpp_uart_config & (~(CY_U3P_LPP_UART_RTS | CY_U3P_LPP_UART_RX_ENABLE));

    for (;;)
    {
        if (glIsApplnActive)
        {
            /* While the application is active, check for data sent during the last 50 ms. If no data
               has been sent to the host, use the channel wrap-up feature to send any partial buffer to
               the USB host.
            */
			if (glPktsPending == 0)
            {
                /* Disable UART Receiver Block */
                UART->lpp_uart_config = regValueDs;

                CyU3PDmaChannelSetWrapUp (&glChHandleUarttoUsb);

                /* Enable UART Receiver Block */
                UART->lpp_uart_config = regValueEn;
            }

            glPktsPending = 0;
        }

        CyU3PThreadSleep (50);
    }
}
#endif

/* Entry function for the slFifoAppThread. */
void SlFifoAppThread_Entry(
    uint32_t input)
{
	uint8_t wrBuffer[1];

    // Initialize the send message buffer
    FifoInitial(&glSendFifo, glFifoBuffer, FIFO_DEEPTH_MAX_VALUE, sizeof(FIFO_DATA));
    /* Initialize the slave FIFO application */
    CyFxSlFifoApplnInit();

    //先拉住fpga校验密码
    CyU3PGpioSetValue(FPGA_N_CONFIG_PIN, CyTrue);
    CyU3PThreadSleep(100);
    CyU3PGpioSetValue(FPGA_N_CONFIG_PIN, CyFalse);
    CyU3PThreadSleep(1);
	CyU3PThreadSleep(800);
	wrBuffer[0] = 0xab;
	MCUSpiWriteRead(wrBuffer, 1,NULL,0,0);

	StorageReadSystemContent(&glbSystem);
	// read flash id for password
	StorageGetHard_ID(glbSystem.nDeviceUniqID);

	if(!StorageCheckPassword(glbSystem.nDeviceUniqID,glbSystem.nPassword))
	{
		glbSystem.nisDevicePassOK = CyFalse;
	}
	else
	{
		glbSystem.nisDevicePassOK = CyTrue;
	}

	if(glbSystem.nisDevicePassOK == CyTrue)
	{
	    // initialize the FPGA
		if(CyFalse == fpga_init())
		{
			CyFxAppErrorHandler(0);
		}
	}

    for (;;)
    {
			while(restartFlg || restartKey)
			{
				CyU3PThreadSleep(10);
			}
            CyU3PGpioSetValue(FX3_LED_PIN, CyTrue);
            CyU3PThreadSleep(500);
            if(qtConnectedState)
            	CyU3PGpioSetValue(FX3_LED_PIN, CyFalse);
            CyU3PThreadSleep(500);
            GrabGetSystemStatus();
            GrabTriggerFlcBitAndUpdate();
    }
}


void RestartDevice(void)
{
	restartFlg = CyTrue;
	// todo reset函数的作用得验一下，通道，线程，dma
	// 这个执行完之后 windows上的设备就没了
    CyU3PConnectState (CyFalse, CyTrue);
    CyU3PThreadSleep (3000);
	// 从 main 开始重新执行的，设备会消失，很彻底，上位机的扫描有bug,我重启的时候应该断开一下，不然是假连接，控制通道和数据通道不能用
	CyU3PDeviceReset(CyFalse);
}


void
RestartAppThread_Entry (
        uint32_t input)
{
	CyBool_t gpio_value = CyFalse;
	uint8_t key2num = 0;

    for(;;)
    {
    	while(!restartFlg)
    	{
            CyU3PGpioGetValue(FX3_RESET_KEY,&gpio_value);
            if(gpio_value == CyTrue)
            {
            	CyU3PDebugPrint(4,"\n reset key 1");
            	gpio_value = CyFalse;
            	CyU3PThreadSleep(2000);
            	CyU3PGpioGetValue(FX3_RESET_KEY,&gpio_value);
            	if(gpio_value == CyTrue)
            	{
            		CyU3PDebugPrint(4,"\n reset key 2");
            		while(1)
            		{
            			CyU3PGpioSetValue(FX3_LED_PIN, CyFalse);
            			CyU3PGpioGetValue(FX3_RESET_KEY,&gpio_value);
            			if(gpio_value == CyFalse)
            			{
            				restartKey = CyTrue;
            				key2num = 0;
            				break;
            			}
            			else
            			{
            				key2num++;
            				CyU3PThreadSleep(100);
            				if(key2num >= 50)
            				{
            					key2num = 0;
            					break;
            				}
            			}
            		}
            		if(restartKey)
            		{
            			restartKey = CyFalse;
            			// 把fpga的灯也关掉
            			GrabSetFpgaLedStatus(100000000,0);
            			RestartDevice();
            		}
            		else
            		{
            			CyU3PGpioSetValue(FX3_LED_PIN, CyTrue);
                		CyU3PDebugPrint(4,"\n reset key 2, continue");
                		CyU3PThreadSleep(500);
            		}

            	}
            	else
            	{
            		CyU3PDebugPrint(4,"\n reset key 1, continue");
            		CyU3PThreadSleep(1000);
            	}
            }
            else
            {
            	if(glbCheckDogEnable == CyTrue)
            		GrabFpgaClkStatusDog();
            	CyU3PThreadSleep(1000);
            }
    	}
    }
}


/* Application define function which creates the threads. */
void CyFxApplicationDefine(
    void)
{
    void *ptr = NULL;
    uint32_t retThrdCreate = CY_U3P_SUCCESS;

    /* Allocate the memory for the thread */
    ptr = CyU3PMemAlloc(CY_FX_SLFIFO_THREAD_STACK);

    /* Create the thread for the application */
    retThrdCreate = CyU3PThreadCreate(&slFifoAppThread,             /* Slave FIFO app thread structure */
                                      "21:Slave_FIFO_sync",         /* Thread ID and thread name */
                                      SlFifoAppThread_Entry,        /* Slave FIFO app thread entry function */
                                      0,                            /* No input parameter to thread */
                                      ptr,                          /* Pointer to the allocated thread stack */
                                      CY_FX_SLFIFO_THREAD_STACK,    /* App Thread stack size */
                                      CY_FX_SLFIFO_THREAD_PRIORITY, /* App Thread priority */
                                      CY_FX_SLFIFO_THREAD_PRIORITY, /* App Thread pre-emption threshold */
                                      CYU3P_NO_TIME_SLICE,          /* No time slice for the application thread */
                                      CYU3P_AUTO_START              /* Start the thread immediately */
    );

    /* Check the return code */
    if (retThrdCreate != 0)
    {
        /* Thread Creation failed with the error code retThrdCreate */

        /* Add custom recovery or debug actions here */

        /* Application cannot continue */
        /* Loop indefinitely */
        while (1);
    }


    void *ptr1 = NULL;
    ptr1 = CyU3PMemAlloc(CY_FX_SLFIFO_THREAD_STACK);
    retThrdCreate = CyU3PThreadCreate (&RestartAppThread,          /* USBUART Example App Thread structure */
                "22:Restart_App",                   /* Thread ID and Thread name */
                RestartAppThread_Entry,                  /* USBUART Example App Thread Entry function */
                0,                                       /* No input parameter to thread */
                ptr1,                                     /* Pointer to the allocated thread stack */
                CY_FX_SLFIFO_THREAD_STACK,               /* USBUART Example App Thread stack size */
                CY_FX_SLFIFO_THREAD_PRIORITY,            /* USBUART Example App Thread priority */
                CY_FX_SLFIFO_THREAD_PRIORITY,            /* USBUART Example App Thread priority */
                CYU3P_NO_TIME_SLICE,                     /* No time slice for the application thread */
                CYU3P_AUTO_START                         /* Start the Thread immediately */
                );
        /* Check the return code */
        if (retThrdCreate != 0)
        {
            /* Loop indefinitely */
            while(1);
        }
}

/*
 * Main function
 */
int main(void)
{
    if (fx3_device_init() != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* This is a non returnable call for initializing the RTOS kernel */
    CyU3PKernelEntry();

    /* Dummy return to make the compiler happy */
    return 0;

handle_fatal_error:

    /* Cannot recover from this error. */
    while (1)
    {
    }
}

/* [ ] */
