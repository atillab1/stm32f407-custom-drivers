# 022 – Software timer library

A small software timer library on top of one hardware timer. Four LEDs toggle on independent
1, 2, 4 and 8 second timers, and the 1-second timer also drives a seconds/minutes/hours clock.
The same library is used in [`STM_Project_001`](../STM_Project_001/).

## Configuration

| Setting | Value |
|---|---|
| System clock | HSE 8 MHz → PLL (M = 4, N = 150, P = 2) → **150 MHz**; APB1 37.5 MHz |
| TIM14 | Timer clock 75 MHz, prescaler 74, period 999 → update interrupt every **1 ms** |
| LEDs | PD12 (1 s), PD13 (2 s), PD14 (4 s), PD15 (8 s) |

## API (`Core/Inc/software_timer.h`)

| Function | What it does |
|---|---|
| `Software_Timer_Init(&htim14)` | Starts TIM14 with its update interrupt |
| `Software_Timer_Set_Time(&t, ms)` | Stores the current tick and the interval, and activates the timer |
| `Software_Timer_Check_Elapsed_Time(&t)` | Returns `true` once when the interval has passed, then stops the timer |
| `Software_Timer_Get_Time()` | Milliseconds since start-up |
| `Software_Timer_Stop(&t)` | Deactivates the timer |

`HAL_TIM_PeriodElapsedCallback()` increments a `volatile` millisecond counter on every TIM14
interrupt. The elapsed time is `current - start` in unsigned arithmetic, so it stays correct when
the counter wraps around from 0xFFFFFFFF to 0.

A timer fires once. To make it periodic, call `Software_Timer_Set_Time()` again after it fires,
as the main loop does.
