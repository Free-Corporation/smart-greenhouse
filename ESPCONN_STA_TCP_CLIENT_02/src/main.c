#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "espconn.h"
//------------------------------------------------------
#define LED 2
//------------------------------------------------------
static os_timer_t os_timer01;
static uint8_t led_state=0;
//------------------------------------------------------
typedef enum {
	WIFI_CONNECTING,
	WIFI_CONNECTING_ERROR,
	WIFI_CONNECTED
} tConnState;
//------------------------------------------------------
static tConnState connState = WIFI_CONNECTING;
//------------------------------------------------------
struct espconn pConn;
esp_udp ConnUDP;
//------------------------------------------------------
static void ICACHE_FLASH_ATTR wifi_check_ip(void *arg);
//------------------------------------------------------
void ICACHE_FLASH_ATTR tcpclient_discon_cb(void* arg)
{
	struct espconn *pConn = (struct espconn *)arg;
	os_free(pConn->proto.tcp);
	os_free(pConn);
	os_printf("TCP disconnected\r\n");
}
//------------------------------------------------------
void ICACHE_FLASH_ATTR tcpclient_sent_cb(void* arg)
{
	struct espconn *pConn = (struct espconn *)arg;
	os_printf("TCP packet sent\r\n");
}
//------------------------------------------------------
void ICACHE_FLASH_ATTR tcpclient_recv_cb(void* arg, char* pusrdata, uint16_t length)
{
	uint16_t idx;
	struct espconn *pConn = (struct espconn *)arg;
	for (idx=0;idx<length;idx++)
	{
		uart_tx_one_char(UART0,pusrdata[idx]);
	}
	espconn_disconnect(pConn);
}
//------------------------------------------------------
static void ICACHE_FLASH_ATTR tcpclient_connect_cb(void *arg)
{
	char payload[30]; // our string
	struct espconn *pConn = (struct espconn *)arg;
	espconn_regist_sentcb(pConn, tcpclient_sent_cb); //sending TCP-packet
	espconn_regist_recvcb(pConn, tcpclient_recv_cb); //sending TCP-packet
	espconn_regist_disconcb(pConn, tcpclient_discon_cb); //disconnecting the server
	os_sprintf(payload, "Hello from ESP8266!!!\r\n");
	espconn_sent(pConn, (uint8_t*) payload, strlen(payload));
}
//------------------------------------------------------
static void ICACHE_FLASH_ATTR tcp_connect()
{
	uint32_t ip;
	char tcpserverip[15] = {};
	struct espconn *pCon = (struct espconn *)os_zalloc(sizeof(struct espconn));
	if (pCon == NULL)
	{
		os_printf("TCP connect failed\r\n");
		return;
	}
	pCon->type = ESPCONN_TCP;
	pCon->state = ESPCONN_NONE;
	os_sprintf(tcpserverip, "%s", TCPSERVERIP);
	ip = ipaddr_addr(tcpserverip);
	pCon->proto.tcp = (esp_tcp *)os_zalloc(sizeof(esp_tcp));
	pCon->proto.tcp->local_port = espconn_port(); // port of client
	pCon->proto.tcp->remote_port = TCPSERVERPORT; // port of server
	os_memcpy(pCon->proto.tcp->remote_ip, &ip,4);
	espconn_regist_connectcb(pCon, tcpclient_connect_cb);
	os_printf("Start TCP connect...\r\n");
	espconn_connect(pCon);
}
//------------------------------------------------------
static void ICACHE_FLASH_ATTR wifi_check_ip(void *arg)
{
	struct ip_info ipConfig;
	os_timer_disarm(&os_timer01);
	uint8_t wifiStatus = wifi_station_get_connect_status();
	if (wifiStatus == STATION_GOT_IP)
	{
		wifi_get_ip_info(STATION_IF, &ipConfig);
		if (ipConfig.ip.addr != 0)
		{
			if(connState != WIFI_CONNECTED)
			{
				connState = WIFI_CONNECTED;
				tcp_connect();
				return;
			}
			else
			{
				led_state = (led_state==0) ? 1 : 0;
				GPIO_OUTPUT_SET(LED, led_state);
			}
		}
	}
	else
	{
		connState = WIFI_CONNECTING;
		espconn_delete(&pConn); //free memory for structure espconn
		if(wifi_station_get_connect_status() == STATION_WRONG_PASSWORD)
		{
			connState = WIFI_CONNECTING_ERROR;
			os_printf("STATION_WRONG_PASSWORD\r\n");
		}
		else if(wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
		{
			connState = WIFI_CONNECTING_ERROR;
			os_printf("STATION_NO_AP_FOUND\r\n");
		}
		else if(wifi_station_get_connect_status() == STATION_CONNECT_FAIL)
		{
			connState = WIFI_CONNECTING_ERROR;
			os_printf("STATION_CONNECT_FAIL\r\n");
		}
		else
		{
			connState = WIFI_CONNECTING;
			os_printf("WiFi connecting\r\n");
		}
	}
	os_timer_setfn(&os_timer01, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&os_timer01, 500, 0);
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
	// Disconnecting then autoconnect
	wifi_station_disconnect();
	// Stop DHCP client
	wifi_station_dhcpc_stop();
	if(wifi_station_get_config(&stationConf))
	{
		os_memset(stationConf.ssid, 0, sizeof(stationConf.ssid));
		os_memset(stationConf.password, 0, sizeof(stationConf.password));
		os_sprintf(stationConf.ssid, "%s", WIFI_CLIENTSSID);
		os_sprintf(stationConf.password, "%s", WIFI_CLIENTPASSWORD);
		if(!wifi_station_set_config(&stationConf))
		{
			os_printf("Not set station config!\r\n");
		}
	}
	wifi_set_sleep_type(NONE_SLEEP_T);
	wifi_station_connect();
	wifi_station_dhcpc_start();
	if(wifi_get_phy_mode() != PHY_MODE_11N)
		wifi_set_phy_mode(PHY_MODE_11N);
	if(wifi_station_get_auto_connect() == 0)
		wifi_station_set_auto_connect(1);
	os_timer_disarm(&os_timer01);
	os_timer_setfn(&os_timer01, (os_timer_func_t *)wifi_check_ip, NULL);
	os_timer_arm(&os_timer01, 1000, 0);
}
//------------------------------------------------------
