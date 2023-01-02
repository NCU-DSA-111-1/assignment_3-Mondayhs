#ifndef Arithmetic_H
#define Arithmetic_H

#include <stdio.h>

#define MAX_RANGE   0x100000000ULL

typedef unsigned int uint32;

// Probability Density Function
typedef struct {
	uint32 symbol;
	uint32 frequency;
} PDFEntry;

typedef struct {
	uint32 lower;
	uint32 upper;
} Range;

typedef struct {
  	char *filename;
	FILE *file;
	char *buffer;
  	uint32 length;
  	uint32 byteIndex;
  	uint32 bitIndex;  
} Stream;

int streamFlush(Stream *out);
int streamWriteBit(Stream *out, int bit);
int getRangeOfSymbol(Range *range, uint32 symbol, PDFEntry pdfTable[], int pdfTableSize);
void arithmeticEncoder(char data[], int dataSize, PDFEntry pdfTable[], int pdfTableSize);

#endif // Arithmetic_H
