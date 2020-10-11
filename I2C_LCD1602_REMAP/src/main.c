#include "main.h"
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
  char str01[6];
  gpio_init();
  i2c_mas_gpio_init();
  I2C_MASTER_SDA_LOW_SCL_LOW();
  LCD_ini();
  ets_delay_us(100000);
  LCD_String("String 1");
  LCD_SetPos(3,1);
  LCD_String("String 2");
  LCD_SetPos(6,2);
  ets_delay_us(1000000);
  system_soft_wdt_feed();
  ets_delay_us(1000000);
  system_soft_wdt_feed();
  while(1)
  {
    ets_delay_us(100000);
    system_soft_wdt_feed();
  }
}
//------------------------------------------------------
