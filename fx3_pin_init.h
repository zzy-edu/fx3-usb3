#ifndef FX3_PIN_INIT_H
#define FX3_PIN_INIT_H
#include <cyu3types.h>
/*fx3 io��صļĴ���  */
// gpio ���������üĴ���
#define FX3_GCTL_WPD_CFG (*(volatile uint32_t *)0xE0051028)


CyU3PReturnStatus_t fx3_device_init(void);
CyU3PReturnStatus_t fx3_gpio_init(void);
#endif
