#pragma once

#include "TWI.c"
#include <stdlib.h>

//U5 of the main board
#define IOX_0_WRITE 0x40
#define IOX_0_READ  0x41

//U4 of the main board
#define IOX_1_WRITE 0x42
#define IOX_1_READ  0x43

#define RANGE 18

const uint8_t cs_ranges[RANGE][2] = {
	{28, 31},		//IO0 of U4 for blue LEDs
	{34, 37},		//IO0 of U4 for blue LEDs
	{63, 70},		//IO0 of U5 for green LEDs
	{32, 33},		//IO1 of U4 for blue LEDs
	{13, 18},		//IO1 of U4 for red LEDs
	{50, 53},		//IO1 of U5 for green LEDs
	{71, 74},		//IO1 of U5 for green LEDs
	{0, 1},			//IO2 of U4 for red LEDs
	{19, 24},		//IO2 of U4 for red LEDs
	{54, 56},		//IO2 of U5 for green LEDs
	{58, 62},		//IO2 of U5 for green LEDs
	{2, 6},			//IO3 of U4 for red LEDs
	{10, 12},		//IO3 of U4 for red LEDs
	{38, 44},		//IO3 of U5 for blue LEDs
	{57, 57},		//IO3 of U5 for green LEDs
	{7, 9},			//IO4 of U4 for red LEDs
	{25, 27},		//IO4 of U5 for blue LEDs
	{45, 49},		//IO4 of U5 for blue LEDs
};

const uint8_t cs_values[RANGE] = {
	0, 0, 0,
	1, 1, 1, 1,
	2, 2, 2, 2,
	3, 3, 3, 3,
	4, 4, 4,
};

uint8_t lookup_cs_values (uint8_t CS){
	for(uint8_t i = 0; i < RANGE; i++){
		if (CS >= cs_ranges[i][0] && CS <= cs_ranges[i][1]){
			return cs_values[i];
		}
	}
	return 0;
}

const struct {
	uint8_t cs_value;
	uint8_t hex_value;
}

cs_hex_lookup[] = {
	{0, 0xBF},		{1, 0x7F},		{2, 0xFE},		{3, 0xFD},		{4, 0xFB},
	{5, 0xF7},		{6, 0xEF},		{7, 0xFB},		{8, 0xFD},		{9, 0xFE},
	{10, 0x7F},		{11, 0xBF},		{12, 0xDF},		{13, 0xFB},		{14, 0xF7},
	{15, 0xEF},		{16, 0xDF},		{17, 0xBF},		{18, 0x7F},		{19, 0xFE},
	{20, 0xFD},		{21, 0xFB},		{22, 0xF7},		{23, 0xEF},		{24, 0xDF},
	{25, 0xDF},		{26, 0xBF},		{27, 0x7F},		{28, 0xFE},		{29, 0xFD},
	{30, 0xFB},		{31, 0xF7},		{32, 0xFD},		{33, 0xFE},		{34, 0x7F},
	{35, 0xBF},		{36, 0xDF},		{37, 0xEF},		{38, 0xFD},		{39, 0xFB},
	{40, 0xF7},		{41, 0xEF},		{42, 0xDF},		{43, 0xBF},		{44, 0x7F},
	{45, 0xFE},		{46, 0xFD},		{47, 0xFB},		{48, 0xF7},		{49, 0xEF},
	{50, 0xEF},		{51, 0xDF},		{52, 0xBF},		{53, 0x7F},		{54, 0xFE},
	{55, 0xFD},		{56, 0xFB},		{57, 0xFE},		{58, 0x7F},		{59, 0xBF},
	{60, 0xDF},		{61, 0xEF},		{62, 0xF7},		{63, 0xFE},		{64, 0xFD},
	{65, 0xFB},		{66, 0xF7},		{67, 0xEF},		{68, 0xDF},		{69, 0xBF},
	{70, 0x7F},		{71, 0xFE},		{72, 0xFD},		{73, 0xFB},		{74, 0xF7},
};

uint8_t lookup_hex_values(uint8_t CS){
	for (uint8_t i = 0; i < sizeof(cs_hex_lookup) / sizeof(cs_hex_lookup[0]); i++){
		if (cs_hex_lookup[i].cs_value == CS) {
			return cs_hex_lookup[i].hex_value;
		}
	}
	return 0;
}

void Init_IOX()
{
	for(uint8_t i = 0; i < 5; i ++)
	{
		TWI_start();
		TWI_write_address(IOX_1_WRITE);
		TWI_write_data(0x18 + i);
		TWI_write_data(0x00);			 // I/O[0:4] Configured as Output
		TWI_stop();
		_delay_ms(1);
	}
	
	for(uint8_t i = 0; i < 5; i ++)
	{
		TWI_start();
		TWI_write_address(IOX_1_WRITE);
		TWI_write_data(0x08 + i);
		TWI_write_data(0xFF);			// Output port[0:4] defined as output
		TWI_stop();
		_delay_ms(1);
	}

	for(uint8_t i = 0; i < 5; i ++)
	{
		TWI_start();
		TWI_write_address(IOX_0_WRITE);
		TWI_write_data(0x18 + i);
		TWI_write_data(0x00);
		TWI_stop();
		_delay_ms(1);
	}
	
	for(uint8_t i = 0; i < 5; i ++)
	{
		TWI_start();
		TWI_write_address(IOX_0_WRITE);
		TWI_write_data(0x08 + i);
		TWI_write_data(0xFF);
		TWI_stop();
		_delay_ms(1);
	}
}

void CS_low(uint8_t CS)
{
	uint8_t val;
	uint8_t hex_val;
	
	//Associate values to corresponding CS
	val = lookup_cs_values(CS);				//which port to write to
	hex_val = lookup_hex_values(CS);		//which pin of the port to set low
	
	//Write to CS of U4 main board
	if ((CS < 25) | ((CS >= 28) && (CS <= 37)) ) {
		TWI_start();
		TWI_write_address(IOX_1_WRITE);
		TWI_write_data(0x08 + val);
		TWI_write_data(hex_val);
		TWI_stop();
	}
	//Write to CS of U5 main board
	else {
		TWI_start();
		TWI_write_address(IOX_0_WRITE);
		TWI_write_data(0x08 + val);
		TWI_write_data(hex_val);
		TWI_stop();
	}
}

void CS_high(uint8_t CS)
{
	uint8_t val;
	
	val = lookup_cs_values(CS);			//which port to write to
	
	//Write to CS of U4 main board
	if ((CS < 25) | ((CS >= 28) && (CS <= 37))) {
		TWI_start();
		TWI_write_address(IOX_1_WRITE);
		TWI_write_data(0x08 + val);
		TWI_write_data(0xFF);
		TWI_stop();
	}
	//Write to CS of U5 main board
	else {
		TWI_start();
		TWI_write_address(IOX_0_WRITE);
		TWI_write_data(0x08 + val);
		TWI_write_data(0xFF);
		TWI_stop();
	}
}