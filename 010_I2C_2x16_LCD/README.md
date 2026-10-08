# 010 – 2x16 character LCD over I2C

A 2x16 HD44780 character LCD with a PCF8574 I2C backpack, driven by the hand-written
`lcd_2x16_driver` over I2C1. The driver is described in
[`drivers/README.md`](../drivers/README.md#lcd_2x16).

## Wiring

| LCD module | Board |
|---|---|
| GND | GND |
| VCC | 5V |
| SDA | **PB7** (I2C1_SDA) |
| SCL | **PB6** (I2C1_SCL) |

PB6 and PB7 are 5 V tolerant, so the module can run from 5 V with its own pull-ups. If the screen
only shows a row of blocks, turn the contrast potentiometer on the backpack.

Most backpacks answer at address 0x27, which is `0x4E` in the 8-bit form HAL expects
(`LCD_I2C_DEVICE_ADDRESS`). Some use 0x3F (`0x7E`).

## Configuration

| Setting | Value |
|---|---|
| Peripheral | I2C1, standard mode, 100 kHz |
| Pins | PB6 SCL, PB7 SDA |
| System clock | HSI 16 MHz |
| printf | newlib-nano with float support, so `LCD_Printf("%f", ...)` works |

## Try it

`main.c` initialises the LCD, writes `Merhaba!`, then clears it and prints a temperature value with
`LCD_Printf()`:

```c
    /* USER CODE BEGIN 2 */
    LCD_Initialization(&lcd);
    LCD_Clear(&lcd);
    LCD_Set_Cursor(&lcd, 0, 0);
    LCD_Send_String(&lcd, "Merhaba!");
```

The same driver also runs in `STM_Project_001`, where it shows the potentiometer position and the
die temperature.
