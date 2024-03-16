// Simple Functions for setting up USART Interface
#pragma once

void UART_init()
{
	// Configure register UCSRA
	// U2X0  = 1 - Double the BRG speed (since I am using a 8MHz crystal which is divided by 8)
	// MPCM0 = 0 - Normal UART communication
	UCSR0A = (1 << U2X0);
	
	// Configure register UCSRB
	// RXCIE0 = 0 - We will not enable the receiver interrupt
	// TXCIE0 = 0 - We will not enable the transmitter interrupt
	// UDRIE0 = 0 - We will not enable the data register empty interrupt
	// RXEN0  = 1 - Enable reception
	// TXEN0  = 1 - Enable transmission
	// UCSZ02 = 0 - 8 bit character size
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// Configure register UCSRC
	// UMSEL00 = 0 - Normal Asynchronous Mode
	// UMSEL01 = 0
	// UPM00   = 0 - No Parity Bits
	// UPM01   = 0 - No Parity Bits
	// USBS0   = 0 - Use 1 stop bit
	// UCSZ01  = 1 - 8 bit character size
	// UCSZ00  = 1 -
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); /* Use 8-bit character sizes */
	
	// Refer to table 21-6 in Atmega168-PB Data sheet for Baud Rate
	// 12 -> 9600 when U2X0 == 1
	UBRR0H = 0;
	UBRR0L = 103;
}

unsigned char UART_RxChar()
{
	while ((UCSR0A & (1 << RXC0)) == 0);/* Wait till data is received */
	return(UDR0);			/* Return the byte*/
}

void UART_TxChar(char ch)
{
	while (! (UCSR0A & (1<<UDRE0)));	/* Wait for empty transmit buffer*/
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