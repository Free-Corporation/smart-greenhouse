#ifndef LCD_H_
#define LCD_H_
//------------------------------------------------------
#include "osapi.h"
//------------------------------------------------------
void I2C_SendByteByADDR(uint8_t c,uint8_t addr);
//------------------------------------------------------
#define e_set() LCD_WriteByteI2CLCD(portlcd|=0x04) //E line 1 (Enable signal)
#define e_reset() LCD_WriteByteI2CLCD(portlcd&=~0x04) //E line 0 (Enable signal)
#define rs_set() LCD_WriteByteI2CLCD(portlcd|=0x01) //RS line 1 (Register selection)
#define rs_reset() LCD_WriteByteI2CLCD(portlcd&=~0x01) //RS line 0 (Register selection)
#define setled() LCD_WriteByteI2CLCD(portlcd|=0x08) // BL line 1 (Back light control)
#define setwrite() LCD_WriteByteI2CLCD(portlcd&=~0x02) //RW line 0 (Read/Write)
#define setread() LCD_WriteByteI2CLCD(portlcd&|=0x02) //RW line 1 (Read/Write)
//------------------------------------------------------
void LCD_ini(void);
void LCD_String(char* st);
void LCD_SetPos(uint8_t x,uint8_t y);
//------------------------------------------------------
#endif /* LCD_H_ */
