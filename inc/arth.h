#ifndef _ARTH_H_
#define _ARTH_H_

#include <stdio.h>

#define No_of_chars 256
#define EOF_symbol (No_of_chars+1)
#define No_of_symbols (No_of_chars+1)

#define Max_freq ((1<<14)-1)
#define Code_value_bits 16
#define Top_value (((long)1<<Code_value_bits)-1)
#define First_qtr	(Top_value/4+1)
#define Half		(2*First_qtr)
#define Third_qtr	(3*First_qtr)

void init_arth(FILE *fp);
void encode(int code);
int decode(void);
void close_encode(void);
void refine_bits(int bit);
void update_freq(int symbol);



#endif