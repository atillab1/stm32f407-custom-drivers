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
	LCD_Send_Command(lcd, LCD_Cmd_FunctionSet|LCD_4BIT_MODE| LCD_2_LINE| LCD_5x8_DOTS);
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

void LCD_Send_Command(LCD_t *lcd, uint8_t cmd)
{
	uint8_t data_u = cmd & 0xF0;
	uint8_t data_l = (cmd<<4) & 0xF0;

	uint8_t data_t[4];

	data_t[0] = (data_u) | 0x04 | (lcd->backlight ? 0x08 : 0x00); // EN=1
	data_t[1] = 		(data_u) | (lcd->backlight ? 0x08 : 0x00); // EN=0
	data_t[2] = (data_l) | 0x04 | (lcd->backlight ? 0x08 : 0x00);
	data_t[3] = 		(data_l) |  (lcd->backlight ? 0x08 : 0x00);

	HAL_I2C_Master_Transmit(lcd->hi2c, lcd->i2c_addr, data_t, 4, 100);
	HAL_Delay(1);
}

void LCD_Clear(LCD_t *lcd)
{
	LCD_Send_Command(lcd, LCD_Cmd_ClearDisplay);
		HAL_Delay(2);

}

void LCD_Home (LCD_t *lcd)
{
	LCD_Send_Command(lcd, LCD_Cmd_ReturnHome);
	HAL_Delay(2);
}

void LCD_Send_Char(LCD_t *lcd, char ch )
{
	LCD_Send_Data(lcd, (uint8_t)ch);
	HAL_Delay(2);
}

void LCD_Cursor_Show(LCD_t *lcd)
{
	lcd->display_control |= LCD_Cursor_On;
	LCD_Send_Command(lcd, LCD_Cmd_DisplayOnOff|lcd->display_control);
}

void LCD_Cursor_Hide(LCD_t *lcd)
{
	lcd->display_control &= LCD_Cursor_On;
		LCD_Send_Command(lcd, LCD_Cmd_DisplayOnOff|lcd->display_control);
}

void LCD_Set_Cursor(LCD_t *lcd,uint8_t row, uint8_t column)
{

	const uint8_t row_offset [] = {0x00, 0x40};

	if(row >= lcd->rows)
		row = 0;

	if(column>= lcd->columns)
		column = 15;

	LCD_Send_Command(lcd, LCD_Cmd_Set_DDRAM_ADDRESS | (column + row_offset[row]));

}

void LCD_Send_String(LCD_t *lcd, const char *str)
{
	while(*str)
	{
		LCD_Send_Data(lcd, (uint8_t)*str++);
	}


}
void LCD_Send_Data(LCD_t *lcd, uint8_t data)
{
	uint8_t data_u = data & 0xF0;
	uint8_t data_l = (data<<4) & 0xF0;

	uint8_t data_t[4];

		data_t[0] = data_u | 0x05 | (lcd->backlight ? 0x08 : 0x00); // bu islemler instruction setten dolayi boyle RS = 1 , EN = 1
		data_t[1] = data_u | 0x01|(lcd->backlight ? 0x08 : 0x00);// rs 1
		data_t[2] = data_l |0x05 | (lcd->backlight ? 0x08 : 0x00); // rs 1
		data_t[3] = data_l | 0x01 |(lcd->backlight ? 0x08 : 0x00);// rs 1

		HAL_I2C_Master_Transmit(lcd->hi2c, lcd->i2c_addr, data_t, 4, 100);
			HAL_Delay(1);
}


void LCD_Backlight_On(LCD_t *lcd){

	lcd->backlight = true;
	LCD_Send_Command(lcd, 0x00);

}
void LCD_Backlight_Off(LCD_t *lcd){
	lcd->backlight = false;
	LCD_Send_Command(lcd, 0x00);
}

void LCD_Printf(LCD_t *lcd, const char * format, ...)
{

	char buffer[64];

	va_list args;
	va_start(args,format);
	vsniprintf(buffer,sizeof(buffer),format,args);
	va_end(args);

	LCD_Send_String(lcd, buffer);
}

void LCD_Scrool_Text(LCD_t *lcd, const char *text ,uint8_t row, uint16_t delayMs)
{
	char buffer [17];   		//ekranda tek seferde 16 karakter var +1 de null karakter
	uint8_t len = strlen(text);

	if(len <= lcd->columns)
	{
		LCD_Set_Cursor(lcd, row, 0);
		LCD_Send_String(lcd, text);

	return;
	}

	for(uint8_t i = 0; i< len-lcd->columns; i++)
	{
		strncpy(buffer,&text[i],lcd->columns);
		buffer[lcd->columns] = '\0';

		LCD_Set_Cursor(lcd, row, 0);
		LCD_Send_String(lcd, buffer);
		HAL_Delay(delayMs);
	}
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
