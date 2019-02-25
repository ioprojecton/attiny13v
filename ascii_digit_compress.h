#ifndef _ASCII_DIGIT_COMPRESS_H_
#define	_ASCII_DIGIT_COMPRESS_H_

#ifndef _STDIO_H_
#include <stdio.h>

unsigned char compress(unsigned char x, unsigned char y);
void uncompress(unsigned char input, unsigned char *output1, unsigned char *output2);

unsigned char compress(unsigned char x, unsigned char y){
x=(x & 0x0f) << 4;
y&=0x0F;
return x|y;
}

void uncompress(unsigned char input, unsigned char *output1, unsigned char *output2){
output1=((input & 0xf0) >> 4) | 0x30;
output2=input & 0x0f | 0x30;
return;
}
