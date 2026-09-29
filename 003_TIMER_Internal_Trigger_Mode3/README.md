# 003 – Timer chaining: TIM2 triggers TIM1 (slave trigger mode)

TIM2 acts as the master: its update event is routed to TIM1 through the internal trigger line
**ITR1**. TIM1 runs in slave trigger mode, so the hardware starts it on the first trigger without
any code in between.

## Configuration

| Setting | TIM2 (master) | TIM1 (slave) |
|---|---|---|
| Clock | Internal, 16 MHz | Internal, 16 MHz |
| Prescaler | 15999 (1 kHz tick) | 0 |
| Period (ARR) | 9999, update every **10 s** | 65535 |
| Trigger output (TRGO) | Update event | – |
| Slave mode | – | Trigger mode, input ITR1 (= TIM2 on STM32F407) |

## Try it

Watch `counterValue1` (TIM1) and `counterValue2` (TIM2) in **Live Expressions**.

- For the first ~10 s TIM2 counts up in milliseconds and TIM1 stays at **0**.
- When TIM2 overflows for the first time, TIM1 starts counting at 16 MHz.

## Notes

- Trigger mode only **starts** the slave. Later TIM2 updates have no effect because TIM1 is already
  running. To count TIM2 overflows instead, use external clock mode 1 with ITR1 as the clock source.
- TIM1 is started before TIM2, so it is armed before the first trigger arrives.
- `MX_TIM2_Init()` must run before `MX_TIM1_Init()`. Initialising a timer generates an update event,
  and a TIM2 update after TIM1 is configured would start TIM1 immediately.
