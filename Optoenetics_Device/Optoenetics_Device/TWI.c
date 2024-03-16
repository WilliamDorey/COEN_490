// Basic functions for TWI / I2C interface
#pragma once

void TWI_init_master(void) // Function to initialize master
{
	TWBR=0x02;    // Bit rate
	TWSR=(1<<TWPS1)|(0<<TWPS0);    // Setting prescalar bits
	// SCL freq= F_CPU/(16+2(TWBR).4^TWPS)
}

void TWI_start(void)
{
	// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT))); // Wait till start condition is transmitted
	while((TWSR & 0xF8)!= 0x08); // Check for the acknowledgment
}

void TWI_read_address(unsigned char data)
{
	TWDR=data;    // Address and read instruction
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte received
	while((TWSR & 0xF8)!= 0x40);  // Check for the acknowledgment
}

void TWI_read_data(unsigned char *data)
{
    TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
    while((TWSR & 0xF8) != 0x58); // Check for the acknowledgment
    (*data)=TWDR;
}

void TWI_write_address(unsigned char data)
{
	TWDR=data;    // Address and read instruction
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte received
	while((TWSR & 0xF8)!= 0x18);  // Check for the acknowledgment
}

void TWI_write_data(unsigned char data)
{
	TWDR=data;    // put data in TWDR
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8) != 0x28); // Check for the acknowledgment
}

void TWI_stop(void)
{
	// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted
}
