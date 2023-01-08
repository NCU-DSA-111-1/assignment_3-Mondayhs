#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#include <stdio.h>
#include <sys/malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


/*数据列表长度*/
#define LIST_SIZE 256
/*构建Huffman树需要产生的森林长度*/
#define FOREST_SIZE (LIST_SIZE * 2 - 1)
/*单个数据产生的Huffman编码文本串的最大容量*/
#define CODE_MAX 2048

/*文件路徑長度*/
#define PATH_MAX 1024

// static const char FILE_HEADER_FLAG;


/*文件類型*/
enum {
    FILE_TYPE_NULL,          /*读取出错*/
    FILE_TYPE_ENCODE,        /*原始数据文件*/
    FILE_TYPE_DECODE,        /*压缩数据文件*/
};

/*字節類型*/
typedef unsigned char Byte;

/*节点标识*/
enum {
    NODE_FLAG_ROOT,          /*根节点*/
    NODE_FLAG_LEFT,          /*左孩子节点*/
    NODE_FLAG_RIGHT          /*右孩子节点*/
};

/*节点类型*/
enum {
    NODE_TYPE_DATA,          /*数据节点*/
    NODE_TYPE_BLANK          /*空节点*/
};

/*Huffman树节点*/
typedef struct _tNode {
    int type;                /*节点类型*/
    int data;                /*节点数据*/
    int weight;              /*节点权重*/
    char code[CODE_MAX];     /*Huffman编码*/
    struct _tNode* left;     /*左孩子*/
    struct _tNode* right;    /*右孩子*/
}Node, * pNode;

/*Huffman树信息*/
typedef struct _tHuffmanTree {
    pNode root;              /*根节点*/
    int total                /*总字节数*/;
}HuffmanTree, * pHuffmanTree;


/*========================================================================
    TIME process functions.
 */
struct timeval startTimestamp();
double endTimestamp(struct timeval start);

/*========================================================================
    Huffuman code functions.
 */
pNode createDataNode(int data, int weight);
pNode createBlankNode(int weight);
void addNodeToList(pNode nodelist[], int size, pNode node);
pNode popMinNodeFromList(pNode nodelist[], int size);
void generateHuffmanCode(pNode root);
pNode buildHuffmanTree(int times[]);
pNode setpHuffmanTree(pNode root, int flag);
void destroyHuffmanTree(pNode root);
pNode buildHuffmanTreeFromFile(FILE* input);
int countHuffmanTreeWeightTotal(pNode root);
void convertTreeToList(pNode root, pNode nodelist[]);
int trimNodeList(pNode nodelist[], int size);
void decodeFileData(pNode root, FILE* input, FILE* output, int count);
int canShowChar(char ch);
void outputHuffmanTree(FILE* output, pNode root, int flag);
void printHuffmanTree(FILE* output, pNode root);
void printHuffmanList(FILE* output, pNode root);
void contUserInputTimes(int times[]);
pNode inputDataToBuildHuffmanTreeOption();
int inputOption(int begin, int end);

/*========================================================================
    FILE read & write functions.
 */
int getFileType(const char filename[]);
int writeFileHeader(pNode root, FILE* output);
void readFileHeader(FILE* input, int times[]);
void toDecode(pNode root, FILE* input);
void toEncode(pNode root, FILE* input);
int encodeFileData(pNode root, FILE* input, FILE* output);
void encodeFileOption(const char filename[]);
void decodeFileOption(const char filename[]);

void huffmanDemo();
void inputDataToTestOption();
void readFileOption();



#endif