/*
 * uart_ex.c
 *
 *  Created on: Sep 29, 2026
 *      Author: atill
 */

#include"uart_ex.h"



void UARTx_Initilalization(UART_Ex_t *uart,UART_HandleTypeDef *huart,Circular_Buffer_t *cbIn,Circular_Buffer_t *cbOut)
{
	uart->huart = huart;
	uart->cbIn = cbIn;
	uart->cbOut = cbOut;

	Circular_Buffer_Init(cbIn);
	Circular_Buffer_Init(cbOut);

	__HAL_UART_ENABLE_IT(uart->huart ,UART_IT_RXNE); //RX Interrupt enabled ...
	__HAL_UART_ENABLE_IT(uart->huart ,UART_IT_TXE); //TX Interrupt enabled ...

}


void UARTx_Write(UART_Ex_t *uart , char ch)
{
	if(Circular_Buffer_Enqueue(uart->cbOut, ch)){
		if(!(uart->huart->Instance->CR1 & USART_CR1_TXEIE)) // bit bit karsilastirma icin "&"
		{
			uint8_t ch;
			if(Circular_Buffer_Dequeue(uart->cbOut, &ch))
			{
				uart->huart->Instance->DR = ch;
				__HAL_UART_ENABLE_IT(uart->huart,UART_IT_TXE);
			}
		}
	}
}
void UARTx_Put_String(UART_Ex_t *uart,char *str){

	while(*str)
	{
		UARTx_Write(uart, *str);
		str++;
	}


}


int UARTx_Printf(UART_Ex_t *uart ,const char *format, ...)
{
 char tx_buffer[256];
 va_list args; // degisken yapidaki veri boyutlarini tutan yapi

 va_start(args,format);
int length = vsnprintf(tx_buffer,sizeof(tx_buffer),format,args);
 va_end(args);
 __HAL_UART_ENABLE_IT(uart->huart ,UART_IT_RXNE);
 UARTx_Put_String(uart, tx_buffer);

 return length;
}
