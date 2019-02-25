#include "tinyEEPROM.h>

#define counter_storage 0
unsigned int counter;

int main(void) { 
 eeprom_read_byte(counter_storage,&counter);
 //if(counter==255) counter=0; will rollover anyways.
 eeprom_write_byte(counter_storage,++counter);
 
 while(1){
 //code
 }
      return 0; 
    }
