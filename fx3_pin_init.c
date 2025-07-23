#include "fx3_pin_init.h"
#include "fx3_pin_define.h"
#include "cyfxgpif2config.h"
#include "cyfxslfifosync.h"
#include <cyu3gpif.h>
#include <cyu3system.h>
#include <cyu3pib.h>
#include <cyu3gpio.h>
#include <cyu3error.h>
#include <cyu3uart.h>
CyU3PReturnStatus_t fx3_device_init(void)
{
    CyU3PIoMatrixConfig_t io_cfg;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
    CyU3PSysClockConfig_t clkCfg;
    /* setSysClk400 clock configurations */
    clkCfg.setSysClk400 = CyTrue;   /* FX3 device's master clock is set to a frequency > 400 MHz */
    clkCfg.cpuClkDiv = 2;           /* CPU clock divider */
    clkCfg.dmaClkDiv = 2;           /* DMA clock divider */
    clkCfg.mmioClkDiv = 2;          /* MMIO clock divider */
    clkCfg.useStandbyClk = CyFalse; /* device has no 32KHz clock supplied */
    clkCfg.clkSrc = CY_U3P_SYS_CLK; /* Clock source for a peripheral block  */

    /* Initialize the device */
    status = CyU3PDeviceInit(&clkCfg);
    if (status != CY_U3P_SUCCESS)
    {
        return CyFalse;
    }

    /* Initialize the caches. Enable instruction cache and keep data cache disabled.
     * The data cache is useful only when there is a large amount of CPU based memory
     * accesses. When used in simple cases, it can decrease performance due to large
     * number of cache flushes and cleans and also it adds to the complexity of the
     * code. */
    status = CyU3PDeviceCacheControl(CyTrue, CyTrue, CyTrue);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Configure the IO matrix for the device. */
    io_cfg.useUart = CyTrue;
    io_cfg.useI2C = CyFalse;
    io_cfg.useI2S = CyFalse;
    io_cfg.useSpi = CyFalse;
    io_cfg.isDQ32Bit = CyTrue;
    io_cfg.lppMode = CY_U3P_IO_MATRIX_LPP_DEFAULT;
    /* GPIO 20-27 33-40 60 are enabled. */
    io_cfg.gpioSimpleEn[0]  = 0x0E800000; /* GPIO 23 25-27 */
    io_cfg.gpioSimpleEn[1]  = 0x121C2000; /* GPIO  45 50-52 57 60*/
    io_cfg.gpioComplexEn[0] = 0x00000000;
    io_cfg.gpioComplexEn[1] = 0x00000000;
    status = CyU3PDeviceConfigureIOMatrix(&io_cfg);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }
    /* GPIO 20 */
//    status = CyU3PDeviceGpioOverride(FPGA_SPI_CS, CyTrue);
//    if (status != CY_U3P_SUCCESS)
//    {
//    	goto handle_fatal_error;
//    }
    /* GPIO 22 */
    status = CyU3PDeviceGpioOverride(FPGA_RESET_PIN, CyTrue);
    if (status != CY_U3P_SUCCESS)
    {
    	goto handle_fatal_error;
    }

    status = CyU3PDeviceGpioOverride(FX3_RESET_KEY, CyTrue);
    if (status != CY_U3P_SUCCESS)
    {
    	goto handle_fatal_error;
    }

    // ≈‰÷√ FX3_RESET_KEYŒ™»ıœ¬¿≠
    FX3_GCTL_WPD_CFG |= 0x40000000;

    return CY_U3P_SUCCESS;

handle_fatal_error:
    while (1);
}

CyU3PReturnStatus_t fx3_gpio_init(void)
{
    CyU3PPibClock_t pibClock;
    CyU3PGpioClock_t gpioClock;
    CyU3PGpioSimpleConfig_t gpioConfig;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    /* Initialize the p-port block. */
    pibClock.clkDiv = 2;
    pibClock.clkSrc = CY_U3P_SYS_CLK;
    pibClock.isHalfDiv = CyFalse;
    /* Disable DLL for sync GPIF */
    pibClock.isDllEnable = CyFalse;
    apiRetStatus = CyU3PPibInit(CyTrue, &pibClock);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    /* Load the GPIF configuration for Slave FIFO sync mode. */
    apiRetStatus = CyU3PGpifLoad(&CyFxGpifConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    CyU3PGpifSocketConfigure (0,CY_FX_PRODUCER_PPORT_0_SOCKET,20,CyFalse,1);

    /* Start the state machine. */
    apiRetStatus = CyU3PGpifSMStart(RESET, ALPHA_RESET);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    /* Init the GPIO module */
    gpioClock.fastClkDiv = 2;
    gpioClock.slowClkDiv = 0;
    gpioClock.simpleDiv = CY_U3P_GPIO_SIMPLE_DIV_BY_2;
    gpioClock.clkSrc = CY_U3P_SYS_CLK;
    gpioClock.halfDiv = 0;

    apiRetStatus = CyU3PGpioInit(&gpioClock, NULL);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    /* GPIO config */
    gpioConfig.inputEn = CyTrue;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyTrue;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FX3_LED_PIN, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }


    /* FX3_RESET_KEY GPIO config */
    gpioConfig.inputEn = CyTrue;
    gpioConfig.driveLowEn = CyFalse;
    gpioConfig.driveHighEn = CyFalse;
    gpioConfig.outValue = CyFalse;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FX3_RESET_KEY, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

//    gpioConfig.inputEn = CyFalse;
//    gpioConfig.driveLowEn = CyTrue;
//    gpioConfig.driveHighEn = CyFalse;
//    gpioConfig.outValue = CyFalse;
//    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
//    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_GND, &gpioConfig);
//    if (apiRetStatus != CY_U3P_SUCCESS)
//    {
//        return apiRetStatus;
//    }

//    gpioConfig.inputEn = CyFalse;
//    gpioConfig.driveLowEn = CyTrue;
//    gpioConfig.driveHighEn = CyTrue;
//    gpioConfig.outValue = CyTrue;
//    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
//    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_NCE_PIN, &gpioConfig);
//    if (apiRetStatus != CY_U3P_SUCCESS)
//    {
//        return apiRetStatus;
//    }

    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyTrue;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_N_CONFIG_PIN, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyFalse;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_RESET_PIN, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    /* Init the Spi IO */
    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyTrue;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_SPI_CS, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyTrue;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_SPI_CLK, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyFalse;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_SPI_SIMO, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    gpioConfig.inputEn = CyTrue;
    gpioConfig.driveLowEn = CyFalse;
    gpioConfig.driveHighEn = CyFalse;
    gpioConfig.outValue = CyFalse;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(FPGA_SPI_SOMI, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }

    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyTrue;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(MCU_SPI_SS2, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }
    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyFalse;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(MCU_SPI_SCK2, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }
    gpioConfig.inputEn = CyFalse;
    gpioConfig.driveLowEn = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.outValue = CyFalse;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(MCU_SPI_SDO2, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }
    gpioConfig.inputEn = CyTrue;
    gpioConfig.driveLowEn = CyFalse;
    gpioConfig.driveHighEn = CyFalse;
    gpioConfig.outValue = CyFalse;
    gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
    apiRetStatus = CyU3PGpioSetSimpleConfig(MCU_SPI_SDI2, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        return apiRetStatus;
    }
    return CY_U3P_SUCCESS;
}
