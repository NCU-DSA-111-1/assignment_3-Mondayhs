#include <stdio.h>
#include "Arithmetic.h"

int streamWriteBit(Stream *out, int bit) {
  if(out->bitIndex == 0)
    out->buffer[out->byteIndex] = 0;
  out->buffer[out->byteIndex] |= (bit & 1) << out->bitIndex;
  if(++out->bitIndex >= 8) {
    out->bitIndex = 0;
    out->byteIndex++;
  }  
  return out->byteIndex * 8 + out->bitIndex;
}

int getRangeOfSymbol(Range *range, uint32 symbol, PDFEntry pdfTable[], int pdfTableSize) {
  int found = 0;
  uint32 i, total = 0, lowCumulativeFreq, highCumulativeFreq;
  for(i = 0; i < pdfTableSize; i++) {    
    total += pdfTable[i].frequency;
    if(pdfTable[i].symbol == symbol) {
      found = 1;
      break;
    }  
  }
  if(i < pdfTableSize) {
    highCumulativeFreq = total;
    lowCumulativeFreq = total - pdfTable[i].frequency;
    printf("lower:%u, upper:%u\n", lowCumulativeFreq, highCumulativeFreq);  
    for(i++; i < pdfTableSize; i++) {    
      total += pdfTable[i].frequency;
    }
    range->lower = MAX_RANGE * lowCumulativeFreq / total;
    range->upper = MAX_RANGE * highCumulativeFreq / total - 1;    
  }  
  printf("range.lower:%d, range.upper:%d, total: %d\n", range->lower, range->upper, total);  
  return found;
}

void arithmeticEncoder(char data[], int dataSize, PDFEntry pdfTable[], int pdfTableSize) {
  
}