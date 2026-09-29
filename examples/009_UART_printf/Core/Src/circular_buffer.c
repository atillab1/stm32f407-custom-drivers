/*
 * circular_buffer.c
 *
 *  Created on: Sep 28, 2026
 *      Author: atill
 */


#include "circular_buffer.h"


//Buffer Initiliaze
void Circular_Buffer_Init(Circular_Buffer_t *circularBuffer)
{
	memset(circularBuffer->buffer,0, CIRCULAR_BUFFER_SIZE);
	circularBuffer->head = 0; // basini sifirla initiliazedasin
	circularBuffer->tail = 0; // ayni sekilde sonunu da sifirla initiliazdasin

}

//Buffer Durum Kontrolu bos mu dolu mu

bool Circular_Buffer_Is_Empty(Circular_Buffer_t *circularBuffer)
{
	return (circularBuffer->head == circularBuffer->tail)? true : false;
}


bool Circular_Buffer_Is_Full(Circular_Buffer_t *circularBuffer)
{
 int div = circularBuffer->head - circularBuffer->tail;

 if(div <0)
 {
	 div = div + CIRCULAR_BUFFER_SIZE;

 }
 return (div == (CIRCULAR_BUFFER_SIZE - 1)) ? true : false;
}
//veri yazma ve okuma
bool Circular_Buffer_Enqueue(Circular_Buffer_t *circularBuffer,uint8_t data)
{
	if(Circular_Buffer_Is_Full(circularBuffer))
	{
		return false;
	}
	circularBuffer->buffer[circularBuffer->head] = data;
	circularBuffer->head = (circularBuffer->head + 1) % CIRCULAR_BUFFER_SIZE;

	return true;
}
bool Circular_Buffer_Dequeue(Circular_Buffer_t *circularBuffer,uint8_t *data)
{
if(Circular_Buffer_Is_Empty(circularBuffer))
{
	return false;
}
 *data = circularBuffer->buffer[circularBuffer->tail];
 circularBuffer->tail = (circularBuffer->tail + 1) % CIRCULAR_BUFFER_SIZE;

 return true;
}

uint16_t Circular_Buffer_Count(Circular_Buffer_t *circularBuffer)
{
	if(circularBuffer->head >= circularBuffer->tail)
		return circularBuffer->head - circularBuffer->tail;
	else
		return (CIRCULAR_BUFFER_SIZE - circularBuffer->tail + circularBuffer->head) ;
}
