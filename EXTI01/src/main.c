#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
//------------------------------------------------------
#define LED 2
#define BUTTON 4
//------------------------------------------------------
uint8_t fl;
//------------------------------------------------------
void ets_isr_mask(unsigned intr);
void ets_isr_unmask(unsigned intr);
//------------------------------------------------------
static void gpio_intr_handler()
{
	uint32_t gpio_st = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	ETS_GPIO_INTR_DISABLE();
	if(gpio_st & BIT(4))
	{
		if(!fl) fl=1;
		else fl=0;
		os_delay_us(100000);
	}
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_st);
	ETS_GPIO_INTR_ENABLE();
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
	gpio_init();
	fl = 0;
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
	gpio_output_set(0, 0, (1 << LED), 0);
	gpio_output_set(0, 0, 0, (1 << BUTTON));
	ETS_GPIO_INTR_DISABLE();
	ETS_GPIO_INTR_ATTACH(gpio_intr_handler,NULL);
	gpio_pin_intr_state_set(GPIO_ID_PIN(4),GPIO_PIN_INTR_POSEDGE);
	ETS_GPIO_INTR_ENABLE();
	while(1)
	{
		ets_delay_us(100000);
		system_soft_wdt_feed();
		if(fl)
		{
			gpio_output_set(0, (1 << LED), 0, 0);
		}
		ets_delay_us(100000);
		system_soft_wdt_feed();
		gpio_output_set((1 << LED), 0, 0, 0);
	}
}
//------------------------------------------------------
