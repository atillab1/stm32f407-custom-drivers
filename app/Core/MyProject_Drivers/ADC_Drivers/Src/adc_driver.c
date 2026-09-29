/*
 * adc_driver.c
 *
 *  Created on: May 5, 2026
 *      Author: atill
 */


#include "adc_driver.h"

DMA_Transfer_Status_t globalDmaTransferStatus = DMA_Transfer_Not_Yet_Start;

void ADC_Initialization(ADC_Info_t *adcInfo, ADC_HandleTypeDef *hadc)
{
    adcInfo->hadc = hadc;
    adcInfo->adcErrorStatus = ADC_No_Error;

    for(int i = 0; i < NUM_OF_ADC_CHANNEL; i++)
    {
        adcInfo->adcConvertedData[i] = 0;
        adcInfo->adcVoltageData[i]   = 0;
        adcInfo->adcRawsum[i]		=  0;
    }
    adcInfo->adcSampleCount = 0;

    adcInfo->dmaTransferStatus = globalDmaTransferStatus;

    if(HAL_ADC_Start_DMA(adcInfo->hadc, (uint32_t*)adcInfo->adcConvertedData, NUM_OF_ADC_CHANNEL) != HAL_OK)
    {
        adcInfo->adcErrorStatus = ADC_Init_Start_Error;
    }
}

void ADC_DMA_Conversion(ADC_Info_t *adcInfo)
{
    adcInfo->dmaTransferStatus = globalDmaTransferStatus;

    if(adcInfo->dmaTransferStatus == DMA_Transfer_Completed)
    {
        globalDmaTransferStatus = DMA_Transfer_Not_Completed;

        for(int i = 0; i < NUM_OF_ADC_CHANNEL; i++)
        {
            adcInfo->adcRawsum[i]+=adcInfo->adcConvertedData[i];
        }

        adcInfo->adcSampleCount += 1;

        if(adcInfo->adcSampleCount>=ADC_AVERAGE_COUNT)
        {
        	adcInfo->adcSampleCount = 0;

        	for(int i=0;i<NUM_OF_ADC_CHANNEL;i++)
        	{
        		adcInfo->adcAverageData[i] = adcInfo->adcRawsum[i]/ADC_AVERAGE_COUNT;


        		        	adcInfo->adcRawsum[i] = 0;
        	}
        			adcInfo->realVDDA=__LL_ADC_CALC_VREFANALOG_VOLTAGE(adcInfo->adcAverageData[ADC_Channel_Vref],
        	                  												DIGITAL_SCALE);
        	        adcInfo->temprature= __LL_ADC_CALC_TEMPERATURE(adcInfo->realVDDA,adcInfo->adcAverageData[ADC_Channel_Temp],DIGITAL_SCALE);

        	        for(int i=0;i<NUM_OF_ADC_CHANNEL;i++)
        	        {
        	        	adcInfo->adcVoltageData[i] = __LL_ADC_CALC_DATA_TO_VOLTAGE(adcInfo->realVDDA,
        	        																 adcInfo->adcAverageData[i],
																					 DIGITAL_SCALE)/1000.0f;
        	        }

        	        adcInfo->vBAT = adcInfo->adcVoltageData[4];

        	        adcInfo->potPercentage = MAP_Voltage_To_Percantage(adcInfo->adcVoltageData[ADC_Channel_2],
        	                                                            0.0f,
        	                                                            3.3f,
        	                                                            0,
        	                                                            100);
        }
    }
}

uint8_t MAP_Voltage_To_Percantage(float voltage, float inMin, float inMax, uint8_t outMin, uint8_t outMax)
{
uint8_t percentage =0;
percentage = (voltage-inMin)*(outMax-outMin)/(inMax-inMin)+(outMin);
return percentage;
}

/**
  * @brief  Regular conversion complete callback in non blocking mode
  * @param  hadc pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    // DMA Transfer Completed.
    globalDmaTransferStatus = DMA_Transfer_Completed;
}

/**
  * @brief  Regular conversion half DMA transfer callback in non blocking mode
  * @param  hadc pointer to a ADC_HandleTypeDef structure that contains
  *         the configuration information for the specified ADC.
  * @retval None
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    // DMA Transfer not yet completed.
    globalDmaTransferStatus = DMA_Transfer_Not_Completed;
}
