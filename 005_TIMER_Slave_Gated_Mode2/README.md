# 005 – TIM2 slave gated mode

**PA0** acts as a gate: TIM2 counts only while the pin is high, which on the Discovery board means
while the user button is held down.

## Configuration

| Setting | Value |
|---|---|
| Timer | TIM2 |
| Clock | Internal, 16 MHz |
| Prescaler | 15999 (1 ms tick) |
| Period (ARR) | 9999, wraps after 10 s |
| Slave mode | Gated mode, trigger TI1FP1 (PA0 = TIM2_CH1), rising polarity (counts while high) |

## Try it

Watch `counterValue1` in **Live Expressions**.

- Hold the button: the value increases by one per millisecond.
- Release it: the value stops and **keeps** its value.
- Press again: counting continues from where it stopped.

The last point is the difference from [004](../004_TIMER_Slave_Mode_Reset_Mode/), where each press
resets the counter.

## Notes

- With `TIM_TRIGGERPOLARITY_FALLING` (CC1P = 1) the gate is inverted and the timer counts while
  the button is released.
