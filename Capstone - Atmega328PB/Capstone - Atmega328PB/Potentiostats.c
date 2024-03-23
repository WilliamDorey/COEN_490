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
	uint8_t analogH; 
	uint8_t analogL;
	uint8_t opcode; 
	uint16_t temp = 0;
	
	opcode = 0x01;
	PORTD = !(1 << (2 + adc));
	
	SPI_Write(opcode);
	//opcode |= (channel << 4);
	opcode = 0b10000000;
	analogH = SPI_Transceiver(opcode);
	analogH = (analogH & 0b00000011);
	analogL = SPI_Transceiver(0);
	PORTD = 0xFF;
	
	temp = (analogH << 8) + analogL;
	
	return temp;
}