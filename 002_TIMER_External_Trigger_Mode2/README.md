# 002 – TIM2 external clock mode 2 (ETR)

TIM2 counts pulses on its external trigger input **ETR (PA0)**. The main loop measures how many
pulses arrive per second and turns that into a frequency.

The signal comes from the MCU itself: `HAL_RCC_MCOConfig()` outputs **HSI / 2 = 8 MHz** on
**MCO1 (PA8)**, so the project measures the chip's own internal oscillator.

## Wiring

Connect **PA8** to **PA0** with a jumper wire.

## Configuration

| Setting | Value |
|---|---|
| Timer | TIM2 (32-bit) |
| Clock source | External clock mode 2, ETR on PA0, non-inverted, ETR prescaler DIV1, no filter |
| Prescaler | 0 |
| Period (ARR) | 0xFFFFFFFF (free-running) |
| Signal | MCO1 on PA8 = HSI / 2 |
| System clock | HSI 16 MHz |

## How the measurement works

`Measure_ETR_Frequency()` reads the counter and `HAL_GetTick()`, waits for a 1 s window and divides
the pulse count by the time that actually passed. Unsigned subtraction keeps both the tick and the
counter correct across overflow. `realfrequencyHz` multiplies the result by 2 to get back the HSI
frequency, because MCO1 divides it by 2.

## Try it

Watch `frequencyHz`, `frequencyMHZ` and `realfrequencyHz` in **Live Expressions**.

## Known limitation

The reference manual (RM0090, `TIMx_SMCR.ETPS`) limits the ETR signal to **1/4 of the timer clock**.
With TIM2 at 16 MHz that is 4 MHz, but MCO1 outputs 8 MHz, so pulses can be missed. To stay within
the limit, set the ETR prescaler to DIV4 (`sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV4`)
and multiply the measured value by 4.
