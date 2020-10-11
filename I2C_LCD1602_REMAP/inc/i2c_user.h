#ifndef I2C_USER_H_
#define I2C_USER_H_
//------------------------------------------------
#include "osapi.h"
//------------------------------------------------
#define I2C_MASTER_SDA_MUX PERIPHS_IO_MUX_GPIO5_U
#define I2C_MASTER_SCL_MUX PERIPHS_IO_MUX_GPIO4_U
#define I2C_MASTER_SDA_FUNC FUNC_GPIO5
#define I2C_MASTER_SCL_FUNC FUNC_GPIO4
#define I2C_MASTER_SDA_GPIO 5
#define I2C_MASTER_SCL_GPIO 4
//------------------------------------------------
#define I2C_MASTER_SDA_HIGH_SCL_HIGH()  \
    gpio_output_set(1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)

#define I2C_MASTER_SDA_HIGH_SCL_LOW()  \
    gpio_output_set(1<<I2C_MASTER_SDA_GPIO, 1<<I2C_MASTER_SCL_GPIO, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)

#define I2C_MASTER_SDA_LOW_SCL_HIGH()  \
    gpio_output_set(1<<I2C_MASTER_SCL_GPIO, 1<<I2C_MASTER_SDA_GPIO, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)

#define I2C_MASTER_SDA_LOW_SCL_LOW()  \
    gpio_output_set(0, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 1<<I2C_MASTER_SDA_GPIO | 1<<I2C_MASTER_SCL_GPIO, 0)
//------------------------------------------------
void ICACHE_FLASH_ATTR i2c_mas_gpio_init(void);
void I2C_SendByteByADDR(uint8_t c,uint8_t addr);
//------------------------------------------------
#define i2c_mas_wait    os_delay_us
//------------------------------------------------
#endif /* I2C_USER_H_ */
