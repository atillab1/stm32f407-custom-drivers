# 008 – PWM: RGB LED colour fade

TIM3 drives an RGB LED with three PWM channels. The main loop fades smoothly through a table of
colours.

## Wiring

| Colour | Channel | Pin |
|---|---|---|
| Red | TIM3_CH1 | PA6 |
| Green | TIM3_CH2 | PA7 |
| Blue | TIM3_CH3 | PB0 |

Use a series resistor (220–330 Ω) on each colour. For a common-cathode LED, connect the common pin
to GND.

## Configuration

| Setting | Value |
|---|---|
| Timer | TIM3 |
| Clock | Internal, 16 MHz |
| Prescaler | 15 (1 MHz tick) |
| Period (ARR) | 255, so a 0–255 colour value maps directly to the duty cycle |
| PWM frequency | 16 MHz / 16 / 256 ≈ 3.9 kHz (no visible flicker) |
| Mode | PWM mode 1, active high |

## How it works

- `Set_RGB_Color(r, g, b)` writes the three compare registers with `__HAL_TIM_SET_COMPARE()`.
- `Fade_To_Color(from, to, steps, step_ms)` moves each colour linearly from `from` to `to`:
  `value = from + (to - from) * i / steps`.
- The main loop walks through eight colours (red, orange, yellow, green, cyan, blue, purple, white):
  a 1 s fade to the next colour, then a 0.5 s hold.

## Notes

- With a **common-anode** LED the colours appear inverted. Write `255 - value` in `Set_RGB_Color()`
  or set the channel polarity to low in CubeMX.
- TIM4 is also configured for PWM on the on-board LEDs (PD12–PD15: 25 / 50 / 75 / ~100 %) but is
  not started.
