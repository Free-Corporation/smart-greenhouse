#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
//------------------------------------------------------
#define LED 2
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
	// Configuring the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	gpio_output_set(0, 0, (1 << LED), 0);
	while(1)
	{
		i++;
		if(i>9999) i=1;
		ets_delay_us(1000000);
		os_printf("String %04d\r\n", i);
	    os_printf("String %04d\r\n", i);
	    os_printf("SDK version: %s\n", system_get_sdk_version());
	    os_printf("Version info of boot: %d\n", system_get_boot_version());
	    os_printf("Userbin address: 0x%x\n", system_get_userbin_addr());
	    os_printf("Time = %ld\r\n", system_get_time());
	    os_printf("RTC time = %ld\r\n", system_get_rtc_time());
	    os_printf("Chip id = 0x%x\r\n", system_get_chip_id());
	    os_printf("CPU freq = %d MHz\r\n", system_get_cpu_freq());
	    os_printf("Flash size map = %d\r\n", system_get_flash_size_map());
	    os_printf("Free heap size = %d\r\n", system_get_free_heap_size());
	    system_print_meminfo();
		system_soft_wdt_feed();
		gpio_output_set(0, (1 << LED), 0, 0);
		ets_delay_us(1000000);
		system_soft_wdt_feed();
		gpio_output_set((1 << LED), 0, 0, 0);
	}
}
//------------------------------------------------------
