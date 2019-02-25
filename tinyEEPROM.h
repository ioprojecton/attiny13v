#ifndef _TINY_EEPROM_H_
#define _TINY_EEPROM_H_

#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdbool.h>
#include<iotn13.h>

bool eeprom_write_byte(const unsigned char address, const unsigned char data);
bool eeprom_read_byte(unsigned char address,unsigned char *storage);

bool eeprom_write_byte(const unsigned char address, const unsigned char data){

if (address > 64) return false;

while (EECR & (1<<EEPE));

EECR|=(0<<EEPM1)|(0<<EEPM0);

EEARL=address;

EEDR=data;

bool flag=0;

if(SREG & (1<<7)) {
cli();
flag=1;
}
EECR|=(1<<EEMPE);

EECR|=(1<<EEPE);

if(flag) sei();

return;
}

bool eeprom_read_byte(const unsigned char address,unsigned char *storage){

if(address>64) return false;

while(EECR & (1<<EEPE));

EEARL=address;

EECR |= (1<<EERE);

storage=EEDR;

return;
}
