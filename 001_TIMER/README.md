# 001 – TIM2 external clock mode 1

TIM2 is clocked by its own input pin instead of the internal clock, so the counter counts
rising edges on **PA0** (the user button).

## Configuration

| Setting | Value |
|---|---|
| Timer | TIM2 |
| Clock source | External clock mode 1, trigger TI1FP1 (PA0 = TIM2_CH1), rising edge, no filter |
| Prescaler | 0 |
| Period (ARR) | 10, so the counter wraps after 11 edges |
| System clock | HSI 16 MHz |

## Try it

Start a debug session, add `counterValue1` to **Live Expressions** and press the blue button.
The value goes up by one per press and returns to 0 after 10.

`counterValue1` is read with `__HAL_TIM_GET_COUNTER()` and `counterValue2` directly from `TIM2->CNT`;
both show the same value.

## Notes

- The trigger filter is 0, so contact bounce can add extra counts for one press.
