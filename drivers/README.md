# Drivers

Our own driver library: every hand-written driver, one `.h`/`.c` pair per folder. The host
tests and the static analysis in CI run on these files.

The CubeIDE projects do not link to this folder. Each project keeps its own copy of the drivers it
uses, so every project builds on its own (see
[Adding a driver to a CubeIDE project](#adding-a-driver-to-a-cubeide-project)). When a driver is
improved inside a project, copy the change here too, so this folder always has the newest version.

| Driver | Copy in the projects | Depends on |
|---|---|---|
| [`io`](io/) | `STM_Project_001/Core/MyProject_Drivers/IO_Drivers` | HAL GPIO, pin labels from the project's `main.h` |
| [`adc`](adc/) | `STM_Project_001/Core/MyProject_Drivers/ADC_Drivers` | HAL ADC + DMA, LL ADC helper macros |
| [`circular_buffer`](circular_buffer/) | `009_UART_printf/Core`, `STM_Project_001/Core/MyProject_Drivers/Circular_Buffer_Drivers` | standard C only |
| [`uart`](uart/) | `009_UART_printf/Core`, `STM_Project_001/Core/MyProject_Drivers/UART_Drivers` | HAL UART, `circular_buffer` |
| [`lcd_2x16`](lcd_2x16/) | `010_I2C_2x16_LCD/Core`, `STM_Project_001/Core/MyProject_Drivers/LCD_I2C_Drivers` | HAL I2C |

## io

Describes the user button and the four LEDs as structs grouped in `IO_Info_t`. Outputs are written
from their `pinState` field; the button is debounced without blocking, using `HAL_GetTick()`
(`DEBOUNCE_TIME` = 100 ms).

| Function | Purpose |
|---|---|
| `IO_Initialization(IO_Info_t *)` | Bind the button and the LEDs to their pins and reset their state |
| `IO_Status_Control(IO_Info_t *)` | Write all outputs and update the debounced input; call every loop |
| `IO_Input_Control_With_Debounce(Input_State_t *)` | Debounce one input; the result is `inputStatus` |
| `IO_Output_Control(Output_State_t *)` | Write one output |

The pin names (`USER_BUTTON_Pin`, `LED_GREEN_Pin`, …) come from the labels set in CubeMX.

## adc

ADC1 converts five channels into `adcConvertedData[]` with DMA: IN2 (PA2), IN3 (PA3), the
temperature sensor, VREFINT and VBAT. The DMA callbacks set a `volatile` flag; the main loop
accumulates 64 complete frames and then refreshes the averaged results.

| Result field | Meaning |
|---|---|
| `adcAverageData[]` | Averaged raw value per channel (12-bit) |
| `realVDDA` | Supply voltage in mV, computed from VREFINT and its factory calibration |
| `adcVoltageData[]` | Channel voltages in V, based on `realVDDA` |
| `temprature` | Die temperature in °C, from the factory calibration values |
| `vBAT` | VBAT in V (the channel measures VBAT/2 on STM32F40x, the driver scales it back) |
| `potPercentage` | PA2 voltage as a share of the measured supply (`realVDDA`), 0–100 % |

| Function | Purpose |
|---|---|
| `ADC_Initialization(ADC_Info_t *, ADC_HandleTypeDef *)` | Start ADC + DMA; sets `adcErrorStatus` on failure |
| `ADC_DMA_Conversion(ADC_Info_t *)` | Process finished DMA frames; call every loop |
| `MAP_Voltage_To_Percantage(...)` | Linear map from a voltage range to an integer range, rounded to the nearest integer |

## circular_buffer

A fixed-size FIFO of bytes (`CIRCULAR_BUFFER_SIZE` = 512). It is meant for one writer and one
reader, typically an interrupt and the main loop: the writer only changes `head`, the reader only
changes `tail`, and both fields are `volatile`. One slot always stays free so that *empty*
(`head == tail`) and *full* can be told apart, which gives a capacity of 511 bytes.

| Function | Purpose |
|---|---|
| `Circular_Buffer_Init(cb)` | Reset to empty |
| `Circular_Buffer_Enqueue(cb, data)` | Append one byte; `false` if full (nothing is overwritten) |
| `Circular_Buffer_Dequeue(cb, &data)` | Take the oldest byte; `false` if empty |
| `Circular_Buffer_Is_Empty(cb)` / `Circular_Buffer_Is_Full(cb)` | State checks |
| `Circular_Buffer_Count(cb)` | Number of stored bytes |

It has no hardware dependency and is unit tested on the host: `make -C tests`.

## uart

Interrupt-driven UART on top of the HAL handle, with one circular buffer for received bytes and
one for bytes waiting to be sent.

- **RX**: the USART interrupt reads `DR` on RXNE and enqueues the byte into `cbIn`.
- **TX**: `UARTx_Write()` enqueues into `cbOut`. If the transmitter is idle (TXE interrupt off) it
  writes the first byte to `DR` itself and enables the TXE interrupt; the interrupt then sends the
  rest and disables itself when `cbOut` is empty.

| Function | Purpose |
|---|---|
| `UARTx_Initilalization(uart, huart, cbIn, cbOut)` | Bind the HAL handle and buffers, enable the RX interrupt |
| `UARTx_Write(uart, ch)` | Queue one character for sending |
| `UARTx_Put_String(uart, str)` | Queue a string |
| `UARTx_Printf(uart, fmt, ...)` | Format up to 255 characters and queue them |
| `UARTx_ReadLine(uart, buf, maxLen)` | Collect received bytes into `buf` until `"\r\n"`; returns `true` once when a complete line is ready |

`UARTx_ReadLine()` is meant to be called from the main loop with the same buffer every time: it
keeps the partial line in `buf` between calls and reports the finished line on the following call.
It only recognises **CR+LF** line endings, so set the terminal to send CR+LF on Enter. Its
state is kept in static variables, so it serves one UART at a time.

The interrupt handler lives in the project's `stm32f4xx_it.c`
(see [`009_UART_printf`](../009_UART_printf/)). It must check both the TXE flag
**and** that the TXE interrupt is enabled before touching `cbOut`.

`HAL_UART_IRQHandler()` switches the RXNE interrupt off when it sees an overrun. `STM_Project_001`
switches it back on in `HAL_UART_ErrorCallback()` (`Core/Src/main.c`).

Limitations: characters are dropped when `cbOut` is full, and received bytes stay in `cbIn` until
the application reads them with `Circular_Buffer_Dequeue()`.

## lcd_2x16

Drives an HD44780 2x16 character LCD through a PCF8574 I2C backpack. The LCD runs in 4-bit mode:
every byte goes out as two nibbles, and each nibble is written twice, once with EN = 1 and once
with EN = 0, so the LCD latches it on the falling edge. The other bits of the PCF8574 byte are
RS (`0x01`, data instead of command), EN (`0x04`) and the backlight (`0x08`).

The state lives in `LCD_t`: the I2C handle, the address (`LCD_I2C_DEVICE_ADDRESS` = `0x4E`, the
8-bit form of 0x27 that HAL expects), the size and the backlight flag.

| Function | Purpose |
|---|---|
| `LCD_Initialization(lcd)` | Reset sequence (`0x30` three times, `0x20`), then 4-bit 2-line mode, display on, clear, entry mode |
| `LCD_Clear(lcd)` / `LCD_Home(lcd)` | Clear the screen / move the cursor to the top left |
| `LCD_Set_Cursor(lcd, row, column)` | Move the cursor; row 0 starts at DDRAM `0x00`, row 1 at `0x40` |
| `LCD_Send_Char` / `LCD_Send_String` | Write text at the cursor |
| `LCD_Printf(lcd, fmt, ...)` | Format up to 63 characters and write them |
| `LCD_Scrool_Text(lcd, text, row, delayMs)` | Scroll a text longer than the row across it; blocks while scrolling |
| `LCD_Cursor_Show` / `LCD_Cursor_Hide` | Show or hide the cursor |
| `LCD_Backlight_On` / `LCD_Backlight_Off` | Switch the backlight |

`%f` in `LCD_Printf()` needs float support in newlib-nano: **Project → Properties → C/C++ Build →
Settings → MCU Settings → Use float with printf from newlib-nano**.

## Adding a driver to a CubeIDE project

`STM_Project_001` keeps its drivers under `Core/MyProject_Drivers`, one folder per driver:

```
Core/MyProject_Drivers/
├── ADC_Drivers/
│   ├── Inc/adc_driver.h
│   └── Src/adc_driver.c
├── Circular_Buffer_Drivers/
│   ├── Inc/circular_buffer.h
│   └── Src/circular_buffer.c
├── IO_Drivers/
│   ├── Inc/io_driver.h
│   └── Src/io_driver.c
└── UART_Drivers/
    ├── Inc/uart_ex.h
    └── Src/uart_ex.c
```

To add another one:

1. Right-click `Core/MyProject_Drivers` → **New → Folder** → `<Name>_Drivers`, and inside it two
   folders, `Inc` and `Src`.
2. Copy the `.h` file from this library into `Inc` and the `.c` file into `Src`.
3. **Project → Properties → C/C++ Build → Settings → MCU GCC Compiler → Include paths**: add
   `${workspace_loc:/${ProjName}/Core/MyProject_Drivers/<Name>_Drivers/Inc}` for **both** Debug
   and Release.

`Core` is already a source folder, so the `.c` file is compiled without any further setting.
