// Simple Functions for setting up USART Interface
#pragma once

#define F_CPU 8000000UL		// Defining the CPU Frequency

#include <avr/io.h>			// Contains all the I/O Register Macros
#include <util/delay.h>		// Generates a Blocking Delay

#define USART_BAUDRATE 9600 //Desired Baud Rate
#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection
#define PARITY_MODE (0<<UPM00) // USART Parity Bit Selection = Disabled
#define STOP_BIT (0<<USBS0)    // USART Stop Bit Selection = 1 bit
#define DATA_BIT (3<<UCSZ00)   // USART Data Bit Selection = 8 nit

void UART_init()
{
	// Configure register UCSRA
	// U2X  = 1 - Double the BRG speed (since I am using a 8MHz crystal which is divided by 8)
	// MPCM0 = 0 - Normal UART communication
	UCSR0A = (1 << U2X0);
	
	// Configure register UCSRB
	// RXCIE = 0 - We will not enable the receiver interrupt
	// TXCIE = 0 - We will not enable the transmitter interrupt
	// UDRIE = 0 - We will not enable the data register empty interrupt
	// RXEN  = 1 - Enable reception
	// TXEN  = 1 - Enable transmission
	// UCSZ2 = 0 - 8 bit character size
	//UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Configure register UCSRC
	// UMSEL00 = 0 - Normal Asynchronous Mode
	// UMSEL01 = 0
	// UPM00   = 0 - No Parity Bits
	// UPM01   = 0 - No Parity Bits
	// USBS0   = 0 - Use 1 stop bit
	// UCSZ01  = 1 - 8 bit character size
	// UCSZ00  = 1 -
	//UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); /* Use 8-bit character sizes */
	
	// Refer to table 24-8 in Atmega328-PB Data sheet for Baud Rate
	// 12 -> 9600 when U2X0 == 1
	UBRR0H = 0;
	UBRR0L = 103;
	
	// Set Frame Format
	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
	
	// Enable Receiver and Transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

unsigned char UART_RxChar()
{
	while ((UCSR0A & (1 << RXC0)) == 0);/* Wait till data is received */
	return(UDR0);			/* Return the byte*/
}

void UART_TxChar(char ch)
{
	while ( (UCSR0A & (1<<UDRE0)) == 0) {};	/* Wait for empty transmit buffer*/
	UDR0 = ch ;
}

void UART_SendString(char *str)
{
	unsigned char j=0;
	
	while (str[j]!=0)		/* Send string till null */
	{
		UART_TxChar(str[j]);
		j++;
	}
}