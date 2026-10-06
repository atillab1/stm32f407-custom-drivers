/*
 * lcd_2x16_driver.c
 *
 *  Created on: Oct 6, 2026
 *      Author: atill
 */

#include "lcd_2x16_driver.h"

void LCD_Initialization(LCD_t *lcd){

	HAL_Delay(50);

	// LCD henuz 8-bit modda calisiyor ve yalnizca ust nibble gonderen ozel fonksiyonlar kullanilmali

	LCD_Send_InitNibble(lcd,0x30);
	HAL_Delay(5);
	LCD_Send_InitNibble(lcd,0x30);
	HAL_Delay(1);
	LCD_Send_InitNibble(lcd,0x30);
	HAL_Delay(1);

	// 4- bit moda gecis(sadece (0x2)
	LCD_Send_InitNibble(lcd, 0x20);
	HAL_Delay(1);

	// Artik 4 bitlik komutlarla calisiyorum
	LCD_Send_Command(lcd, LCD_Cmd_FunctionSet|LCD_4BIT_MODE, LCD_2_LINE,LCD_5x8_DOTS);
	HAL_Delay(1);

	lcd->display_control = LCD_Display_On;
	LCD_Send_Command(lcd, LCD_Cmd_DisplayOnOff | lcd->display_control);
	HAL_Delay(1);

	LCD_Send_Command(lcd, LCD_Cmd_ClearDisplay);
	HAL_Delay(2);

	LCD_Send_Command(lcd, LCD_Cmd_EntryMode | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT);
	HAL_Delay(1);

	 if(lcd->backlight)
		 LCD_Backlight_On(lcd);
	 else
		 LCD_Backlight_Off(lcd);
}
// LCD ekran baslarken kendini 8 bit modda zannediyor biz 4 bitlik modda kualllanacagiz
//bundan dolayi 8 bitin yalnizca ilk 4bitini gonderecegiz
void LCD_Send_InitNibble(LCD_t *lcd ,uint8_t nibble){

	//x x x x y y y y
	uint8_t data_t[2];
	uint8_t data_u = nibble & 0xF0;

	// EN = 1, RS = 0, RW = 0, BACKLIGHT
	data_t[0] = data_u | 0x04 | (lcd->backlight ? 0x08 : 0x00);
	// EN = 1, RS = 0, RW = 0, BACKLIGHT
	data_t[1] = data_u | (lcd->backlight ? 0x08 : 0x00);

	HAL_I2C_Master_Transmit(lcd->hi2c, lcd->i2c_addr, data_t, 2, 100);

}
