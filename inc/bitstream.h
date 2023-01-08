#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

#include <stdio.h>

void putbits(int bit,int length);
void init_bitstream(FILE *fp);
void done_outputing_bits();
int getbit(void);

#endif