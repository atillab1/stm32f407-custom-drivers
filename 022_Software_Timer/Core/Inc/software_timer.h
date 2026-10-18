/*
 * software_timer.h
 *
 *  Created on: Oct 18, 2026
 *      Author: atill
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "main.h"
#include "stdbool.h"

typedef struct
{
	uint32_t startTime;
	uint32_t intervalTime;
	bool activatedTime;

}STimer_t;

// 1- Timer cevresel birimini sec ve baslat

void Software_Timer_Init(TIM_HandleTypeDef *htim);

//2- Timer set et ve baslat

void Software_Timer_Set_Time(STimer_t *sTimer,uint32_t intervalMs);

// 3- Zamani al

uint32_t Software_Timer_Get_Time(void);

// 4- Gecen zamani kontro et

bool Software_Timer_Check_Elapsed_Time(STimer_t *sTimer);

// 5- Zamanlamayi durdur
void Software_Timer_Stop(STimer_t *sTimer);

#endif /* INC_SOFTWARE_TIMER_H_ */
