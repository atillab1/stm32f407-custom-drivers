/*
 * circular_buffer.h
 *
 *  Created on: Sep 28, 2026
 *      Author: atill
 */

#ifndef INC_CIRCULAR_BUFFER_H_
#define INC_CIRCULAR_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>


#define CIRCULAR_BUFFER_SIZE 512   // 2'nin kuvveti: % islemi tek bir AND'e donusur

/*
 * Tek yazan + tek okuyan (ornegin UART ISR'i ve main) icin tasarlandi:
 * head'i sadece yazan, tail'i sadece okuyan degistirir. Iki taraf da
 * ayni degiskenlere eristigi icin alanlar volatile.
 * Kapasite CIRCULAR_BUFFER_SIZE - 1 eleman (bos/dolu ayrimi icin bir yer bos kalir).
 */
typedef struct
{
	volatile uint8_t buffer[CIRCULAR_BUFFER_SIZE];
	volatile uint16_t head; // bir sonraki verinin YAZILACAGI yer
	volatile uint16_t tail; // bir sonraki verinin OKUNACAGI yer

}Circular_Buffer_t;


//Buffer Initiliaze
void Circular_Buffer_Init(Circular_Buffer_t *circularBuffer);

//Buffer Durum Kontrolu bos mu dolu mu

bool Circular_Buffer_Is_Empty(const Circular_Buffer_t *circularBuffer);
bool Circular_Buffer_Is_Full(const Circular_Buffer_t *circularBuffer);

//veri yazma ve okuma
bool Circular_Buffer_Enqueue(Circular_Buffer_t *circularBuffer,uint8_t data);
bool Circular_Buffer_Dequeue(Circular_Buffer_t *circularBuffer,uint8_t *data);

uint16_t Circular_Buffer_Count(const Circular_Buffer_t *circularBuffer);

#endif /* INC_CIRCULAR_BUFFER_H_ */
