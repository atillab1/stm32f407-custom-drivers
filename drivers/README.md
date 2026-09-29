# Drivers

Hand-written drivers used by the projects in this repository. Each folder holds one driver as a
`.h`/`.c` pair. CubeIDE projects include them through linked folders (see
[Using a driver in a CubeIDE project](#using-a-driver-in-a-cubeide-project)), so there is only one
copy of every file.

| Driver | Used by | Depends on |
|---|---|---|
| [`io`](io/) | `app` | HAL GPIO, pin labels from the project's `main.h` |
| [`adc`](adc/) | `app` | HAL ADC + DMA, LL ADC helper macros |
| [`circular_buffer`](circular_buffer/) | `uart`, host tests | standard C only |
| [`uart`](uart/) | `examples/009_UART_printf` | HAL UART, `circular_buffer` |

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
| `potPercentage` | PA2 voltage mapped to 0–100 % |

| Function | Purpose |
|---|---|
| `ADC_Initialization(ADC_Info_t *, ADC_HandleTypeDef *)` | Start ADC + DMA; sets `adcErrorStatus` on failure |
| `ADC_DMA_Conversion(ADC_Info_t *)` | Process finished DMA frames; call every loop |
| `MAP_Voltage_To_Percantage(...)` | Linear map from a voltage range to an integer range |

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

The interrupt handler lives in the project's `stm32f4xx_it.c`
(see [`examples/009_UART_printf`](../examples/009_UART_printf/)). It must check both the TXE flag
**and** that the TXE interrupt is enabled before touching `cbOut`.

Limitations: characters are dropped when `cbOut` is full, and received bytes stay in `cbIn` until
the application reads them with `Circular_Buffer_Dequeue()`.

## Using a driver in a CubeIDE project

The projects add drivers as linked folders, which CubeIDE stores in `.project` and `.cproject`:

1. Right-click the project → **New → Folder** → **Advanced** → **Folder is not located in the
   file system (Virtual Folder)** → name it `UserDrivers`.
2. Right-click `UserDrivers` → **New → Folder** → **Advanced** → **Link to alternate location** →
   **Variables…** and enter `PARENT-1-PROJECT_LOC/drivers/<name>` (`PARENT-2-…` for a project under
   `examples/`).
3. **Project → Properties → C/C++ Build → Settings → MCU GCC Compiler → Include paths**: add
   `${workspace_loc:/${ProjName}/UserDrivers/<name>}` for **both** Debug and Release.
4. **C/C++ General → Paths and Symbols → Source Location**: add `/UserDrivers` if it is not listed.
