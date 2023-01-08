

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include "arth.h"
#include "../inc/huffman.h"

void print_usage(char *file){
	printf("%s", "erro!");
	printf("%s -[d,e] [input file] [output file]\n", file);
	
}

int main(int argc, char** argv) {
	clock_t start_arth = 0, finish_arth = 0 ;
	clock_t start_huff = 0, finish_huff = 0 ;
    FILE *input, *output = NULL;
    FILE *input1, *output1 = NULL;
    double duration_arith, duration_huff;
	int symbol;

    // FILE* input = fopen("/Users/zhouchou/github-classroom/NCU-DSA-111-1/assignment_3-Mondayhs/data/data100.txt", "rb");
    // FILE* output = fopen("/Users/zhouchou/github-classroom/NCU-DSA-111-1/assignment_3-Mondayhs/data/ec.txt", "wb");
    
    
    
    if(argc!=5)
	{
		print_usage(argv[0]);
		return 1;
	}

	input = fopen(argv[2], "rb");
	input1 = fopen(argv[2], "rb");
	if(input)
	{
		if(argv[1][0]=='-')
		{
			output = fopen(argv[3], "wb");
			output1 = fopen(argv[4], "wb");
			switch(argv[1][1])
			{
			case 'e':

				printf("       start encoding by Arithmetic ...  \n");
				init_arth(output);
				start_arth = clock();
				while((symbol=fgetc(input))!=EOF){
					encode(symbol);
				}
				encode(-1);
				close_encode();
				finish_arth = clock();
                duration_arith = (double)(finish_arth - start_arth) / CLOCKS_PER_SEC;
	            printf("       time cost: %f\n", duration_arith);

				printf("       start encoding by Huffman ...    \n");
				start_huff = clock();
                pNode root = buildHuffmanTreeFromFile(input1);
                encodeFileData(root, input1, output1);
                finish_huff = clock();
                duration_huff = (double)(finish_huff - start_huff) / CLOCKS_PER_SEC;
	            printf("       time cost: %f\n", duration_huff);
				break;
			case 'd':

				printf("       start decoding by Arithmetic...\n");
				init_arth(input);
				start_arth = clock();
				while((symbol=decode())>=0){
					putc(symbol, output);
				}
				finish_arth = clock();

				break;
			}

			fclose(output);
			fclose(output1);
		}
		fclose(input);
		fclose(input1);
	}

    return 0;
}