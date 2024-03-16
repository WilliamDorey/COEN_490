// Simple SPI interface commands
#pragma once

#define SS   PINB2
#define MOSI PINB3
#define MISO PINB4
#define SCK  PINB5

void SPI_Init()
{
	DDRB |= (1 << SS)  | (1 << MOSI) | (1 << SCK);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);// | (1 << CPHA);
	SPSR = (0 << SPI2X);
}

void SPI_Write(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
}

uint8_t SPI_Read()
{
	SPDR = 0xFF;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}
