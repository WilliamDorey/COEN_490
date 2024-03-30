                   #include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

#include "UART.c"
#include "SPI.c"
#include "LED.c"
#include "TWI.c"
#include "IOX.c"
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

uint8_t ascii_dec(char ascii[], uint8_t count){
	uint8_t val = 0;
	for (uint8_t i = 0; i < count; i ++){
		val = val * 10;
		val += ascii[i] - 0x30;
	}
	return val;
}

int main(){
	uint32_t data_temp = 0;
	uint16_t DAC_value = 0;
	uint16_t temp = 0;
	uint8_t LED[LED_COUNT] = {0};
	uint8_t ledID;
	uint8_t DAC_hi;
	uint8_t DAC_lo;
	uint8_t lvl;
	uint8_t pot;
	
	uint16_t results[46] = {0};
	uint8_t resultIdx = 0;

	unsigned char tmp;
	char opcode;
	char id[2];
	char colour;
	char dir;
	char num;
	char potID;

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
	
	UART_SendString("\r\nConfiguring TWI interface...");
	TWI_init_master();
	UART_SendString("DONE!");
	
	UART_SendString("\r\nConfiguring IO Expanders...");
	Init_IOX();
	UART_SendString("DONE!");

	UART_SendString("\r\nTurning Off LEDs...");
	LED_all_off(LED);
	UART_SendString("DONE!");
	
	for(uint8_t x = 0; x < 8; x ++)
	{
		if (x == 2 || x == 4){			//Potentiostat U3 not working
			continue;
		}
		_delay_ms(10);
		Read_LMP_Status(x, &tmp);
		UART_SendString("\r\nUNLOCKING LM91000 ");
		UART_TxChar(0x30 + x);
		UART_SendString("...");
		Set_LMP_Unlock(x);
		UART_SendString("DONE");
		_delay_ms(10);
	}
	UART_SendString("\r\n\n");
	
	UART_SendString("\n\rOPTOGENETIC DEVICE READY!\n\r");



	// MAIN EXECUTION LOOP
	while(1)
	{
		UART_SendString("\r\ncmd>");
		//Wait until data has been received
		opcode = UART_RxChar();
		UART_TxChar(opcode);

		switch(opcode){
			case 'L':
			//UART_SendString("\n\rSELECT LED>");
			id[0] = UART_RxChar();
			//UART_TxChar(id[0]);
			id[1] = UART_RxChar();
			//UART_TxChar(id[1]);
			ledID = ascii_dec(id, 2);
			//UART_SendString("\n\rSELECT COLOUR>");
			colour = UART_RxChar();
			//UART_TxChar(colour);

			ToggleLED(LED, ledID, colour);
			break;

			//Change DAC value
			case 'D':
			DAC_value = 0;
			uint16_t offset = 1000;
			//UART_SendString("\n\rSelect Direction (value 0000 - 4095) >");
			for (uint8_t i = 0; i < 4; i ++)
			{
				dir = UART_RxChar();
				//UART_TxChar(dir);
				DAC_value += offset * (dir - 0x30);
				offset /= 10;
			}

			//UART_SendString("\n\rSending DAC command...");
			DAC_hi = 0b00110000 | ((DAC_value & 0x3C0) >> 6);
			DAC_lo = (DAC_value & 0x3F) << 2;
			DAC_Program(DAC_hi, DAC_lo);
			break;

		//Change intensity of LEDs
		case 'R':
			//UART_SendString("\n\rSELECT LED>");
			id[0] = UART_RxChar();
			//UART_TxChar(id[0]);
			id[1] = UART_RxChar();
			//UART_TxChar(id[1]);
			ledID = ascii_dec(id, 2);
			//UART_SendString("\n\rSELECT COLOUR>");
			colour = UART_RxChar();
			//UART_TxChar(colour);
			
			//UART_SendString("\n\rSET RESISTANCE (NNN; 000-255)>");
			num = UART_RxChar();
			//UART_TxChar(num);
			lvl = (num - 0x30) * 100;
			num = UART_RxChar();
			//UART_TxChar(num);
			lvl += (num - 0x30) * 10;
			num = UART_RxChar();
			//UART_TxChar(num);
			lvl += (num - 0x30);
			
			SetLEDBrightness(ledID, colour, lvl);
			break;
			
			//Turn off all LEDs
			case 'X':
			//UART_SendString("\r\nResetting all LEDs!");
			LED_all_off(LED);
			break;
			
			//Turn on all LEDs of a colour
			case 'A':
			//UART_SendString("\n\rSELECT COLOUR>");
			colour = UART_RxChar();
			//UART_TxChar(colour);
			
			LED_all_on(LED, colour);
			break;
			
      //Show Temperature of potentiostat
			case 'T':
			data_temp = 0;
			temp = 0;
			
			UART_SendString("\n\rSELECT POTENTIOSTAT (0-7) - EXCEPT 2>");
			potID = UART_RxChar();
			UART_TxChar(potID);
			pot = potID - 0x30;
			
			if ((pot > 7) | (pot == 2)){
				UART_SendString("\n\rNOT A VALID ID OR POTENTIOSTAT OUT OF ORDER!");
				break;
			}
			
			Set_LMP_Temperature(pot);
			
			//Take an average measurement
			for(uint8_t i = 0; i < 16; i++)
			{
				temp = Read_ADC(0, pot);
				data_temp = data_temp + temp;
			}
			data_temp = data_temp / 16;
			
			PORTD = 0xFF;
			UART_SendString("\n\rCH");
			UART_TxChar(potID);
			UART_SendString(" : ");
			UART_TxChar(0x30 + (data_temp / 1000) );
			UART_TxChar(0x30 + ((data_temp % 1000) / 100) );
			UART_TxChar(0x30 + ((data_temp % 100) / 10) );
			UART_TxChar(0x30 + (data_temp % 10));
			
			UART_SendString("\n\rTEMP READ!\r\n");
			break;
			
		case 'V':
			resultIdx = 0;
			pot = 0;
			Prog_LMP(0, 0x12, 0x02); // FET = Disabled, MODE = Standby
			_delay_ms(50);
			Prog_LMP(0, 0x10, 0x1F); // TIA_GAIN = 350k Ohm, RLOAD = 100 Ohm
			_delay_ms(50);
			Prog_LMP(0, 0x11, 0x20); // REF = External, INT_Z = 50%, BIAS_SIGN = Negative, BIAS = 0%
			_delay_ms(50);
			Prog_LMP(0, 0x12, 0x03); // FET = Disabled, MODE = 3-Lead amperometric
			_delay_ms(50);

			for(uint8_t i = 0; i <= 11; i ++)
			{
				Prog_LMP(0, 0x11, 0x20 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Negative, BIAS = i
				_delay_ms(100);
				results[resultIdx] = Read_ADC(0,pot);
				resultIdx ++;
			}
			for(uint8_t i = 11; i > 0; i --)
			{
				Prog_LMP(0, 0x11, 0x20 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Negative, BIAS = i
				_delay_ms(100);
				results[resultIdx] = Read_ADC(0,pot);
				resultIdx ++;
			}
			for(uint8_t i = 0; i <= 11; i ++)
			{
				Prog_LMP(0, 0x11, 0x30 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Positive, BIAS = i
				_delay_ms(100);
				results[resultIdx] = Read_ADC(0,pot);
				resultIdx ++;
			}
			for(uint8_t i = 11; i > 0; i --)
			{
				Prog_LMP(0, 0x11, 0x30 + i); // REF = External, INT_Z = 50%, BIAS_SIGN = Positive, BIAS = i
				_delay_ms(100);
				results[resultIdx] = Read_ADC(0,pot);
				resultIdx ++;
			}
			
			for (uint8_t i = 0; i < resultIdx; i++)
			{
				UART_SendString("\n\rRESULT ");
				UART_TxChar(0x30 + ((i % 1000) / 100) );
				UART_TxChar(0x30 + ((i % 100) / 10) );
				UART_TxChar(0x30 + (i % 10));
				UART_SendString(" : ");
				UART_TxChar(0x30 + (results[i] / 1000) );
				UART_TxChar(0x30 + ((results[i] % 1000) / 100) );
				UART_TxChar(0x30 + ((results[i] % 100) / 10) );
				UART_TxChar(0x30 + (results[i] % 10));
			}
			
			break;
			
		default:
			UART_SendString("\r\nERR: NOT A VALID OPCODE!");
			break;
		}
	}
}