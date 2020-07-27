#include "i2c_user.h"
//------------------------------------------------------
void I2C_SendByteByADDR(uint8_t c,uint8_t addr)
{
	uint16_t i;
	uint8_t ack;
	i2c_master_start();
// Transmit Address SLAVE
	i2c_master_writeByte(addr);
	ack = i2c_master_checkAck();
	if(!ack)
	{
		os_printf("ADDR not ack\r\n");
	}
// Send data
	i2c_master_writeByte(c);
	i2c_master_send_ack();
	i2c_master_stop();
}
