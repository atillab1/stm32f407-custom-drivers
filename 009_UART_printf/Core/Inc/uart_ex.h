/*
 * uart_ex.h
 *
 *  Created on: Sep 29, 2026
 *      Author: atill
 */

#ifndef INC_UART_EX_H_
#define INC_UART_EX_H_

#include "main.h"
#include"circular_buffer.h"
#include"stdio.h"
#include"string.h"
#include"stdarg.h"


typedef struct
{
  UART_HandleTypeDef *huart;
  Circular_Buffer_t *cbIn;
  Circular_Buffer_t *cbOut;


}UART_Ex_t;

void UARTx_Initilalization(UART_Ex_t *uart,UART_HandleTypeDef *huart,Circular_Buffer_t *cbIn,Circular_Buffer_t *cbOut);
void UARTx_Write(UART_Ex_t *uart , char ch);
void UARTx_Put_String(UART_Ex_t *uart,char *str);
int UARTx_Printf(UART_Ex_t *uart ,const char *format, ...);

#endif /* INC_UART_EX_H_ */
