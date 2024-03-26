#pragma once

#include "UART.c"
#include "SPI.c"
#include "TWI.c"
#include "IOX.c"

#define LED_COUNT 50

void LED_all_off(uint8_t LED[])
{
	for (int i = 0; i < LED_COUNT; i++)
	LED[i] = 0b000;
	
	for (uint8_t CS = 0; CS < 75; CS++)
	{
		CS_low(CS);
		
		SPI_Write(0x40);
		SPI_Write(0x88);

		CS_high(CS);
		_delay_ms(1);
	}
}

void LED_all_on(uint8_t LED[], char colour)
{
	uint8_t offset = 0;
	uint8_t mask = 0;
	switch(colour)
	{
	case 'R':
		offset = 0;
		mask = 0b100;
		break;
	case 'G':
		offset = 25;
		mask = 0b010;
		break;
	case 'B':
		offset = 50;
		mask = 0b001;
		break;
	}
	
	for (int i = 0; i < LED_COUNT; i++)
		LED[i] |= mask;
	
	switch(colour)
	{
	case 'R':
		offset = 0;
		break;
	case 'G':
		offset = 25;
		break;
	case 'B':
		offset = 50;
		break;	
	}
	
	for (uint8_t CS = offset; CS < 25 + offset; CS++)
	{
		CS_low(CS);
		
		SPI_Write(0x40);
		SPI_Write(0x88);

		CS_high(CS);
		_delay_ms(1);
	}
}

void ToggleLED(uint8_t LED[], uint8_t ID, char colour)
{
	uint8_t SPI_command = 0;
	uint8_t CS = ID / 2;
	
	if(ID > 49)
	{
		//UART_SendString("\n\rERR: NOT A VALID LED ID!");
		return;
	}
	
	switch (colour)
	{
		default:
		//UART_SendString("\n\rERR: NOT A VALID LED COLOUR!");
		return;
		
		case 'R':
		//UART_SendString("\n\rRED");
		CS = CS;
		LED[ID] = (LED[ID] & 0b011) | (~LED[ID] & 0b100);

		//Turn on P0B or P1B of potentiometer
		if (ID % 2){
			SPI_command |= (LED[ID]   & 0b100) ? 0xF0 : 0x00;
			SPI_command |= (LED[ID-1] & 0b100) ? 0x0F : 0x00;
		}
		else{
			SPI_command |= (LED[ID+1] & 0b100) ? 0xF0 : 0x00;
			SPI_command |= (LED[ID]   & 0b100) ? 0x0F : 0x00;
		}
		break;
		
		case 'G':
		//UART_SendString("\n\rGREEN");
		CS = CS + 25;
		LED[ID] = (LED[ID] & 0b101) | (~LED[ID] & 0b010);
		
		//Turn on P0B or P1B of potentiometer
		if (ID % 2){
			SPI_command |= (LED[ID]   & 0b010) ? 0xF0 : 0x00;
			SPI_command |= (LED[ID-1] & 0b010) ? 0x0F : 0x00;
		}
		else{
			SPI_command |= (LED[ID+1] & 0b010) ? 0xF0 : 0x00;
			SPI_command |= (LED[ID]   & 0b010) ? 0x0F : 0x00;
		}
		break;
		
		case 'B':
		//UART_SendString("\n\rBLUE");
		CS = CS + 50;
		LED[ID] = (LED[ID] & 0b110) | (~LED[ID] & 0b001);
		
		//Turn on P0B or P1B of potentiometer
		if (ID % 2){
			SPI_command |= (LED[ID]   & 0b001) ? 0xF0 : 0x00;
			SPI_command |= (LED[ID-1] & 0b001) ? 0x0F : 0x00;
		}
		else{
			SPI_command |= (LED[ID+1] & 0b001) ? 0xF0 : 0x00;
			SPI_command |= (LED[ID]   & 0b001) ? 0x0F : 0x00;
		}
		break;
	}
	
	//SET CS LOW
	CS_low(CS);

	//PROGRAM POTENTIOMETERS
	SPI_Write(0x40);
	SPI_Write(SPI_command);

	//SET CS HIGH
	CS_high(CS);
	//UART_SendString("\n\rLED updated!\r\n");
}

void SetLEDBrightness(uint8_t ID, char colour, uint8_t level)
{
	uint8_t CS = ID / 2;
	
	switch (colour)
	{
		default:
		//UART_SendString("\n\rERR: NOT A VALID LED COLOUR!");
		return;
		
		case 'R':
		//UART_SendString("\n\rRED");
		CS = CS;
		break;
		
		case 'G':
		//UART_SendString("\n\rGREEN");
		CS = CS + 25;
		break;
		
		case 'B':
		//UART_SendString("\n\rBLUE");
		CS = CS + 50;
		break;
	}
	
	uint8_t potentiometer = (ID % 2) ? 0x10 : 0x00;
	
	//SET CS LOW
	CS_low(CS);

	//PROGRAM POTENTIOMETERS
	SPI_Write(potentiometer);
	SPI_Write(level);

	//SET CS HIGH
	CS_high(CS);
}
