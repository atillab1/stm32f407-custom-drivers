# STM32F407 Custom Drivers

[![CI](https://github.com/atillab1/stm32f407-custom-drivers/actions/workflows/ci.yml/badge.svg)](https://github.com/atillab1/stm32f407-custom-drivers/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

Hand-written peripheral drivers, a main application that uses them, and a series of timer, PWM
and UART experiments for the **STM32F4DISCOVERY** board (STM32F407VGT6), built with STM32CubeIDE
and the STM32Cube HAL.

- [`STM_Project_001/`](STM_Project_001/) — the main application, built from the GPIO, ADC, UART
  and LCD drivers.
- `001_TIMER_External_Trigger_Mode/` … `010_I2C_2x16_LCD/` — one small CubeIDE project per peripheral
  feature, each with its own README.
- [`drivers/`](drivers/) — our own driver library: GPIO with debounce, ADC with DMA, a circular
  buffer, an interrupt-driven UART and a 2x16 I2C LCD.
- [`tests/`](tests/) — host-side unit tests for the hardware-independent code.

Every push is checked by CI: unit tests, static analysis and a firmware build of all eleven projects.

## Hardware

| Item | Value |
|---|---|
| Board | STM32F4DISCOVERY |
| MCU | STM32F407VGT6 (Arm Cortex-M4F) |
| System clock | `STM_Project_001`: HSI 16 MHz → PLL → **168 MHz**. Examples: HSI **16 MHz**, no PLL |
| On-board I/O used | User button on **PA0**; LEDs on **PD12** (green), **PD13** (orange), **PD14** (red), **PD15** (blue) |
| UART (`STM_Project_001`, `009_UART_printf`) | USART3: **PB10** TX, **PB11** RX, 115200 8N1 |
| LCD (`STM_Project_001`, `010_I2C_2x16_LCD`) | 2x16 LCD with PCF8574 backpack on I2C1: **PB6** SCL, **PB7** SDA, address 0x27 |
| Debug probe | On-board ST-LINK |

## Repository layout

```
.
├── STM_Project_001/          main application (STM32CubeIDE project)
├── 001_TIMER_External_Trigger_Mode/ … 010_I2C_2x16_LCD/
│                             example projects, one CubeIDE project each
├── drivers/                  our own driver library (see drivers/README.md)
│   ├── io/                   GPIO inputs/outputs with software debounce
│   ├── adc/                  ADC1 + DMA, averaging, VDDA and temperature
│   ├── circular_buffer/      ISR-safe single-producer/single-consumer byte queue
│   ├── uart/                 interrupt-driven UART on two circular buffers, printf
│   └── lcd_2x16/             2x16 character LCD over an I2C (PCF8574) backpack
├── tests/                    host-side unit tests (make -C tests)
├── tools/                    command-line build of the CubeIDE projects
└── .github/workflows/        CI
```

The repository root is not a CubeIDE project. Every project sits in its own folder and keeps its
own copy of the drivers it uses, so each one builds on its own: `STM_Project_001` under
`Core/MyProject_Drivers`, `009_UART_printf` and `010_I2C_2x16_LCD` in `Core/Inc` and `Core/Src`. `drivers/` holds the
library version of every driver.

## Main application

`STM_Project_001` combines the GPIO, ADC, UART and LCD drivers. Each driver keeps its state in one
struct, and the main loop calls a single update function per driver:

```c
IO_Initialization(&ioInfo);
UARTx_Initilalization(&uart3, &huart3, &uartCbIn, &uartCbOut);
LCD_Initialization(&lcd);
ADC_Initialization(&adcInfo, &hadc1);

while (1)
{
  IO_Status_Control(&ioInfo);     /* drive LEDs, debounce the button */
  ADC_DMA_Conversion(&adcInfo);   /* average samples, update voltages */
  /* DAC output, LED commands from USART3, LCD refresh every 500 ms */
}
```

At start-up the board reports the ADC and DAC status on USART3 and on the LCD. The green LED lights
when the ADC started, the red LED when the ADC or the DAC failed. The ADC samples PA2 and PA3, the internal temperature
sensor, VREFINT and VBAT; the LCD then shows the potentiometer position and the die temperature.

## Drivers

| Driver | Depends on | What it does |
|---|---|---|
| [`io`](drivers/io/) | HAL GPIO, pin labels from CubeMX | Button and LED state in structs; non-blocking 100 ms debounce |
| [`adc`](drivers/adc/) | HAL ADC + DMA | Five-channel scan, 64-sample average, real VDDA from VREFINT, temperature, VBAT |
| [`circular_buffer`](drivers/circular_buffer/) | none | Fixed-size byte FIFO for one ISR and one main-loop user, unit tested on the host |
| [`uart`](drivers/uart/) | HAL UART, `circular_buffer` | RX and TX through interrupts and two circular buffers; `printf`-style output |
| [`lcd_2x16`](drivers/lcd_2x16/) | HAL I2C | HD44780 2x16 LCD through a PCF8574 backpack in 4-bit mode; strings, `printf`, scrolling |

The API of each driver is described in [`drivers/README.md`](drivers/README.md).

## Examples

| # | Project | Peripheral and mode | What it shows |
|---|---|---|---|
| 001 | [`001_TIMER_External_Trigger_Mode`](001_TIMER_External_Trigger_Mode/) | TIM2, external clock mode 1 (TI1FP1, PA0) | Counts rising edges on the user button |
| 002 | [`002_TIMER_External_Trigger_Mode2`](002_TIMER_External_Trigger_Mode2/) | TIM2, external clock mode 2 (ETR, PA0) | Measures the frequency of the MCO1 clock output |
| 003 | [`003_TIMER_Internal_Trigger_Mode3`](003_TIMER_Internal_Trigger_Mode3/) | TIM2 → TIM1 via ITR1, slave trigger mode | One timer starts another in hardware |
| 004 | [`004_TIMER_Slave_Mode_Reset_Mode`](004_TIMER_Slave_Mode_Reset_Mode/) | TIM2, slave reset mode (PA0) | Milliseconds since the last button press |
| 005 | [`005_TIMER_Slave_Gated_Mode2`](005_TIMER_Slave_Gated_Mode2/) | TIM2, slave gated mode (PA0) | Counts only while the button is held |
| 006 | [`006_TIMER_Input_Capture`](006_TIMER_Input_Capture/) | TIM2, input capture on both edges (PA0) | Measures how long the button is held |
| 007 | [`007_TIMER_Output_Compare`](007_TIMER_Output_Compare/) | TIM4, output compare toggle (PD12–PD15) | Four LEDs blinking 250 ms apart, no CPU involved |
| 008 | [`008_PWM`](008_PWM/) | TIM3, PWM (PA6, PA7, PB0) | RGB LED fading through a colour table |
| 009 | [`009_UART_printf`](009_UART_printf/) | USART3, interrupts (PB10, PB11) | UART driver with circular buffers and `printf` |
| 010 | [`010_I2C_2x16_LCD`](010_I2C_2x16_LCD/) | I2C1 (PB6, PB7) | 2x16 character LCD driver over a PCF8574 backpack |

## Getting started

1. Clone the repository.
2. Start STM32CubeIDE and choose the cloned folder itself as the workspace
   (**File → Switch Workspace → Other…**).
3. Choose **File → Import → General → Existing Projects into Workspace**, select the same folder
   as the root directory, tick **Search for nested projects** and leave **Copy projects into
   workspace** unticked.
4. Select the projects you want and click **Finish**. The main application appears as
   `STM_Project_001`.
5. Connect the board over USB, build a project and start a debug session.

With the repository as the workspace, a new project created with **Use default location** lands
in the repository, ready to commit. CubeIDE keeps its own settings in `.metadata/`, which Git
ignores. Each project folder has the same name as its project; CubeIDE requires this for projects
inside the workspace folder.

Most examples store their counters in global variables. Add them to the **Live Expressions** view
while debugging to watch them change.

## Building and testing from the command line

Build any project with the Arm GNU toolchain, the same way CubeIDE does:

```sh
python3 tools/build_cubeide_project.py STM_Project_001 --config Release
python3 tools/build_cubeide_project.py 008_PWM
```

Run the host-side unit tests with a native C compiler:

```sh
make -C tests
```

## Continuous integration

[`.github/workflows/ci.yml`](.github/workflows/ci.yml) runs on every push to `main` and every pull
request:

| Job | Checks |
|---|---|
| Host unit tests | `make -C tests` |
| Static analysis | cppcheck (warning, performance, portability) on `drivers/` |
| Firmware | Debug and Release builds of `STM_Project_001` and all ten examples |

## Roadmap

- [x] UART driver with interrupt-driven RX and TX on circular buffers
- [x] One folder per CubeIDE project, plus a driver library in `drivers/`
- [x] Host-side unit tests for hardware-independent modules
- [x] CI with static analysis and firmware builds
- [x] UART driver in the main application
- [x] I2C character LCD driver, also used in the main application
- [ ] MISRA C checks in CI
- [ ] I2C and SPI sensor drivers
- [ ] FreeRTOS version of the main application

## License

The hand-written code in this repository is released under the [MIT License](LICENSE).
The STM32Cube HAL and CMSIS files in each project's `Drivers/` folder are provided by
STMicroelectronics and Arm under their own licenses; see the `LICENSE.txt` file in each package.
