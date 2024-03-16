#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#include "UART.c"
#include "SPI.c"
#include "TWI.c"
#include "IOX.c"
#include "LED.c"
#include "Potentiostats.c"

#define IO_EN_0 PINB1
#define DAC_SEL PINB0

#define I2C_RST PINC0


#define DAC_STEP 0x10

void DAC_Program(uint8_t hi, uint8_t lo)
{
	PORTB ^= (1 << DAC_SEL);
	SPI_Write(hi);
	SPI_Write(lo);
	PORTB ^= (1 << DAC_SEL);
}

int main()
{
	uint8_t  LED[LED_COUNT] = {0};
	uint16_t DAC_value = 0;
	char opcode;
	
	char id, colour, num;
	uint8_t lvl, dec_id;

	// DDR Setup
	DDRB = (1 << IO_EN_0) | (1 << DAC_SEL) | (1 << SS)  | (1 << MOSI) | (1 << SCK);
	PORTB =  (1 << DAC_SEL);
	
	DDRC = (1 << I2C_RST);
	PORTC = 0xFF;
	
	DDRD  = 0xFF;
	PORTD = 0xFF;
	
	UART_init();
	UART_SendString("\r\nConfiguring SPI interface...");	
	SPI_Init();	
	UART_SendString("DONE!");	
	
	UART_SendString("\r\nConfiguring TWI/I2C interface...");
	TWI_init_master();
	UART_SendString("DONE!");

	UART_SendString("\r\nCONFIGURING IO EXPANDERS...");
	Init_IOX();
	UART_SendString("DONE!");

	UART_SendString("\r\nTURNING OFF LEDS...");
	LED_all_off(LED);
	UART_SendString("DONE");


	unsigned char tmp;
	Read_LMP_Status(0, &tmp);
	for(uint8_t x = 0; x < 2; x ++)
	{
		UART_SendString("\r\nUNLOCKING LM91000 ");
		UART_TxChar(0x30 + x);
		UART_SendString("...");
		Set_LMP_Unlock(x);
		UART_SendString("DONE");
	}
	UART_SendString("\r\n\n");
	
	//Cyclic V
	_delay_ms(50);
	/*
	Set_LMP_Standby(0);
	Set_LMP_FET(0, 1);
	Set_LMP_gain();
	Set_LMP_RLoad();
	Set_LMP_ExtRef();
	Set_LMP_IntZ();
	Set_LMP_Amperic(0);
	*/
	Prog_LMP(0, 0x12, 0x02); // FET = Disabled, MODE = Standby
			_delay_ms(50);
	Prog_LMP(0, 0x10, 0x04); // TIA_GAIN = 2.75k Ohm, RLOAD = 10 Ohm
			_delay_ms(50);
	Prog_LMP(0, 0x11, 0x20); // REF = External, INT_Z = 50%, BIAS_SIGN = Negative, BIAS = 0%
			_delay_ms(50);
	Prog_LMP(0, 0x12, 0x03); // FET = Disabled, MODE = 3-Lead amperometric
			_delay_ms(50);
	while(1)
	{
		PORTB = 0x00;
		for(uint8_t i = 0; i <= 11; i ++)
		{
			Prog_LMP(0, 0x11, 0x20 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Negative, BIAS = i
			_delay_ms(100);
		}
		for(uint8_t i = 10; i >= 0; i --)
		{
			Prog_LMP(0, 0x11, 0x20 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Negative, BIAS = i
			_delay_ms(100);
		}
		PORTB = 0xFF;
		for(uint8_t i = 0; i <= 11; i ++)
		{
			Prog_LMP(0, 0x11, 0x30 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Positive, BIAS = i
			_delay_ms(100);
		}
		for(uint8_t i = 10; i >= 0; i --)
		{
			Prog_LMP(0, 0x11, 0x30 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Positive, BIAS = i
			_delay_ms(100);
		}
	}
	
	// Temperature
	Set_LMP_Temperature(0);
	while(1)
	{
		uint16_t data = 0;
		
		PORTD = 0x00;
		data = Read_ADC(0,0);
		PORTD = 0xFF;
		UART_SendString("CH0 : ");
		UART_TxChar(0x30 + (data / 1000) );
		UART_TxChar(0x30 + ((data % 1000) / 100) );
		UART_TxChar(0x30 + ((data % 100) / 10) );
		UART_TxChar(0x30 + (data % 10));

		PORTD = 0x00;
		data = Read_ADC(0,1);
		PORTD = 0xFF;
		UART_SendString("\tCH1 : ");
		UART_TxChar(0x30 + (data / 1000) );
		UART_TxChar(0x30 + ((data % 1000) / 100) );
		UART_TxChar(0x30 + ((data % 100) / 10) );
		UART_TxChar(0x30 + (data % 10));

		UART_TxChar('\r');
	}
	
	UART_SendString("\n\rOPTOGENETIC DEVICE READY!\n\r");

	// MAIN EXECUTION LOOP
	while(1)
    {
		UART_SendString("\r\ncmd>");
	    // Wait until data has been received
		opcode = UART_RxChar();
		UART_TxChar(opcode);
		
		switch (opcode)
		{
		case 'L':
			UART_SendString("\n\rSELECT LED>");
			id = UART_RxChar();
			UART_TxChar(id);
			dec_id = id - 0x30;
			UART_SendString("\n\rSELECT COLOUR>");
			colour = UART_RxChar();
			UART_TxChar(colour);
			
			ToggleLED(LED, dec_id, colour);
			break;
		
		case 'V':
			UART_SendString("\n\rSelect Direction (U/D)>");
			char dir = UART_RxChar();
			UART_TxChar(dir);
			if (dir == 'U')
			{
				DAC_value += DAC_STEP;
			} else {
				DAC_value -= DAC_STEP;
			}
			UART_SendString("\n\rSending DAC command...");
			uint8_t DAC_hi, DAC_lo;
			DAC_hi = 0b00110000 | ((DAC_value & 0x3C0) >> 6);
			DAC_lo = (DAC_value & 0x3F) << 2;
			DAC_Program(DAC_hi, DAC_lo);
			break;
			
		case 'X':
			UART_SendString("\r\nResetting all LEDS...");
			LED_all_off(LED);			
			break;
		
		case 'R':
			UART_SendString("\n\rSELECT LED>");
			id = UART_RxChar();
			UART_TxChar(id);
			dec_id = id - 0x30;
			UART_SendString("\n\rSELECT COLOUR>");
			colour = UART_RxChar();
			UART_TxChar(colour);

			UART_SendString("\n\rSET RESISTANCE (NNN; 000-255)>");
			num = UART_RxChar();
			UART_TxChar(num);
			lvl = (num - 0x30) * 100;
			num = UART_RxChar();
			UART_TxChar(num);
			lvl += (num - 0x30) * 10;
			num = UART_RxChar();
			UART_TxChar(num);
			lvl += (num - 0x30);
				
			SetLEDBrightness(dec_id, colour, lvl);
			break;
			
		default:
			UART_SendString("\r\nERR: NOT A VALID OPCODE!");
			break;	
		}
	    
    }
}

