#include "ascii_digit_compress.h"

unsigned char b = '5';
unsigned char c = '6';


printf("b=%c,c=%c\n",b,c);

unsigned char d=compress(b,c);
printf("%c\n",d);


//write to eeprom or something;

//read from eeprom to d

uncompress(d,&b,&c);

printf("%c,%c\n",b,c);
//print b and c;

      return 0; 
    }
