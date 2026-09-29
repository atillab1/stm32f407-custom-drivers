# STM32F407 Custom Drivers

[![CI](https://github.com/atillab1/stm32f407-custom-drivers/actions/workflows/ci.yml/badge.svg)](https://github.com/atillab1/stm32f407-custom-drivers/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

Hand-written peripheral drivers, a main application that uses them, and a series of timer, PWM
and UART experiments for the **STM32F4DISCOVERY** board (STM32F407VGT6), built with STM32CubeIDE
and the STM32Cube HAL.

- [`drivers/`](drivers/) — reusable drivers written on top of the HAL: GPIO with debounce,
  ADC with DMA, a circular buffer and an interrupt-driven UART.
- [`app/`](app/) — the main application, built from the GPIO and ADC drivers.
- [`examples/`](examples/) — one small CubeIDE project per peripheral feature, each with its own README.
- [`tests/`](tests/) — host-side unit tests for the hardware-independent code.

Every push is checked by CI: unit tests, static analysis and a firmware build of all ten projects.

## Hardware

| Item | Value |
|---|---|
| Board | STM32F4DISCOVERY |
| MCU | STM32F407VGT6 (Arm Cortex-M4F) |
| System clock | `app/`: HSI 16 MHz → PLL → **168 MHz**. Examples: HSI **16 MHz**, no PLL |
| On-board I/O used | User button on **PA0**; LEDs on **PD12** (green), **PD13** (orange), **PD14** (red), **PD15** (blue) |
| UART (example 009) | USART3: **PB10** TX, **PB11** RX, 115200 8N1 |
| Debug probe | On-board ST-LINK |

## Repository layout

```
.
├── drivers/                  hand-written drivers (see drivers/README.md)
│   ├── io/                   GPIO inputs/outputs with software debounce
│   ├── adc/                  ADC1 + DMA, averaging, VDDA and temperature
│   ├── circular_buffer/      ISR-safe single-producer/single-consumer byte queue
│   └── uart/                 interrupt-driven UART on two circular buffers, printf
├── app/                      main application (STM32CubeIDE project STM_Project_001)
├── examples/                 self-contained example projects 001–009
├── tests/                    host-side unit tests (make -C tests)
├── tools/                    command-line build of the CubeIDE projects
└── .github/workflows/        CI
```

The drivers live in one place and are **not copied** into the projects. `app/` and
`examples/009_UART_printf` show them in a virtual `UserDrivers` folder that links to
`drivers/`, so an edit made in CubeIDE changes the shared file.

## Main application

`app/` combines the GPIO and ADC drivers. Each driver keeps its state in one struct, and the main
loop calls a single update function per driver:

```c
IO_Info_t  ioInfo;
ADC_Info_t adcInfo;

IO_Initialization(&ioInfo);
ADC_Initialization(&adcInfo, &hadc1);

while (1)
{
  IO_Status_Control(&ioInfo);     /* drive LEDs, debounce the button */
  ADC_DMA_Conversion(&adcInfo);   /* average samples, update voltages */
}
```

At start-up the green LED lights when ADC + DMA started correctly, the red LED when they failed.
The ADC samples PA2 and PA3, the internal temperature sensor, VREFINT and VBAT.

## Drivers

| Driver | Depends on | What it does |
|---|---|---|
| [`io`](drivers/io/) | HAL GPIO, pin labels from CubeMX | Button and LED state in structs; non-blocking 100 ms debounce |
| [`adc`](drivers/adc/) | HAL ADC + DMA | Five-channel scan, 64-sample average, real VDDA from VREFINT, temperature, VBAT |
| [`circular_buffer`](drivers/circular_buffer/) | none | Fixed-size byte FIFO for one ISR and one main-loop user, unit tested on the host |
| [`uart`](drivers/uart/) | HAL UART, `circular_buffer` | RX and TX through interrupts and two circular buffers; `printf`-style output |

The API of each driver is described in [`drivers/README.md`](drivers/README.md).

## Examples

| # | Project | Peripheral and mode | What it shows |
|---|---|---|---|
| 001 | [`001_TIMER`](examples/001_TIMER/) | TIM2, external clock mode 1 (TI1FP1, PA0) | Counts rising edges on the user button |
| 002 | [`002_TIMER_External_Trigger_Mode2`](examples/002_TIMER_External_Trigger_Mode2/) | TIM2, external clock mode 2 (ETR, PA0) | Measures the frequency of the MCO1 clock output |
| 003 | [`003_TIMER_Internal_Trigger_Mode3`](examples/003_TIMER_Internal_Trigger_Mode3/) | TIM2 → TIM1 via ITR1, slave trigger mode | One timer starts another in hardware |
| 004 | [`004_TIMER_Slave_Mode_Reset_Mode`](examples/004_TIMER_Slave_Mode_Reset_Mode/) | TIM2, slave reset mode (PA0) | Milliseconds since the last button press |
| 005 | [`005_TIMER_Slave_Gated_Mode2`](examples/005_TIMER_Slave_Gated_Mode2/) | TIM2, slave gated mode (PA0) | Counts only while the button is held |
| 006 | [`006_TIMER_Input_Capture`](examples/006_TIMER_Input_Capture/) | TIM2, input capture on both edges (PA0) | Measures how long the button is held |
| 007 | [`007_TIMER_Output_Compare`](examples/007_TIMER_Output_Compare/) | TIM4, output compare toggle (PD12–PD15) | Four LEDs blinking 250 ms apart, no CPU involved |
| 008 | [`008_PWM`](examples/008_PWM/) | TIM3, PWM (PA6, PA7, PB0) | RGB LED fading through a colour table |
| 009 | [`009_UART_printf`](examples/009_UART_printf/) | USART3, interrupts (PB10, PB11) | UART driver with circular buffers and `printf` |

## Getting started

1. Clone the repository.
2. In STM32CubeIDE choose **File → Import → General → Existing Projects into Workspace**.
3. Select the cloned folder as the root directory, tick **Search for nested projects** and leave
   **Copy projects into workspace** unticked, so that edits go straight into the repository.
4. Select the projects you want and click **Finish**. The main application appears as
   `STM_Project_001`.
5. Connect the board over USB, build a project and start a debug session.

Tip: use a workspace folder that does not already contain projects with the same names, otherwise
CubeIDE refuses to import them.

Most examples store their counters in global variables. Add them to the **Live Expressions** view
while debugging to watch them change.

## Building and testing from the command line

Build any project with the Arm GNU toolchain, the same way CubeIDE does:

```sh
python3 tools/build_cubeide_project.py app --config Release
python3 tools/build_cubeide_project.py examples/008_PWM
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
| Firmware | Debug and Release builds of `app/` and all nine examples |

## Roadmap

- [x] UART driver with interrupt-driven RX and TX on circular buffers
- [x] Split the repository into `drivers/`, `app/` and `examples/`
- [x] Host-side unit tests for hardware-independent modules
- [x] CI with static analysis and firmware builds
- [ ] UART in the main application
- [ ] MISRA C checks in CI
- [ ] I2C and SPI sensor drivers
- [ ] FreeRTOS version of the main application

## License

The hand-written code in this repository is released under the [MIT License](LICENSE).
The STM32Cube HAL and CMSIS files in each project's `Drivers/` folder are provided by
STMicroelectronics and Arm under their own licenses; see the `LICENSE.txt` file in each package.
