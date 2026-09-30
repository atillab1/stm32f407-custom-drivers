/*
 * circular_buffer.h
 *
 *  Created on: Sep 28, 2026
 *      Author: atill
 */

#ifndef INC_CIRCULAR_BUFFER_H_
#define INC_CIRCULAR_BUFFER_H_

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"


#define CIRCULAR_BUFFER_SIZE 512


typedef struct
{
	uint8_t buffer[CIRCULAR_BUFFER_SIZE];
	uint16_t head; // nereden itibaren verilerim tutulacak
	uint16_t tail;// verilerim nereye kadar tutulacak

}Circular_Buffer_t;


//Buffer Initiliaze
void Circular_Buffer_Init(Circular_Buffer_t *circularBuffer);

//Buffer Durum Kontrolu bos mu dolu mu

bool Circular_Buffer_Is_Empty(Circular_Buffer_t *circularBuffer);
bool Circular_Buffer_Is_Full(Circular_Buffer_t *circularBuffer);

//veri yazma ve okuma
bool Circular_Buffer_Enqueue(Circular_Buffer_t *circularBuffer,uint8_t data);
bool Circular_Buffer_Dequeue(Circular_Buffer_t *circularBuffer,uint8_t *data);

uint16_t Circular_Buffer_Count(Circular_Buffer_t *circularBuffer);

#endif /* INC_CIRCULAR_BUFFER_H_ */
