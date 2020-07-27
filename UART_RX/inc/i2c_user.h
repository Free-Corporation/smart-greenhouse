#ifndef I2C_USER_H_
#define I2C_USER_H_
//------------------------------------------------------
#include "osapi.h"
#include "driver/i2c_master.h"
//------------------------------------------------------
void I2C_SendByteByADDR(uint8_t c,uint8_t addr);
//------------------------------------------------------
#endif /* I2C_USER_H_ */
