# 013 – Sleep mode

The four LEDs blink once a second for 10 seconds, then the MCU enters **Sleep mode**. Pressing the
user button (PA0) wakes it up and the cycle starts again.

## Configuration

| Setting | Value |
|---|---|
| System clock | HSE 8 MHz → PLL (M = 4, N = 168, P = 2) → **168 MHz** |
| User button | **PA0**, EXTI0 on the rising edge |
| LEDs | PD12–PD15 outputs |

## How it works

1. `HAL_SuspendTick()` stops the SysTick interrupt. Otherwise the 1 ms tick would wake the core
   straight away.
2. `HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE)` stops the CPU clock. The
   peripherals and the RAM keep running.
3. The EXTI0 interrupt from the button wakes the core and execution continues after the call.
4. `HAL_ResumeTick()` restarts SysTick and the 10-second counter starts again.

In Sleep mode only the CPU clock stops, so wake-up is immediate and nothing has to be
reconfigured.
