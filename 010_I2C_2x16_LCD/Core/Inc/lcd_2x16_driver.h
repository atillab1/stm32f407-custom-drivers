/*
 * lcd_2x16_driver.h
 *
 *  Created on: Oct 6, 2026
 *      Author: atill
 */

#ifndef INC_LCD_2X16_DRIVER_H_
#define INC_LCD_2X16_DRIVER_H_

#include "main.h"
#include "stdbool.h"
#include "stdarg.h"

#define  LCD_I2C_DEVICE_ADDRESS   0x4E


// Function Set Flags
#define LCD_8BIT_MODE		0x10
#define LCD_4BIT_MODE		0x00
#define LCD_2_LINE			0x08
#define LCD_1_LINE			0x00
#define LCD_5x10_DOTS		0x04
#define LCD_5x8_DOTS        0x00

// Entry Mode Flags
#define LCD_ENTRY_RIGHT					0x00
#define LCD_ENTRY_LEFT 					0x02
#define LCD_ENTRY_SHIFT_INCREMENT		0x01
#define LCD_ENTRY_SHIFT_DECREMENT		0x00

/* ************* LCD Commands  ************* */
typedef enum{

 LCD_Cmd_ClearDisplay 			= 0x01, // 0<<1
 LCD_Cmd_ReturnHome				= 0x02, // 1<<1
 LCD_Cmd_EntryMode 				= 0x04,
 LCD_Cmd_DisplayOnOff 			= 0x08,
 LCD_Cmd_CursorShift 			= 0x10,
 LCD_Cmd_FunctionSet 			= 0x20,
 LCD_Cmd_Set_CGRAM_ADDRESS 		= 0x40,
 LCD_Cmd_Set_DDRAM_ADDRESS 		= 0x80
}LCD_Commands;

/* ************* Display Setups  ************* */
typedef enum{
	LCD_Display_On 		= 0x04,
	LCD_Cursor_On 		= 0x02,
	LCD_Cursor_Blink_On = 0x01

}LCD_Display_Control_Flags;

/* ************* LCD Structure  ************* */
typedef struct{
	I2C_HandleTypeDef 			*hi2c;
	uint8_t 					i2c_addr;
	uint8_t 					columns;
	uint8_t						rows;
	bool 						backlight;
	uint8_t						display_control;

}LCD_t;

/* ************* LCD APIs Functions  ************* */

void LCD_Initialization(LCD_t *lcd);
void LCD_Clear(LCD_t *lcd);
void LCD_Home (LCD_t *lcd);
void LCD_Set_Cursor(LCD_t *lcd,uint8_t row, uint8_t column);
void LCD_Send_Char(LCD_t *lcd, char ch );
void LCD_Send_String(LCD_t *lcd, const char *str);
void LCD_Send_Command(LCD_t *lcd, uint8_t command);
void LCD_Send_Data(LCD_t *lcd, uint8_t data);
void LCD_Backlight_On(LCD_t *lcd);
void LCD_Backlight_Off(LCD_t *lcd);
void LCD_Cursor_Show(LCD_t *lcd);
void LCD_Cursor_Hide(LCD_t *lcd);
void LCD_Printf(LCD_t *lcd, const char * format, ...);
void LCD_Scrool_Text(LCD_t *lcd, const char *text ,uint8_t row, uint16_t delayMs);
void LCD_Send_InitNibble(LCD_t *lcd ,uint8_t nibble);


#endif /* INC_LCD_2X16_DRIVER_H_ */
