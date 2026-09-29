# 007 – TIM4 output compare: phase-shifted LEDs

TIM4 toggles the four on-board LEDs in hardware. Each channel has a different compare value, so the
LEDs change state 250 ms apart. The CPU does nothing after start-up.

## Configuration

| Setting | Value |
|---|---|
| Timer | TIM4 |
| Clock | Internal, 16 MHz |
| Prescaler | 15999 (1 kHz tick) |
| Period (ARR) | 999, one period = 1 s |
| Output mode | Output compare, toggle on match |

| Channel | Pin | LED | Compare value | Toggles at |
|---|---|---|---|---|
| CH1 | PD12 | Green | 249 | 250 ms |
| CH2 | PD13 | Orange | 499 | 500 ms |
| CH3 | PD14 | Red | 749 | 750 ms |
| CH4 | PD15 | Blue | 999 | 1000 ms |

## Try it

Flash the board: each LED is on for 1 s and off for 1 s, and the four LEDs follow each other
250 ms apart.

## Notes

- In toggle mode the compare value sets **when** the pin changes within the period, not the
  brightness. For brightness control see [008_PWM](../008_PWM/).
- The project also contains the TIM2 input capture setup from [006](../006_TIMER_Input_Capture/);
  it is not started.
