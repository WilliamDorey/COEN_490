#pragma once

#include "TWI.c"

#define IOX_0_WRTIE 0x40
#define IOX_0_READ  0x41

void Init_IOX()
{
	for(uint8_t i = 0; i < 5; i ++)
	{
		TWI_start();
		TWI_write_address(IOX_0_WRTIE);
		TWI_write_data(0x18 + i);
		TWI_write_data(0x00);
		TWI_stop();
		_delay_ms(1);
	}
	
	for(uint8_t i = 0; i < 5; i ++)
	{
		TWI_start();
		TWI_write_address(IOX_0_WRTIE);
		TWI_write_data(0x08 + i);
		TWI_write_data(0xFF);
		TWI_stop();
		_delay_ms(1);
	}
}

void CS_low(uint8_t CS)
{
	TWI_start();
	TWI_write_address(IOX_0_WRTIE);
	TWI_write_data(0x08 + (CS >> 3) );
	TWI_write_data( ~(1 << (CS % 0x08) ) );
	TWI_stop();
}

void CS_high(uint8_t CS)
{
	TWI_start();
	TWI_write_address(IOX_0_WRTIE);
	TWI_write_data(0x08 + (CS >> 3));
	TWI_write_data(0xFF);
	TWI_stop();
}
