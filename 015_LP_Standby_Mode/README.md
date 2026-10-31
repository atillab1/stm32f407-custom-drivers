# 015 – Standby mode

The four LEDs blink once a second for 10 seconds, then the MCU enters **Standby mode**, the
lowest-power mode of the STM32F4.

## Configuration

| Setting | Value |
|---|---|
| System clock | HSE 8 MHz → PLL (M = 4, N = 168, P = 2) → **168 MHz** |
| User button | **PA0**, EXTI0 on the rising edge |
| LEDs | PD12–PD15 outputs |

## How it works

`HAL_PWR_EnterSTANDBYMode()` switches off the 1.2 V domain: the core, the clocks, the SRAM and the
peripheral registers all lose power. Only the backup domain stays on.

Waking up from Standby is like a reset: the program starts again from `main()` and all variables
are lost. The STM32F4 leaves Standby on the NRST (reset) pin, the RTC alarm/wake-up events or a
rising edge on the WKUP pin (PA0). This project does not call `HAL_PWR_EnableWakeUpPin()`, so
in its current form the **reset button** brings the board back. The EXTI0 interrupt does not
work in Standby.
