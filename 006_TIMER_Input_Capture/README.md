# 006 – TIM2 input capture: button press duration

TIM2 captures the counter value on **both edges** of **PA0**. The capture callback stores the time
of the press and of the release, and computes how long the button was held.

## Configuration

| Setting | Value |
|---|---|
| Timer | TIM2 (32-bit) |
| Clock | Internal, 16 MHz |
| Prescaler | 15999 (1 ms tick) |
| Period (ARR) | 0xFFFFFFFF |
| Channel | CH1 on PA0, direct input, both edges, no filter, interrupt enabled |

## Try it

Watch `pressDuration` in **Live Expressions**. Hold the button for a moment and release it: the
value shows how many milliseconds it was held.

## How it works

`HAL_TIM_IC_CaptureCallback()` alternates between two states using `isPressed`:

1. First edge: store the captured value as `pressTime`.
2. Second edge: store it as `releaseTime` and compute `pressDuration`, handling counter overflow.

## Notes

- The callback checks `htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1`. Inside the callback HAL sets
  `Channel` to the *active channel* value, not to `TIM_CHANNEL_1`.
- The code assumes the first edge is a press. There is no input filter, so contact bounce can add
  an extra edge and swap press and release. An input filter (`ICFilter`) or checking the pin level
  in the callback would make the pairing robust.
