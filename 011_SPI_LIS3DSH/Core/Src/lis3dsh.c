/*
 * lis3dsh.c
 *
 *  Created on: Oct 12, 2026
 *      Author: atill
 */


#include "lis3dsh.h"



void LIS3DSH_CS_ENABLE(LIS3DSH_t *lis3dsh)
{
	HAL_GPIO_WritePin(lis3dsh->cs_port, lis3dsh->cs_pin, GPIO_PIN_RESET);
}
void LIS3DSH_CS_DISABLE(LIS3DSH_t *lis3dsh)
{
	HAL_GPIO_WritePin(lis3dsh->cs_port, lis3dsh->cs_pin, GPIO_PIN_SET);
}





bool LIS3DSH_Initialization(LIS3DSH_t *lis3dsh,SPI_HandleTypeDef *hspi,GPIO_TypeDef *csport,uint16_t cs_pin ){

	lis3dsh->hspi		= hspi;
	lis3dsh->cs_port	= csport;
	lis3dsh->cs_pin		= cs_pin;

	lis3dsh->x_raw		= 0;
	lis3dsh->y_raw		= 0;
	lis3dsh->z_raw		= 0;

	if(LIS3DSH_Who_I_Am(lis3dsh)== LIS3DSH_WHO_I_AM_VALUE)
	{
		// sensor bulundu
		lis3dsh->found = true;

		//Config
		// CTRL_REG4 X Y Z AKTIF OUTPUT DATARATE 800 Hz
		if(!LIS3DSH_Write_Register(lis3dsh, LIS3DSH_CTRL_REG4, 0x87))
			return false;
		// CTRL_REG5  +_2G  full scale , anti-alias filter 800 Hz
		if(!LIS3DSH_Write_Register(lis3dsh, LIS3DSH_CTRL_REG5, 0x00))
					return false;

	}
	else
	{
		lis3dsh->found = false;
	}
	return true;
}
uint8_t LIS3DSH_Who_I_Am(LIS3DSH_t *lis3dsh)
{
	return LIS3DSH_Read_Register(lis3dsh, LIS3DSH_WHO_I_AM_ADDR);
}

bool LIS3DSH_Read_XYZ(LIS3DSH_t *lis3dsh)
{
	return false;
}

uint8_t LIS3DSH_Read_Register(LIS3DSH_t *lis3dsh,uint8_t regAddr){
	//MSB = 1 olacak sekilde okuma islemi icin
	uint8_t txData = regAddr | 0x80;
	uint8_t rxData = 0;

	LIS3DSH_CS_ENABLE(lis3dsh);

	// yontem 1
	if(HAL_SPI_Transmit(lis3dsh->hspi, &txData, 1, 100)== HAL_OK)
	{
		if(HAL_SPI_Receive(lis3dsh->hspi, &rxData, 1, 100)== HAL_OK)
		{
		// diagnostic mesajlar ya da farkli mesajlar
		}
	}
	LIS3DSH_CS_DISABLE(lis3dsh);
	return rxData;

}
bool LIS3DSH_Write_Register(LIS3DSH_t *lis3dsh,uint8_t regAddr,uint8_t data)
{

	uint8_t txData[2] = {0};

	txData[0] = regAddr & 0x7F;  // adres bilgisi
	txData[1] = data; // veri bilgisi

	LIS3DSH_CS_ENABLE(lis3dsh);
	HAL_StatusTypeDef status = HAL_SPI_Transmit(lis3dsh->hspi, txData, 2, 100);
	LIS3DSH_CS_DISABLE(lis3dsh);

	return (status == HAL_OK);
}









