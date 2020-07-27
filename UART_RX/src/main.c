#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/i2c_master.h"
#include "driver/uart.h"
#include "lcd.h"
//------------------------------------------------------
void ets_isr_mask(unsigned intr);
void ets_isr_unmask(unsigned intr);
//------------------------------------------------------
char rx_buf[100] = {};
uint8 fifo_len;
uint8_t fl;
//------------------------------------------------------
static void uart0_rx_intr_handler(void *para)
{
  if (UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_FRM_ERR_INT_ST)) {
      WRITE_PERI_REG(UART_INT_CLR(UART0), UART_FRM_ERR_INT_CLR);
  }
  if (UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST)) {
      uart_rx_intr_disable(UART0);
  } else if (UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_TOUT_INT_ST)) {
      uart_rx_intr_disable(UART0);
  } else if (UART_TXFIFO_EMPTY_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_TXFIFO_EMPTY_INT_ST)) {
      CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_TXFIFO_EMPTY_INT_ENA);
      WRITE_PERI_REG(UART_INT_CLR(UART0), UART_TXFIFO_EMPTY_INT_CLR);
  } else if (UART_RXFIFO_OVF_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_OVF_INT_ST)) {
      WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_OVF_INT_CLR);
  }
  fifo_len = (READ_PERI_REG(UART_STATUS(UART0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
  if(fifo_len>0) fl=1;
}
//------------------------------------------------------
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 8;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}
//------------------------------------------------------
void ICACHE_FLASH_ATTR user_init()
{
	uint16_t i=0;
	uint8 d_tmp = 0;
	uint8 idx=0;
	char str01[6];
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	ets_delay_us(100000);
	os_printf("\r\n");
	gpio_init();
	i2c_master_gpio_init();
	I2C_MASTER_SDA_LOW_SCL_LOW();
	ETS_UART_INTR_DISABLE();
	ETS_UART_INTR_ATTACH(uart0_rx_intr_handler, rx_buf);
	PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
	ETS_UART_INTR_ENABLE();
	LCD_ini();
	ets_delay_us(100000);
	while (1)
	{
		while(!fl)
		{
			i++;
			if(i>500)
			{
				i=0;
				system_soft_wdt_feed();
			}
			ets_delay_us(1000);
		}
		fl=0;
		os_printf("fifo_len: %d\r\n",fifo_len);
		LCD_SetPos(3,1);
		LCD_String("					");
		LCD_SetPos(3,1);
		for(idx=0;idx<fifo_len;idx++)
		{
			d_tmp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
			if((d_tmp != 0x0D) && (d_tmp != 0x0A)) // if not CarriageReturn & NewLine
				LCD_Char(d_tmp);
		}
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);
		uart_rx_intr_enable(UART0);
	}
}
//------------------------------------------------------
