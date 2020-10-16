#include "wifi.h"
//------------------------------------------------------
wifi_state_cb_t on_station_connect  = NULL;
wifi_disco_cb_t on_station_disconnect = NULL;
wifi_state_cb_t on_station_first_connect  = NULL;
wifi_state_cb_t on_client_connect  = NULL;
wifi_state_cb_t on_client_disconnect  = NULL;
//------------------------------------------------------
volatile bool wifi_station_is_connected = false;
volatile bool wifi_station_static_ip = false;
//------------------------------------------------------
static os_timer_t timer;
//------------------------------------------------------
#define GPIO_LED 2
//------------------------------------------------------
void ICACHE_FLASH_ATTR wifi_event_handler_cb(System_Event_t *event)
{
	static bool station_was_connected = false;
	if (event == NULL)
	{
		return;
	}
	os_printf("[WiFi] event %u\n", event->event_id);
	switch (event->event_id)
	{
		case EVENT_STAMODE_DISCONNECTED:
			wifi_station_is_connected = false;
			GPIO_OUTPUT_SET(GPIO_LED, 1);
			Event_StaMode_Disconnected_t *ev = (Event_StaMode_Disconnected_t *)&event->event_info;
			if(on_station_disconnect)
			{
				on_station_disconnect(ev->reason);
			}
			break;
		case EVENT_STAMODE_CONNECTED:
			if(wifi_station_static_ip)
			{
				os_printf("STA IP is static\n");
			}
			wifi_station_is_connected = true;
			if(!station_was_connected)
			{
				station_was_connected = true;
				if(on_station_first_connect)
				{
					on_station_first_connect();
				}
			}
			if(on_station_connect){
				on_station_connect();
			}
			break;
		case EVENT_STAMODE_DHCP_TIMEOUT:
			if(wifi_station_is_connected)
			{
				wifi_station_is_connected = false;
				if(on_station_disconnect)
				{
					on_station_disconnect(REASON_UNSPECIFIED);
					if(on_station_first_connect)
					{
						on_station_first_connect();
					}
				}
				if(on_station_connect)
				{
					on_station_connect();
				}
			}
			break;
		case EVENT_STAMODE_GOT_IP:
			wifi_station_is_connected = true;
			GPIO_OUTPUT_SET(GPIO_LED, 0);
			if(!station_was_connected)
			{
				station_was_connected = true;
			}
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			if(on_station_connect)
			{
				on_station_connect();
			}
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			if(on_client_disconnect)
			{
				on_client_disconnect();
			}
			break;
		default:
			break;
	}
}
//------------------------------------------------------
bool ICACHE_FLASH_ATTR wifi_station_connected()
{
	if(!wifi_station_is_connected)
	{
		return false;
	}
	WIFI_MODE mode = wifi_get_opmode();
	if((mode & STATION_MODE) == 0)
	{
		return false;
	}
	STATION_STATUS wifistate = wifi_station_get_connect_status();
	wifi_station_is_connected = (wifistate == STATION_GOT_IP ||
			(wifi_station_static_ip && wifistate == STATION_CONNECTING));
	return wifi_station_is_connected;
}
//------------------------------------------------------
LOCAL void ICACHE_FLASH_ATTR wait_for_connection_ready(uint8 flag)
{
	os_timer_disarm(&timer);
	if(wifi_station_connected())
	{
		os_printf("connected\n");
	}
	else
	{
		os_printf("reconnect after 2s\n");
		os_timer_setfn(&timer, (os_timer_func_t *)wait_for_connection_ready, NULL);
		os_timer_arm(&timer, 2000, 0);
	}
}
//------------------------------------------------------
void ICACHE_FLASH_ATTR set_on_station_connect(wifi_state_cb_t cb)
{
	on_station_connect = cb;
}
//------------------------------------------------------
void ICACHE_FLASH_ATTR set_on_station_disconnect(wifi_disco_cb_t cb)
{
	on_station_disconnect = cb;
}
//------------------------------------------------------
LOCAL void ICACHE_FLASH_ATTR on_wifi_connect()
{
	os_printf("on_wifi_connect();\n");
	os_timer_disarm(&timer);
	os_timer_setfn(&timer, (os_timer_func_t *)wait_for_connection_ready, NULL);
	os_timer_arm(&timer, 100, 0);
}
//------------------------------------------------------
LOCAL void ICACHE_FLASH_ATTR on_wifi_disconnect(uint8_t reason)
{
	os_printf("disconnect %d\n", reason);
}
//------------------------------------------------------
bool ICACHE_FLASH_ATTR wifi_set_mode(WIFI_MODE mode)
{
	if(!mode){
		bool s = wifi_set_opmode(mode);
		wifi_fpm_open();
		wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
		wifi_fpm_do_sleep(0xFFFFFFFF);
		return s;
	}
	wifi_fpm_close();
	return wifi_set_opmode(mode);
}
//------------------------------------------------------
bool ICACHE_FLASH_ATTR start_wifi_station(const char * ssid, const char * pass)
{
	WIFI_MODE mode = wifi_get_opmode();
	if ((mode & STATION_MODE) == 0)
	{
		mode |= STATION_MODE;
		if(!wifi_set_mode(mode))
		{
			os_printf("Failed to enable Station mode!\n");
			return false;
		}
	}
	if(!ssid)
	{
		os_printf("No SSID Given. Will connect to the station in flash\n");
		return true;
	}
	struct station_config config;
	memset(&config, 0, sizeof(struct station_config));
	strcpy(config.ssid, ssid);
	if(pass)
	{
		strcpy(config.password, pass);
	}
	if(!wifi_station_set_config(&config))
	{
		os_printf("Failed to set Station config!\n");
		return false;
	}
	wifi_set_sleep_type(NONE_SLEEP_T);
	if(!wifi_station_dhcpc_status())
	{
		os_printf("DHCP is not started. Starting it...\n");
		if(!wifi_station_dhcpc_start())
		{
			os_printf("DHCP start failed!\n");
			return false;
		}
	}
	if(wifi_get_phy_mode() != PHY_MODE_11N)
		wifi_set_phy_mode(PHY_MODE_11N);
	return wifi_station_connect();
}
//------------------------------------------------------
bool ICACHE_FLASH_ATTR stop_wifi_ap()
{
	WIFI_MODE mode = wifi_get_opmode();
	mode &= ~SOFTAP_MODE;
	if(!wifi_set_mode(mode))
	{
		os_printf("Failed to disable AP mode!\n");
		return false;
	}
	return true;
}
//------------------------------------------------------
WIFI_MODE ICACHE_FLASH_ATTR init_esp_wifi()
{
	set_on_station_connect(on_wifi_connect);
	set_on_station_disconnect(on_wifi_disconnect);
	wifi_set_event_handler_cb(wifi_event_handler_cb);
	WIFI_MODE mode = wifi_get_opmode_default();
	wifi_set_mode(mode);
	stop_wifi_ap();
	return mode;
}
//------------------------------------------------------
