#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
//------------------------------------------------------
#define LED 2
//------------------------------------------------------
static os_timer_t os_timer01;
static uint8_t led_state=0;
static uint8_t wifiStatus = STATION_IDLE;
static uint8_t connectStatus = 0;
//------------------------------------------------------
static void ICACHE_FLASH_ATTR wifi_check_ip(void *arg)
{
	struct ip_info ipConfig;
	os_timer_disarm(&os_timer01);
	wifiStatus = wifi_station_get_connect_status();
	if (wifiStatus == STATION_GOT_IP && ipConfig.ip.addr != 0)
	{
		os_timer_setfn(&os_timer01, (os_timer_func_t *)wifi_check_ip, NULL);
		os_timer_arm(&os_timer01, 2000, 0);
		if(connectStatus == 0)
		{
		os_printf("STATION WIFI CONNECTED\r\n");
		connectStatus = 1;
		}
		else
		{
			led_state = (led_state==0) ? 1 : 0;
			GPIO_OUTPUT_SET(LED, led_state);
		}
	}
	else
	{
		connectStatus = 0;
		if(wifi_station_get_connect_status() == STATION_WRONG_PASSWORD)
		{
			os_printf("STATION_WRONG_PASSWORD\r\n");
			wifi_station_connect();
		}
		else if(wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
		{
			os_printf("STATION_NO_AP_FOUND\r\n");
			wifi_station_connect();
		}
		else if(wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
		{
				os_printf("STATION_NO_AP_FOUND\r\n");
				wifi_station_connect();
		}
		else
		{
				os_printf("STATION_IDLE\r\n");
		}
		os_timer_setfn(&os_timer01, (os_timer_func_t *)wifi_check_ip, NULL);
		os_timer_arm(&os_timer01, 500, 0);
	}
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
	struct station_config stationConf;
	// Configure UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	gpio_output_set(0, 0, (1 << LED), 0);
	ets_delay_us(100000);
	os_printf("\r\n");
	wifi_set_opmode_current(STATION_MODE);
	wifi_set_sleep_type(NONE_SLEEP_T);
	os_memset(&stationConf, 0, sizeof(struct station_config));
	os_sprintf(stationConf.ssid, "%s", WIFI_CLIENTSSID);
	os_sprintf(stationConf.password, "%s", WIFI_CLIENTPASSWORD);
	wifi_station_set_config_current(&stationConf);
	os_timer_disarm(&os_timer01);
	os_timer_setfn(&os_timer01, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&os_timer01, 1000, 0);
	wifi_station_connect();
}
//------------------------------------------------------
