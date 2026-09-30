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
	// buffer[]'i sifirlamaya gerek yok: head'e yazilmadan hicbir kutu okunmaz
	circularBuffer->head = 0; // basini sifirla initiliazedasin
	circularBuffer->tail = 0; // ayni sekilde sonunu da sifirla initiliazdasin

}

//Buffer Durum Kontrolu bos mu dolu mu

bool Circular_Buffer_Is_Empty(const Circular_Buffer_t *circularBuffer)
{
	return circularBuffer->head == circularBuffer->tail;
}


bool Circular_Buffer_Is_Full(const Circular_Buffer_t *circularBuffer)
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

uint16_t Circular_Buffer_Count(const Circular_Buffer_t *circularBuffer)
{
	// head/tail'i bir kez oku: hesap sirasinda ISR degistirse bile tutarli iki deger kullanilir
	uint16_t head = circularBuffer->head;
	uint16_t tail = circularBuffer->tail;

	if(head >= tail)
		return head - tail;
	else
		return (CIRCULAR_BUFFER_SIZE - tail + head) ;
}
