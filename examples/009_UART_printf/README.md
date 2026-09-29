# 009 – Interrupt-driven UART with circular buffers

USART3 sends and receives through interrupts. Received bytes go into one circular buffer and
bytes to send wait in another, so the main loop never blocks on the UART. The project uses the
shared [`uart`](../../drivers/uart/) and [`circular_buffer`](../../drivers/circular_buffer/)
drivers through the linked `UserDrivers` folder.

## Wiring

Use a 3.3 V USB-to-UART adapter:

| Adapter | Board |
|---|---|
| RX | **PB10** (USART3_TX) |
| TX | **PB11** (USART3_RX) |
| GND | GND |

Open a serial terminal (PuTTY, Tera Term, CubeIDE terminal) at **115200 baud, 8N1**.

## Configuration

| Setting | Value |
|---|---|
| Peripheral | USART3, 115200 baud, 8 data bits, no parity, 1 stop bit |
| Interrupt | USART3_IRQn, RXNE always enabled, TXE enabled only while there is data to send |
| Buffers | `uartCbIn` and `uartCbOut`, 512 bytes each |
| System clock | HSI 16 MHz |

## How it works

- `UARTx_Initilalization()` binds `huart3` and the two buffers and enables the RX interrupt.
- `USART3_IRQHandler()` in `Core/Src/stm32f4xx_it.c`:
  - **RXNE** → read `DR`, enqueue the byte into `uartCbIn`.
  - **TXE**, only if the TXE interrupt is enabled → send the next byte from `uartCbOut`, or
    disable the TXE interrupt when the buffer is empty.
- `UARTx_Write()`, `UARTx_Put_String()` and `UARTx_Printf()` queue bytes into `uartCbOut` and
  start the transmitter if it is idle.

## Try it

The main loop is still empty. To see both directions working, print a banner and echo every
received byte back:

```c
  /* USER CODE BEGIN 2 */
  UARTx_Initilalization(&uart3, &huart3, &uartCbIn, &uartCbOut);
  UARTx_Printf(&uart3, "009_UART_printf ready\r\n");
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint8_t ch;
    if (Circular_Buffer_Dequeue(&uartCbIn, &ch))
    {
      UARTx_Write(&uart3, (char)ch);   /* echo */
    }
  }
  /* USER CODE END 3 */
```

## Notes

- The TX branch of the ISR checks `__HAL_UART_GET_IT_SOURCE(..., UART_IT_TXE)` as well as the TXE
  flag. TXE is set whenever `DR` is empty, so without that check an interrupt entered for a received
  byte would also run the TX code while `UARTx_Write()` is using the same buffer.
- `HAL_UART_IRQHandler()` still runs after the custom code. It does no work here because no HAL
  receive or transmit is active, but on an overrun it disables the RXNE interrupt. If reception
  ever stops, handle the error flags yourself and return before calling it.
- Characters are dropped when `uartCbOut` is full; `UARTx_Printf()` formats at most 255 characters.
