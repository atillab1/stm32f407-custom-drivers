# 004 – TIM2 slave reset mode

TIM2 counts milliseconds and every rising edge on **PA0** (pressing the user button) resets the
counter to 0. The counter therefore shows the time since the last press.

## Configuration

| Setting | Value |
|---|---|
| Timer | TIM2 |
| Clock | Internal, 16 MHz |
| Prescaler | 15999 (1 ms tick) |
| Period (ARR) | 9999, wraps after 10 s |
| Slave mode | Reset mode, trigger TI1FP1 (PA0 = TIM2_CH1), rising edge |
| Input filter | IC1F = 15 (about 16 µs), set in `USER CODE BEGIN TIM2_Init 2` |

## Try it

Watch `counterValue1` in **Live Expressions**. It climbs from 0 to 9999 in 10 s and drops back to
0 every time you press the button.

## Notes

- Reset mode does not start the counter; `HAL_TIM_Base_Start()` sets CEN.
- The digital filter rejects short noise spikes but not contact bounce, which lasts milliseconds.
  Bounce only restarts the count a few milliseconds late.
