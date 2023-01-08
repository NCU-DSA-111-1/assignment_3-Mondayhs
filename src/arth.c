#include <stdio.h>

#include "bitstream.h"
#include "arth.h"

#define putbit1() refine_bits(1)
#define putbit0() refine_bits(0)

int c2i[No_of_chars];
unsigned char i2c[No_of_symbols+1];
int freq[No_of_symbols+1];
int cum[No_of_symbols+1];
int low, high;
int refine, value;

void init_arth(FILE *fp)
{
	register int i;

	for(i=0; i<No_of_chars; i++)
	{
		c2i[i] = i+1;
		i2c[i+1] = i;
	}

	for(i=0; i<=No_of_symbols; i++)		// 建立出現頻率表格
	{
		freq[i] = 1;
		cum[i] = No_of_symbols - i;		// 建立頻率累計表
	}

	freq[0] = 0;

	low = 0;
	high = Top_value;
	value = refine = 0;

	init_bitstream(fp);
}

void close_encode(void)
{
	refine += 1;
	if(low<First_qtr) putbits(0, 1);
	else putbits(1, 1);

	done_outputing_bits();
}

int decode(void)
{
	static int first_time = 1;
	int range;
	int curr_cum, symbol;

	if(first_time)
	{
		for(range=1; range<=Code_value_bits; range++)
		{
			value = (value<<1) + getbit();
		}
		first_time = 0;
	}

	range = (high - low) + 1;
	curr_cum = (((value-low)+1)*cum[0]-1)/range;
	for(symbol = 1; cum[symbol]>curr_cum; symbol++);
	high = low + (range*cum[symbol-1])/cum[0]-1;
	low = low + (range*cum[symbol])/cum[0];

	while(1)
	{
		if(high<Half)
		{

		}
		else if (low>=Half)
		{
			value -= Half;
			low -= Half;
			high -= Half;
		}
		else if(low>=First_qtr && high<Third_qtr)
		{
			value -= First_qtr;
			low -= First_qtr;
			high -= First_qtr;
		}
		else break;

		low <<= 1;
		high = (high<<1)+1;
		value = (value<<1)+getbit();
	}

	if(symbol==EOF_symbol) return -1;
	update_freq(symbol);

	return i2c[symbol];
}

void encode(int code)
{
	long range;
	int symbol;

	if(code<0) symbol = EOF_symbol;
	else symbol = c2i[code];

	range = (long) (high - low) + 1;	// 目前編碼區間的長度
	high = low + (range*cum[symbol-1])/cum[0] - 1;
	low = low + (range*cum[symbol])/cum[0];

	while(1)
	{
		if(high<Half)
		{
			putbit0();
		}
		else if (low>=Half)
		{
			putbit1();
			low -= Half;
			high -= Half;
		}
		else if (low >= First_qtr && high<Third_qtr)
		{
			refine += 1;
			low -= First_qtr;
			high -= First_qtr;
		}
		else break;

		low <<= 1;		// 將區間擴展開來
		high = (high<<1) + 1;	
	}

	update_freq(symbol);
}

void refine_bits(int bit)
{
	putbits(bit, 1);
	while(refine>0)
	{
		putbits(!bit, 1);
		refine -= 1;
	}
}

void update_freq(int symbol)
{
	if(cum[0]>=Max_freq)
	{
		int i, temp=0;
		for(i=No_of_symbols; i>=0; i--)
		{
			freq[i] = (freq[i]+1)>>1;
			cum[i] = temp;
			temp += freq[i];
		}
	}

	freq[symbol] += 1;

	while(symbol >= 0)
	{
		symbol --;
		cum[symbol] += 1;
	}
}
