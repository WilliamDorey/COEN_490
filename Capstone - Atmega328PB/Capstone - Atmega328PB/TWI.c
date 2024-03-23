// Basic functions for TWI / I2C interface
#pragma once

void TWI_init_master(void) // Function to initialize master
{
	TWBR0=0x02;    // Bit rate
	TWSR0=(1<<TWPS1)|(0<<TWPS0);    // Setting prescalar bits
	// SCL freq= F_CPU/(16+2(TWBR).4^TWPS)
}

void TWI_start(void)
{
	// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	TWCR0= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR0 & (1<<TWINT))); // Wait till start condition is transmitted
	while((TWSR0 & 0xF8)!= 0x08); // Check for the acknowledgment
}

void TWI_read_address(unsigned char data)
{
	TWDR0=data;    // Address and read instruction
	TWCR0=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR0 & (1<<TWINT))); // Wait till complete TWDR byte received
	while((TWSR0 & 0xF8)!= 0x40);  // Check for the acknowledgment
}

void TWI_read_data(unsigned char *data)
{
    TWCR0=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR0 & (1<<TWINT))); // Wait till complete TWDR byte transmitted
    while((TWSR0 & 0xF8) != 0x58); // Check for the acknowledgment
    (*data)=TWDR0;
}

void TWI_write_address(unsigned char data)
{
	TWDR0=data;    // Address and read instruction
	TWCR0=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR0 & (1<<TWINT))); // Wait till complete TWDR byte received
	while((TWSR0 & 0xF8)!= 0x18);  // Check for the acknowledgment
}

void TWI_write_data(unsigned char data)
{
	TWDR0=data;    // put data in TWDR
	TWCR0=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR0 & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR0 & 0xF8) != 0x28); // Check for the acknowledgment
}

void TWI_stop(void)
{
	// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	TWCR0= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(!(TWCR0 & (1<<TWSTO)));  // Wait till stop condition is transmitted
}
