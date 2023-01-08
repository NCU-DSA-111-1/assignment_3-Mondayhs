#include <stdio.h>
#include "bitstream.h"

FILE *st;
int buffer;
int freebits;
int outbytes;

void init_bitstream(FILE *fp)
{
	buffer = 0;
	freebits = 8;
	outbytes = 0;
	st = fp;
}                                   

void done_outputing_bits()
{
	putc(buffer>>freebits,st);
	outbytes ++;
}

void putbits(int bit,int length)
{
	buffer >>= 1;
	if(bit) buffer |= 0x80;
	freebits -= 1;
	if(freebits == 0)
	{
		putc(buffer, st);
		outbytes ++;
		freebits = 8;      
	}
}

int getbit(void)
{
	int retval;

	if(freebits==8)
	{
		if((buffer=fgetc(st))==EOF) return 1;
		freebits = 0;
	}
	retval = buffer & 1;
	buffer >>= 1;
	freebits ++;

	return retval;
}