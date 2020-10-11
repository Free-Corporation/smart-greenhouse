#include "main.h"
//------------------------------------------------------
typedef struct
{
	unsigned char num_task;
	unsigned short del;
} pData;
pData dt1, dt2;
//------------------------------------------------------
void ICACHE_FLASH_ATTR task1(void *pvParameters)
{
	uint32 cnt=0;
	char str01[20];
	pData *pdt = (pData*) pvParameters;
	while(1)
	{
		snprintf(str01, sizeof(str01), "Task%d: %7lu", pdt->num_task, cnt);
		os_printf("%s\n",str01);
		cnt++;
		if(cnt>=10000000) cnt=0;
		vTaskDelay(pdt->del / portTICK_RATE_MS);
	}
}
//------------------------------------------------------
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
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
	UART_SetBaudrate(UART0, BIT_RATE_115200);
	UART_SetBaudrate(UART1, BIT_RATE_115200);
	os_printf("\r\n\r\n");
	os_printf("SDK version:%s\n", system_get_sdk_version());
	os_printf("\r\n\r\n");
	dt1.del = 1000; dt1.num_task = 1;
	dt2.del = 900; dt2.num_task = 2;
	xTaskCreate(task1, "task1", 256, (void *) &dt1, 1, NULL);
	xTaskCreate(task1, "task2", 256, (void *) &dt2, 1, NULL);
}
//------------------------------------------------------
