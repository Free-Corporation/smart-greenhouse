#include "main.h"
//------------------------------------------------------
void ICACHE_FLASH_ATTR task1(void *pvParameters)
{
	while(1)
	{
		os_printf("SDK version: %s\n", system_get_sdk_version());
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}
//------------------------------------------------------
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
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
	UART_SetBaudrate(UART0, BIT_RATE_115200);
	UART_SetBaudrate(UART1, BIT_RATE_115200);
	os_printf("\r\n\r\n");
	os_printf("SDK version:%s\n", system_get_sdk_version());
	os_printf("\r\n\r\n");
	PIN_FUNC_SELECT (PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	init_esp_wifi();
	start_wifi_ap(WIFI_APSSID, WIFI_APPASSWORD);
	xTaskCreate(task1, "task1", 256, NULL, 1, NULL);
}
//------------------------------------------------------
