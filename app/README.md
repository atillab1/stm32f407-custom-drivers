# Main application (`STM_Project_001`)

The main application of the repository. It uses the [`io`](../drivers/io/) and
[`adc`](../drivers/adc/) drivers, which CubeIDE shows under the linked `UserDrivers` folder.

## What it does

- Reads the user button with a non-blocking 100 ms debounce and drives the four on-board LEDs.
- Samples five ADC channels continuously with DMA, averages 64 frames and computes the channel
  voltages, the real supply voltage (VDDA), the die temperature and VBAT.
- Lights the green LED when ADC + DMA started, the red LED when they failed.

## Configuration

| Item | Value |
|---|---|
| System clock | HSI 16 MHz → PLL (M = 8, N = 168, P = 2) → **168 MHz**; APB1 42 MHz, APB2 84 MHz |
| ADC1 | Scan + continuous mode, software start, 5 conversions, DMA in circular mode |
| ADC channels | IN2 (**PA2**, potentiometer), IN3 (**PA3**), temperature sensor, VREFINT, VBAT |
| GPIO | User button **PA0**; LEDs **PD12** green, **PD13** orange, **PD14** red, **PD15** blue |
| Also configured | EXTI on PA1, DAC channel 1 on PA4 (not used yet) |

## Watching the results

Start a debug session and add `adcInfo` and `ioInfo` to **Live Expressions**. Useful fields:
`adcInfo.adcVoltageData`, `adcInfo.realVDDA`, `adcInfo.temprature`, `adcInfo.vBAT`,
`adcInfo.potPercentage` and `ioInfo.inputsInfo.userButton.inputStatus`.

## Build from the command line

```sh
python3 tools/build_cubeide_project.py app --config Release
```
