# Main application (`STM_Project_001`)

The main application of the repository. Its hand-written drivers live inside the project, under
`Core/MyProject_Drivers`:

| Folder | Driver |
|---|---|
| `Core/MyProject_Drivers/IO_Drivers` | [`io`](../drivers/io/): user button and LEDs |
| `Core/MyProject_Drivers/ADC_Drivers` | [`adc`](../drivers/adc/): ADC1 + DMA, VDDA, temperature, VBAT |

## What it does

- Reads the user button with a non-blocking 100 ms debounce and drives the four on-board LEDs.
- Samples five ADC channels continuously with DMA, averages 64 frames and computes the channel
  voltages, the real supply voltage (VDDA), the die temperature and VBAT.
- Lights the green LED when ADC + DMA started, the red LED when they failed.

## Work in progress: UART

USART3 is configured in CubeMX and `USART3_IRQHandler()` in `Core/Src/stm32f4xx_it.c` already
moves bytes between the UART and the `uart3` circular buffers. The UART driver itself
(`uart_ex` and `circular_buffer`) is not in `Core/MyProject_Drivers` yet, so the project does not
build until it is added. See
[Adding a driver to a CubeIDE project](../drivers/README.md#adding-a-driver-to-a-cubeide-project).

## Configuration

| Item | Value |
|---|---|
| System clock | HSI 16 MHz → PLL (M = 8, N = 168, P = 2) → **168 MHz**; APB1 42 MHz, APB2 84 MHz |
| ADC1 | Scan + continuous mode, software start, 5 conversions, DMA in circular mode |
| ADC channels | IN2 (**PA2**, potentiometer), IN3 (**PA3**), temperature sensor, VREFINT, VBAT |
| GPIO | User button **PA0**; LEDs **PD12** green, **PD13** orange, **PD14** red, **PD15** blue |
| USART3 | **PB10** TX, **PB11** RX, 115200 8N1; RXNE and TXE interrupts handled in `USART3_IRQHandler()` |
| Also configured | EXTI on PA1, DAC channel 1 on PA4 (not used yet) |

## Watching the results

Start a debug session and add `adcInfo` and `ioInfo` to **Live Expressions**. Useful fields:
`adcInfo.adcVoltageData`, `adcInfo.realVDDA`, `adcInfo.temprature`, `adcInfo.vBAT`,
`adcInfo.potPercentage` and `ioInfo.inputsInfo.userButton.inputStatus`.

## Build from the command line

```sh
python3 tools/build_cubeide_project.py STM_Project_001 --config Release
```
