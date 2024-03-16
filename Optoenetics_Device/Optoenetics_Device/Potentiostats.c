#pragma once

#include "SPI.c"
#include "TWI.c"
#include "IOX.c"

#define LMP_WRITE_ADDR 0x90
#define LMP_READ_ADDR  0x91

#define ADC_SEL_0 PORTD2

void Read_LMP_Status(uint8_t ID, unsigned char *data)
{
	CS_low(ID);
	_delay_ms(1);

	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x00);
	TWI_stop();
	_delay_ms(1);
	
	do{
		TWI_start();
		TWI_read_address(LMP_READ_ADDR);
		TWI_read_data(data);
		TWI_stop();
		_delay_ms(1);
	} while (*data != 0x01);
	
	CS_high(ID);
}
void Prog_LMP(uint8_t ID, uint8_t reg, uint8_t val)
{
	CS_low(ID);
	_delay_ms(1);

	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(reg);
	TWI_write_data(val);
	TWI_stop();
	_delay_ms(1);
	
	CS_high(ID);
}

void Set_LMP_Unlock(uint8_t ID)
{
	CS_low(ID);
	_delay_ms(1);

	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x01);
	TWI_write_data(0x00);
	TWI_stop();
	_delay_ms(1);
		
	CS_high(ID);
}

void Set_LMP_Standby(uint8_t ID)
{
	uint8_t data = 0;
	
	CS_low(ID);
	_delay_ms(1);

	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x12);
	TWI_stop();
	_delay_ms(1);
	
	TWI_start();
	TWI_read_address(LMP_READ_ADDR);
	TWI_read_data(&data);
	TWI_stop();
	_delay_ms(1);
	
	data = (data &0xF8) + 0x2;
	
	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x12);
	TWI_write_data(data);
	TWI_stop();
	_delay_ms(1);
		
	CS_high(ID);
}

void Set_LMP_Amperic(uint8_t ID)
{
	uint8_t data = 0;
	
	CS_low(ID);
	_delay_ms(1);

	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x12);
	TWI_stop();
	_delay_ms(1);
	
	TWI_start();
	TWI_read_address(LMP_READ_ADDR);
	TWI_read_data(&data);
	TWI_stop();
	_delay_ms(1);
	
	data = (data &0xF8) + 0x3;
	
	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x12);
	TWI_write_data(data);
	TWI_stop();
	_delay_ms(1);
	
	CS_high(ID);
}

void Set_LMP_FET(uint8_t ID, uint8_t val)
{
	uint8_t data = 0;
	
	CS_low(ID);
	_delay_ms(1);

	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x12);
	TWI_stop();
	_delay_ms(1);
	
	TWI_start();
	TWI_read_address(LMP_READ_ADDR);
	TWI_read_data(&data);
	TWI_stop();
	_delay_ms(1);
	
	data = (data &0xF7F) + (val ? 0x80 : 0x00);
	
	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x12);
	TWI_write_data(data);
	TWI_stop();
	_delay_ms(1);
		
	CS_high(ID);

}
	
void Set_LMP_gain(uint8_t ID, uint8_t val)
{
	
}

void Set_LMP_RLoad(uint8_t ID, uint8_t val)
{
	
}

void Set_LMP_ExtRef(uint8_t ID, uint8_t val)
{
	
}

void Set_LMP_IntZ(uint8_t ID, uint8_t val)
{
	
}

void Set_LMP_REFCN(uint8_t ID, uint8_t val)
{
	
}

void Set_LMP_Temperature(uint8_t ID)
{
	CS_low(ID);
	_delay_ms(1);

	TWI_start();
	TWI_write_address(LMP_WRITE_ADDR);
	TWI_write_data(0x12);
	TWI_write_data(0x06);
	TWI_stop();
	_delay_ms(1);
		
	CS_high(ID);

}

uint16_t Read_ADC(uint8_t adc, uint8_t channel)
{
	uint8_t hi = 0;
	uint8_t lo = 0;
	//uint8_t adc_sel = 0x04 << adc;
	
	//PORTD = 0xFF ^ adc_sel;
	SPI_Write(0x01);
	SPI_Write(0x40);// + (channel << 4));
	
	hi = SPDR;
	lo = SPI_Read();
	
	//PORTD = 0xFF;
	
	return ( (hi & 0x03) << 8) + lo;
}