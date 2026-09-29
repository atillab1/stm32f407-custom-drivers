# STM32F407 Custom Drivers

Hand-written peripheral drivers and a series of timer and PWM experiments for the
**STM32F4DISCOVERY** board (STM32F407VGT6), built with STM32CubeIDE and the STM32Cube HAL.

The repository has two parts:

- **Main application** (repository root): a small driver layer written on top of the HAL,
  currently covering GPIO and ADC with DMA.
- **Examples** (`001_…` to `008_…`): one self-contained CubeIDE project per timer feature,
  each with its own README.

## Hardware

| Item | Value |
|---|---|
| Board | STM32F4DISCOVERY |
| MCU | STM32F407VGT6 (Arm Cortex-M4F) |
| System clock | Main application: HSI 16 MHz → PLL → **168 MHz**. Examples: HSI **16 MHz**, no PLL |
| On-board I/O used | User button on **PA0**; LEDs on **PD12** (green), **PD13** (orange), **PD14** (red), **PD15** (blue) |
| Debug probe | On-board ST-LINK |

## Toolchain

- STM32CubeIDE 1.15 (project files generated with STM32CubeMX 6.11.1)
- STM32Cube HAL for STM32F4

## Main application: driver layer

The drivers live in [`Core/MyProject_Drivers/`](Core/MyProject_Drivers/). Each driver keeps all of its
state in one struct, and the main loop calls a single update function per driver:

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

### IO driver — [`IO_Drivers/`](Core/MyProject_Drivers/IO_Drivers/)

- Describes every input and output as a struct (port, pin, state) instead of scattering `HAL_GPIO_*` calls.
- Non-blocking software debounce for the user button (100 ms, based on `HAL_GetTick()`; no `HAL_Delay()`).

| Function | Purpose |
|---|---|
| `IO_Initialization(IO_Info_t *)` | Bind the button and the four LEDs to their pins and reset their state |
| `IO_Status_Control(IO_Info_t *)` | Apply the requested LED states and update the debounced button state; call every loop |
| `IO_Input_Control_With_Debounce(Input_State_t *)` | Debounce a single input |
| `IO_Output_Control(Output_State_t *)` | Drive a single output |

### ADC driver — [`ADC_Drivers/`](Core/MyProject_Drivers/ADC_Drivers/)

- ADC1 scans five channels into memory with DMA: **PA2** (IN2), **PA3** (IN3), the internal temperature
  sensor, VREFINT and VBAT.
- Averages 64 DMA frames per channel.
- Computes the actual VDDA from VREFINT and the factory calibration, and uses it for the voltage and
  temperature conversions, so readings stay correct when the supply is not exactly 3.3 V.
- Maps the voltage on PA2 (potentiometer) to 0–100 %.

| Function | Purpose |
|---|---|
| `ADC_Initialization(ADC_Info_t *, ADC_HandleTypeDef *)` | Start ADC + DMA; sets `adcErrorStatus` on failure |
| `ADC_DMA_Conversion(ADC_Info_t *)` | Accumulate finished DMA frames and refresh the averaged results; call every loop |
| `MAP_Voltage_To_Percantage(...)` | Linear map from a voltage range to an integer percentage |

## Examples

| # | Project | Peripheral and mode | What it shows |
|---|---|---|---|
| 001 | [`001_TIMER`](001_TIMER/) | TIM2, external clock mode 1 (TI1FP1, PA0) | Counts rising edges on the user button |
| 002 | [`002_TIMER_External_Trigger_Mode2`](002_TIMER_External_Trigger_Mode2/) | TIM2, external clock mode 2 (ETR, PA0) | Measures the frequency of the MCO1 clock output |
| 003 | [`003_TIMER_Internal_Trigger_Mode3`](003_TIMER_Internal_Trigger_Mode3/) | TIM2 → TIM1 via ITR1, slave trigger mode | One timer starts another in hardware |
| 004 | [`004_TIMER_Slave_Mode_Reset_Mode`](004_TIMER_Slave_Mode_Reset_Mode/) | TIM2, slave reset mode (PA0) | Milliseconds since the last button press |
| 005 | [`005_TIMER_Slave_Gated_Mode2`](005_TIMER_Slave_Gated_Mode2/) | TIM2, slave gated mode (PA0) | Counts only while the button is held |
| 006 | [`006_TIMER_Input_Capture`](006_TIMER_Input_Capture/) | TIM2, input capture on both edges (PA0) | Measures how long the button is held |
| 007 | [`007_TIMER_Output_Compare`](007_TIMER_Output_Compare/) | TIM4, output compare toggle (PD12–PD15) | Four LEDs blinking 250 ms apart, no CPU involved |
| 008 | [`008_PWM`](008_PWM/) | TIM3, PWM (PA6, PA7, PB0) | RGB LED fading through a colour table |

## Getting started

1. Clone the repository.
2. In STM32CubeIDE choose **File → Import → General → Existing Projects into Workspace**.
3. Select the cloned folder as the root directory, tick **Search for nested projects** and leave
   **Copy projects into workspace** unticked, so that edits go straight into the repository.
4. Select the projects you want and click **Finish**.
5. Connect the board over USB, build the project and start a debug session.

Most examples store their counters in global variables. Add them to the **Live Expressions** view while
debugging to watch them change.

## Repository layout

```
.
├── Core/                       main application (CubeMX-generated code + user code)
│   └── MyProject_Drivers/      hand-written drivers
│       ├── IO_Drivers/
│       └── ADC_Drivers/
├── Drivers/                    STM32Cube HAL and CMSIS (vendor code)
├── STM_Project_001.ioc         CubeMX configuration of the main application
├── 001_TIMER/ … 008_PWM/       self-contained example projects
└── LICENSE
```

## Roadmap

- [ ] UART driver with interrupt-driven RX and TX on circular buffers (in progress)
- [ ] Split the repository into `drivers/`, `app/` and `examples/`
- [ ] Host-side unit tests for hardware-independent modules
- [ ] Static analysis (cppcheck with MISRA C rules) in GitHub Actions
- [ ] I2C and SPI sensor drivers
- [ ] FreeRTOS version of the main application

## License

The hand-written code in this repository is released under the [MIT License](LICENSE).
The STM32Cube HAL and CMSIS files in the `Drivers/` folders are provided by STMicroelectronics and Arm
under their own licenses; see the `LICENSE.txt` file in each package.
