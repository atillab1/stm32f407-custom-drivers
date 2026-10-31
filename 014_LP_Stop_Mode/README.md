# 014 – Stop mode

The same cycle as [`013_LP_Sleep_Mode`](../013_LP_Sleep_Mode/), but the MCU enters **Stop mode**:
10 seconds of blinking LEDs, then Stop mode until the user button (PA0) is pressed.

## Configuration

| Setting | Value |
|---|---|
| System clock | HSE 8 MHz → PLL (M = 4, N = 168, P = 2) → **168 MHz** |
| User button | **PA0**, EXTI0 on the rising edge |
| LEDs | PD12–PD15 outputs |

## How it works

1. `HAL_SuspendTick()` stops the SysTick interrupt.
2. `HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI)` stops all clocks in the 1.2 V
   domain, including the PLL and the HSE. RAM and registers keep their contents.
3. The EXTI0 interrupt from the button wakes the MCU. It restarts on the **HSI (16 MHz)**, not on
   the PLL.
4. `SystemClock_Config()` switches back to the HSE and PLL (168 MHz), then `HAL_ResumeTick()`
   restarts SysTick.

Without step 4 the program would keep running at 16 MHz after the first wake-up, and every
timing based on the 168 MHz clock would be wrong.
