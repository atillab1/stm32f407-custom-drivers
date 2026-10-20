/*
 * software_timer.c
 *
 *  Created on: Oct 18, 2026
 *      Author: atill
 */



#include"software_timer.h"

volatile uint32_t msTick;   // kesmede artiyor, main okuyor: volatile olmali

// 1- Timer cevresel birimini sec ve baslat

void Software_Timer_Init(TIM_HandleTypeDef *htim)
{
	HAL_TIM_Base_Start_IT(htim);   // htim zaten pointer, basina & konmaz
}

//2- Timer set et ve baslat

void Software_Timer_Set_Time(STimer_t *sTimer,uint32_t intervalMs)
{
	sTimer->startTime 	  = Software_Timer_Get_Time();   // parantez: fonksiyonu cagir
	sTimer->intervalTime  = intervalMs;
	sTimer->activatedTime = true;
}

// 3- Zamani al

uint32_t Software_Timer_Get_Time(void)
{
	return msTick;
}

// 4- Gecen zamani kontro et

bool Software_Timer_Check_Elapsed_Time(STimer_t *sTimer)
{
	if(sTimer->activatedTime)
	{
		uint32_t currentTime = Software_Timer_Get_Time();

		// unsigned cikarma, msTick 0xFFFFFFFF'ten 0'a donse bile dogru farki verir
		uint32_t elapsedTime = currentTime - sTimer->startTime;

		if(elapsedTime >= sTimer->intervalTime)
		{
			Software_Timer_Stop(sTimer);
			return true;
		}
	}
	return false;
}

// 5- Zamanlamayi durdur

void Software_Timer_Stop(STimer_t *sTimer)
{
	sTimer->activatedTime = false;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM14)
	{
		 // 1ms 1 kesme
		msTick++;
	}

}
