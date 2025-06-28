#ifndef _FX3_PIN_DEFINE_H_
#define _FX3_PIN_DEFINE_H_

#define FPGA_SPI_CS         23 /* bk: CTL  3  20, nw: CTL 6  23 */
#define FPGA_SPI_CLK        27 /* bk: CTL  4  21, nw: CTL 10 27 */
#define FPGA_RESET_PIN      22 // CTL  5
#define FPGA_SPI_SOMI       25 /* bk: CTL  6  23, nw: CTL  8 25 */
#define FPGA_SPI_SIMO       26 /* bk: CTL  7  24, nw: CTL  9 26 */
//#define FPGA_GND            25 // CTL  8
//#define FPGA_NCE_PIN        26 // CTL  9
#define FPGA_N_CONFIG_PIN   45 // CTL 10  27
#define FX3_LED_PIN         60


#define MCU_SPI_SS2  51 //D2
#define MCU_SPI_SCK2 57	//C4
#define MCU_SPI_SDO2 52	//D3
#define MCU_SPI_SDI2 50	//D1
#endif
