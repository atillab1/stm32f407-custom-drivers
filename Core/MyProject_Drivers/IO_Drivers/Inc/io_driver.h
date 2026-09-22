/*
 * io_driver.h
 *
 *  Created on: May 1, 2026
 *      Author: atill
 */

#ifndef INC_IO_DRIVER_H_
#define INC_IO_DRIVER_H_
#define DEBOUNCE_TIME		100

#include "main.h"

typedef enum
{
	Input_Status_Low=0,
	Input_Status_High

}Input_Status_t;

typedef struct
{
    GPIO_TypeDef      *GPOIx;        // Hangi GPIO portu (GPIOA, GPIOB vs.)
    uint16_t           GPIO_Pin;      // Hangi pin (GPIO_PIN_0, GPIO_PIN_13 vs.)
    uint16_t           numOfInput;    // Bu girişe verdiğin numara/ID

    GPIO_PinState      currentState;  // Şu anki okunan değer
    GPIO_PinState      lastState;     // Bir önceki döngüdeki değer

    Input_Status_t     inputStatus;   // Debounce sonrası kesinleşmiş durum

    uint32_t           currentTime;   // Son değişim anındaki zaman damgası
    uint32_t           debounceTime;  // Kaç ms bekleyeceğin (ör. 50ms)
} Input_State_t;


typedef struct
{
	Input_State_t userButton;

}Inputs_Info_t;


typedef struct
{
	GPIO_TypeDef		*GPIOx;
	uint16_t			GPIO_Pin;
	GPIO_PinState		pinState;
}Output_State_t;




typedef struct
{
	Output_State_t ledGreen;
	Output_State_t ledOrange;
	Output_State_t ledRed;
	Output_State_t ledBlue;

}Outputs_Info_t;

typedef struct
{
	Inputs_Info_t inputsInfo;
	Outputs_Info_t outputsInfo;

}IO_Info_t;

void IO_Initialization(IO_Info_t *ioInfo);
void IO_Status_Control(IO_Info_t *ioInfo);
void IO_Input_Control_With_Debounce(Input_State_t *inputState);
void IO_Output_Control(Output_State_t *outputState);




#endif /* INC_IO_DRIVER_H_ */
