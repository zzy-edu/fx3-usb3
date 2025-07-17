#include "app_virtual_uart.h"
#include "cyu3types.h"
#include "cyu3error.h"
#include "cyu3system.h"
#include "fx3_common.h"

/* 是否打开串口调试 */
CyBool_t globUartConfig = CyFalse;
/* Whether the cdc application is active or not. */
CyBool_t glIsCdcInActive = CyFalse;

/* cdc 通道初始化 */
void
CyFxUSBUARTAppStart(
        void )
{
    uint16_t size = 0;
    CyU3PEpConfig_t epCfg;
    CyU3PDmaChannelConfig_t dmaCfg;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
    CyU3PUSBSpeed_t usbSpeed = CyU3PUsbGetSpeed();

    /* Based on the Bus speed configure the endpoint packet size */
    switch (usbSpeed)
    {
        case CY_U3P_FULL_SPEED:
            size = 64;
            break;

        case CY_U3P_HIGH_SPEED:
            size = 512;
            break;

        case  CY_U3P_SUPER_SPEED:
            /* Turning low power mode off to avoid USB transfer delays. */
            CyU3PUsbLPMDisable ();
            size = 1024;
            break;

        default:
            CyFxAppErrorHandler(CY_U3P_ERROR_FAILURE);
            break;
    }

    CyU3PMemSet ((uint8_t *)&epCfg, 0, sizeof (epCfg));
    epCfg.enable = CyTrue;
    epCfg.epType = CY_U3P_USB_EP_BULK;
    epCfg.burstLen = 1;
    epCfg.streams = 0;
    epCfg.pcktSize = size;

    /* Producer endpoint configuration */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_PRODUCER_CDC , &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler (apiRetStatus);
    }

    /* Consumer endpoint configuration */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_CONSUMER_CDC, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler (apiRetStatus);
    }

    /* Interrupt endpoint configuration */
    epCfg.epType = CY_U3P_USB_EP_INTR;
    epCfg.pcktSize = 64;
    epCfg.isoPkts = 1;

    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_INTERRUPT_CDC, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }


    /* Create a DMA_AUTO channel between usb producer socket and uart consumer socket */
    dmaCfg.size = size;
    dmaCfg.count = 8;//CY_FX_USBUART_DMA_BUF_COUNT;
    dmaCfg.prodSckId = CY_FX_EP_PRODUCER_CDC_SOCKET;    //CY_FX_EP_PRODUCER1_SOCKET;
    dmaCfg.consSckId = CY_FX_EP_CONSUMER_CDC_SOCKET;   //CY_FX_EP_CONSUMER1_SOCKET;
    dmaCfg.dmaMode = CY_U3P_DMA_MODE_BYTE;
    dmaCfg.notification = 0;
    dmaCfg.cb = NULL;
    dmaCfg.prodHeader = 0;
    dmaCfg.prodFooter = 0;
    dmaCfg.consHeader = 0;
    dmaCfg.prodAvailCount = 0;

    apiRetStatus = CyU3PDmaChannelCreate (&glChHandleUarttoUart,
    		CY_U3P_DMA_TYPE_AUTO, &dmaCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }


    /* Set DMA Channel transfer size */
    apiRetStatus = CyU3PDmaChannelSetXfer (&glChHandleUarttoUart,0);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Update the status flag. */
    glIsCdcInActive = CyTrue;
}

// cdc 通道关闭
void
CyFxUSBUARTAppStop (
        void)
{
    CyU3PEpConfig_t epCfg;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    /* Update the flag. */
    glIsCdcInActive = CyFalse;

    /* Flush the endpoint memory */
    CyU3PUsbFlushEp(CY_FX_EP_PRODUCER_CDC);
    CyU3PUsbFlushEp(CY_FX_EP_CONSUMER_CDC);
    CyU3PUsbFlushEp(CY_FX_EP_INTERRUPT_CDC);

    /* Destroy the channel */
    CyU3PDmaChannelDestroy (&glChHandleUarttoUart);

    /* Disable endpoints. */
    CyU3PMemSet((uint8_t *)&epCfg, 0, sizeof(epCfg));
    epCfg.enable = CyFalse;

    /* Producer endpoint configuration. */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_PRODUCER_CDC, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler (apiRetStatus);
    }

    /* Consumer endpoint configuration. */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_CONSUMER_CDC, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler (apiRetStatus);
    }

    /* Interrupt endpoint configuration. */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_INTERRUPT_CDC, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler (apiRetStatus);
    }
}

void CdcChannelTryStop(void)
{
	if(glIsCdcInActive)
	{
		CyFxUSBUARTAppStop();
	}
}

// 关闭cdc通道，将cdc的ep consumer作为打印串口
void DebugInitUsingCDC(void)
{
	CyU3PDmaChannelDestroy (&glChHandleUarttoUart);
	CyU3PDebugInit(CY_FX_EP_CONSUMER_CDC_SOCKET, 8);
	CyU3PDebugPreamble(CyFalse);
}

// 关闭调试，打开cdc通道
void DebugDeInitStartCDC(void)
{
	CyU3PDebugDeInit();
	CdcChannelTryStop();
	CyFxUSBUARTAppStart();
}
