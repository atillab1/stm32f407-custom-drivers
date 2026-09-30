/*
 * io_driver.c
 *
 *  Created on: May 1, 2026
 *      Author: atill
 */

#include"io_driver.h"

void IO_Initialization(IO_Info_t *ioInfo)
{
    /****************** Input Initialization ******************/
    ioInfo->inputsInfo.userButton.GPOIx		   = USER_BUTTON_GPIO_Port;
    ioInfo->inputsInfo.userButton.GPIO_Pin     = USER_BUTTON_Pin;
    ioInfo->inputsInfo.userButton.numOfInput   = 0;
    ioInfo->inputsInfo.userButton.currentState = GPIO_PIN_RESET;
    ioInfo->inputsInfo.userButton.lastState    = GPIO_PIN_RESET;
    ioInfo->inputsInfo.userButton.inputStatus  = Input_Status_Low;
    ioInfo->inputsInfo.userButton.currentTime  = 0;
    ioInfo->inputsInfo.userButton.debounceTime = DEBOUNCE_TIME;

    /****************** Output Initialization ******************/
    ioInfo->outputsInfo.ledBlue.GPIOx    = LED_BLUE_GPIO_Port;
    ioInfo->outputsInfo.ledBlue.GPIO_Pin = LED_BLUE_Pin;
    ioInfo->outputsInfo.ledBlue.pinState = GPIO_PIN_RESET;

    ioInfo->outputsInfo.ledRed.GPIOx    = LED_RED_GPIO_Port;
    ioInfo->outputsInfo.ledRed.GPIO_Pin = LED_RED_Pin;
    ioInfo->outputsInfo.ledRed.pinState = GPIO_PIN_RESET;

    ioInfo->outputsInfo.ledGreen.GPIOx    = LED_GREEN_GPIO_Port;
    ioInfo->outputsInfo.ledGreen.GPIO_Pin = LED_GREEN_Pin;
    ioInfo->outputsInfo.ledGreen.pinState = GPIO_PIN_RESET;

    ioInfo->outputsInfo.ledOrange.GPIOx    = LED_ORANGE_GPIO_Port;
    ioInfo->outputsInfo.ledOrange.GPIO_Pin = LED_ORANGE_Pin;
    ioInfo->outputsInfo.ledOrange.pinState = GPIO_PIN_RESET;
}

void IO_Status_Control(IO_Info_t *ioInfo)
{
	IO_Output_Control(&ioInfo->outputsInfo.ledBlue);
	IO_Output_Control(&ioInfo->outputsInfo.ledGreen);
	IO_Output_Control(&ioInfo->outputsInfo.ledOrange);
	IO_Output_Control(&ioInfo->outputsInfo.ledRed);

	IO_Input_Control_With_Debounce(&ioInfo->inputsInfo.userButton);
}

void IO_Input_Control_With_Debounce(Input_State_t *inputState)
{
    inputState->currentState = HAL_GPIO_ReadPin(inputState->GPOIx, inputState->GPIO_Pin);

    if(inputState->currentState == GPIO_PIN_SET)
    {
        if(inputState->inputStatus == Input_Status_Low)
        {
            if(inputState->currentState != inputState->lastState)
            {
                inputState->lastState = inputState->currentState;  // DOĞRUSU BU
                inputState->currentTime = HAL_GetTick();
            }
            if((HAL_GetTick() - inputState->currentTime) >= inputState->debounceTime)
            {
                inputState->inputStatus = Input_Status_High;
            }
        }
        // zaten high işaretledik
    }
    else
    {
        inputState->lastState = inputState->currentState;
        inputState->inputStatus = Input_Status_Low;
    }
}

void IO_Output_Control(Output_State_t *outputState)
{
	HAL_GPIO_WritePin(outputState->GPIOx, outputState->GPIO_Pin, outputState->pinState);
}


