/********************************************
* Huffman Code Demo

* Copyright (C) i@foxzzz.com
*
* C implementation of the Huffman Code's
* encoding and decoding.
*********************************************/


#include "../inc/huffman.h"
/*文件頭標示*/
static const char FILE_HEADER_FLAG[] = { 'F', 'X' };


/*========================================================================
    TIME process functions.
 */
/*得到当前时间戳*/
struct timeval startTimestamp() {
    struct timeval stamp;
    gettimeofday(&stamp, NULL);
    return stamp;
}
/*计算从时间戳到当前时间的毫秒*/
double endTimestamp(struct timeval start) {
    int diff_sec = 0;
    double start_msec = 0;
    double end_msec = 0;
    struct timeval end;
    gettimeofday(&end, NULL);
    diff_sec = (int)(end.tv_sec - start.tv_sec);
    start_msec = (double)start.tv_usec / 1000.0;
    end_msec = (diff_sec * 1000) + ((double)end.tv_usec / 1000.0);
    return (end_msec - start_msec);
}

/*========================================================================
    Huffuman code functions.
 */

/*创建Huffman树的数据节点*/
pNode createDataNode(int data, int weight) {
    pNode node = (pNode)malloc(sizeof(Node));
    memset(node, 0, sizeof(Node));
    node->type = NODE_TYPE_DATA;
    node->data = data;
    node->weight = weight;
    return node;
}

/*创建Huffman树的空节点*/
pNode createBlankNode(int weight) {
    pNode node = (pNode)malloc(sizeof(Node));
    memset(node, 0, sizeof(Node));
    node->type = NODE_TYPE_BLANK;
    node->weight = weight;
    return node;
}

/*将Huffman树节点添加到森林列表*/
void addNodeToList(pNode nodelist[], int size, pNode node) {
    int index;
    for (index = 0; index < size; ++index) {
        if (nodelist[index] == NULL) {
            /*从表中找到空位放入新节点*/
            nodelist[index] = node;
            break;
        }
    }
}

/*从森林列表弹出权重最低的Huffman树节点*/
pNode popMinNodeFromList(pNode nodelist[], int size) {
    int min = -1;
    int index;
    for (index = 0; index < size; ++index) {
        if (nodelist[index]) {
            if (min == -1) {
                min = index;
            } else {
                if (nodelist[min]->weight > nodelist[index]->weight) {
                    /*当发现存在更小权重节点时候更新记录*/
                    min = index;
                }
            }
        }
    }
    if (min != -1) {
        pNode node = nodelist[min];
        nodelist[min] = NULL;
        return node;
    }
    return NULL;
}

/*通过递归遍历方式为Huffman树中的的所有节点产生Huffman编码*/
void generateHuffmanCode(pNode root) {
    if (root) {
        if (root->left) {
            strcpy(root->left->code, root->code);
            strcat(root->left->code, "0");
            generateHuffmanCode(root->left);
        }
        if (root->right) {
            strcpy(root->right->code, root->code);
            strcat(root->right->code, "1");
            generateHuffmanCode(root->right);
        }
    }
}

/*传入权重表构建Huffman树*/
pNode buildHuffmanTree(int times[]) {
    pNode nodelist[FOREST_SIZE] = { NULL };
    pNode root = NULL;
    // struct timeval startstamp = startTimestamp();
    int index;
    /*创建森林表*/
    for (index = 0; index < LIST_SIZE; ++index) {
        if (times[index] > 0) {
            /*将所有节点逐个放入森林表*/
            addNodeToList(nodelist, FOREST_SIZE, createDataNode(index, times[index]));
        }
    }
    /*构建Huffman树*/
    while (1) {
        pNode left = popMinNodeFromList(nodelist, FOREST_SIZE);
        pNode right = popMinNodeFromList(nodelist, FOREST_SIZE);
        if (right == NULL) {
            /*当森林中只剩下一颗树节点的时候表示整个Huffman树构建完成*/
            root = left;
            break;
        } else {
            pNode node = createBlankNode(left->weight + right->weight);
            node->left = left;
            node->right = right;
            /*每次从森林表中取出两个最小的节点，并创建新节点重新放入森林表*/
            addNodeToList(nodelist, FOREST_SIZE, node);
        }
    }
    generateHuffmanCode(root);
    // printf("       bulid huffman tree : %lf msc\n", endTimestamp(startstamp));
    return root;
}

/*在Huffman树中前进一步*/
pNode setpHuffmanTree(pNode root, int flag) {
    switch (flag) {
    case NODE_FLAG_LEFT:
        return root->left;
    case NODE_FLAG_RIGHT:
        return root->right;
    }
    return NULL;
}

/*通过后序遍历的方式销毁Huffman树*/
void destroyHuffmanTree(pNode root) {
    if (root) {
        destroyHuffmanTree(root->left);
        destroyHuffmanTree(root->right);
        free(root);
    }
}

/*从文件构建Huffman树*/
pNode buildHuffmanTreeFromFile(FILE* input) {
    int times[LIST_SIZE] = { 0 };
    Byte byte;
    while (fread(&byte, sizeof(byte), 1, input) == 1) {
        ++times[byte];
    }
    return buildHuffmanTree(times);
}

/*计算Huffman树的权重总值*/
int countHuffmanTreeWeightTotal(pNode root) {
    int total = 0;
    if (root) {
        /*只获取有效字符节点*/
        if (root->type == NODE_TYPE_DATA) {
            total = root->weight;
        }
        total += countHuffmanTreeWeightTotal(root->left);
        total += countHuffmanTreeWeightTotal(root->right);
    }
    return total;
}

/*通过递归遍历将Huffman树转换成Huffman表*/
void convertTreeToList(pNode root, pNode nodelist[]) {
    if (root) {
        /*只获取有效字符节点*/
        if (root->type == NODE_TYPE_DATA) {
            nodelist[root->data] = root;
        }
        convertTreeToList(root->left, nodelist);
        convertTreeToList(root->right, nodelist);
    }
}

/*清理Huffman表中的空指针，并返回实际的表元素数量*/
int trimNodeList(pNode nodelist[], int size) {
    int count = 0;
    int index;
    for (index = 0; index < size; ++index) {
        pNode node = nodelist[index];
        if (node) {
            nodelist[count++] = node;
        }
    }
    return count;
}
/*对文件进行解码*/
void toDecode(pNode root, FILE* input) {
    char filename[PATH_MAX] = { 0 };
    FILE* output = NULL;
    printf("    *******************************\n");
    printf("        # Write File #\n");
    printf("\n");
    printf("       write file name : ");
    scanf("%s", filename);
    output = fopen(filename, "wb");
    if (output) {
        struct timeval startstamp = startTimestamp();
        decodeFileData(root, input, output, countHuffmanTreeWeightTotal(root));
        printf("       Elapsed Time      : %lf msc\n", endTimestamp(startstamp));
        printf("\n");
        printf("       Execute successfully.\n");
        fclose(output);
    } else {
        printf("       error : Failed to write file.\n");
    }
    printf("\n");
}
/*对文件数据进行Huffman编码*/
int encodeFileData(pNode root, FILE* input, FILE* output) {
    int total = 0;
    int count = 0;
    if (root) {
        Byte byte;
        int buffer = 0;
        pNode nodelist[LIST_SIZE] = { NULL };
        /*将Huffman树转换成Huffman表*/
        convertTreeToList(root, nodelist);
        while (fread(&byte, sizeof(byte), 1, input) == 1) {
            char* cursor = nodelist[byte]->code;
            while (*cursor) {
                buffer <<= 1;
                if (*cursor == '0') {
                    buffer |= 0;
                } else {
                    buffer |= 1;
                }
                ++count;
                if (count == 8) {
                    Byte byte = (Byte)buffer;
                    fwrite(&byte, sizeof(byte), 1, output);
                    count = 0;
                    buffer = 0;
                    ++total;
                }
                ++cursor;
            }
        }
        if (count > 0) {
            buffer <<= (8 - count);
            char byte = (char)buffer;
            fwrite(&byte, 1, 1, output);
            ++total;
        }
    }
    return total;
}

/*对文件数据进行Huffman解码*/
void decodeFileData(pNode root, FILE* input, FILE* output, int count) {
    if (root) {
        Byte byte;
        pNode cursor = root;
        while (fread(&byte, sizeof(byte), 1, input) == 1) {
            int buffer = byte;
            int index;
            for (index = 0; index < 8; ++index) {
                buffer <<= 1;
                if (!cursor->left || !cursor->right) {
                    Byte data = (Byte)cursor->data;
                    fwrite(&data, sizeof(data), 1, output);
                    if (--count == 0) {
                        break;
                    }
                    cursor = root;
                }
                if (buffer & ~0xff) {
                    cursor = setpHuffmanTree(cursor, NODE_FLAG_RIGHT);
                } else {
                    cursor = setpHuffmanTree(cursor, NODE_FLAG_LEFT);
                }
                buffer &= 0xff;
            }
        }
    }
}

/*检测是否是可显示字符*/
int canShowChar(char ch) {
    return (ch > 32 && ch < 127);
}

/*通过递归遍历方式打印Huffman树*/
void outputHuffmanTree(FILE* output, pNode root, int flag) {
    if (root) {
        int index;
        char content[128] = { 0 };
        const char* flagname[] = { "ROOT", "LEFT", "RIGHT" };
        int offset = (int)strlen(root->code) - 1;
        for (index = 0; index < offset; ++index) {
            if (root->code[index] == '0') {
                fprintf(output, " │ ");
            } else {
                fprintf(output, "   ");
            }
        }
        switch (root->type) {
        case NODE_TYPE_DATA:
            sprintf(content, "> %-6s #%-4d 0x%02X : '%c'", flagname[flag], root->weight, root->data, canShowChar((char)root->data) ? root->data : ' ');
            break;
        case NODE_TYPE_BLANK:
            sprintf(content, "[+] %-6s #%-4d", flagname[flag], root->weight);
            break;
        }
        switch (flag) {
        case NODE_FLAG_ROOT:
            fprintf(output, "%s", content);
            break;
        case NODE_FLAG_LEFT:
            fprintf(output, " ├─%s", content);
            break;
        case NODE_FLAG_RIGHT:
            fprintf(output, " └─%s", content);
            break;
        }
        if (root->type == NODE_TYPE_DATA) {
            fprintf(output, " CODE : %s\n", root->code);
        } else {
            fprintf(output, "\n");
        }
        outputHuffmanTree(output, root->left, NODE_FLAG_LEFT);
        outputHuffmanTree(output, root->right, NODE_FLAG_RIGHT);
    }
}

/*打印Huffman树*/
void printHuffmanTree(FILE* output, pNode root) {
    fprintf(output, "    *******************************\n");
    fprintf(output, "       Print Huffman Tree\n");
    fprintf(output, "\n");
    outputHuffmanTree(output, root, NODE_FLAG_ROOT);
    fprintf(output, "\n");
}

/*========================================================================
    Present
 */

/*将Huffman表中的数据输出成编码和权重统计表*/
void printHuffmanList(FILE* output, pNode root) {
    pNode nodelist[LIST_SIZE] = { NULL };
    int index;
    int listcount = 0;
    /*将Huffman树转换成Huffman表*/
    convertTreeToList(root, nodelist);
    listcount = trimNodeList(nodelist, LIST_SIZE);
    fprintf(output, "    *******************************\n");
    fprintf(output, "        # Print Huffman Code List #\n");
    fprintf(output, "\n");
    fprintf(output, "                       Total : %d\n", listcount);
    fprintf(output, "\n");
    fprintf(output, "     %-7s%-10s%-10s%s\n", "ASCII", "Char", "Weight", "Code");
    for (index = 0; index < listcount; ++index) {
        pNode node = nodelist[index];
        Byte ch = (Byte)node->data;
        if (canShowChar((char)ch)) {
            /*可显示字符的输出*/
            fprintf(output, "     %-7d%-10c%-10d%s\n", ch, ch, node->weight, node->code);
        } else {
            /*不可显示字符的输出*/
            fprintf(output, "     %-7d%-10s%-10d%s\n", ch, "NOShow", node->weight, node->code);
        }
    }
    printf("\n");
}

/*统计输入的字符权重*/
void contUserInputTimes(int times[]) {
    int index, count;
    printf("    *******************************\n");
    printf("        # Input data to test #\n");
    printf("\n");
    printf("        Number of input nodes : ");
    scanf("%d", &count);
    printf("        Enter the letters and weights for each node : \n");
    for (index = 0; index < count; ++index) {
        char str[128] = { 0 };
        int weight = 0;
        printf("        Char   : ");
        scanf("%s", str);
        printf("        Weight : ");
        scanf("%d", &weight);
        times[(int)str[0]] = weight;
    }
}

/*输入数据构建Huffman树选项*/
pNode inputDataToBuildHuffmanTreeOption() {
    int times[LIST_SIZE] = { 0 };
    contUserInputTimes(times);
    return buildHuffmanTree(times);
}

/*获取输入选项*/
int inputOption(int begin, int end) {
    do {
        int opt;
        if (scanf("%d", &opt) == 1) {
            if (opt >= begin && opt <= end) {
                return opt;
            } else {
                printf("       error : The input range should be between %d and %d.\n", begin, end);
            }
        } else {
            printf("       error : Please enter integer type.\n");
            /*清空缓冲区*/
            setbuf(stdin, NULL);
        }
    } while (1);
}


/*========================================================================
    FILE read & write functions.
 */

/*检测文件类型*/
int getFileType(const char filename[]) {
    int type = FILE_TYPE_NULL;
    FILE* input = fopen(filename, "rb");
    if (input) {
        char buffer[2] = { 0 };
        type = FILE_TYPE_ENCODE;
        if (fread(buffer, 2, 1, input) == 1) {
            if (buffer[0] == FILE_HEADER_FLAG[0] && buffer[1] == FILE_HEADER_FLAG[1]) {
                type = FILE_TYPE_DECODE;
            }
        }
        fclose(input);
    }
    return type;
}

/*读文件选项*/
void readFileOption() {
    char filename[PATH_MAX] = { 0 };
    printf("    *******************************\n");
    printf("        # Read File #\n");
    printf("\n");
    printf("       input file name : ");
    scanf("%s", filename);
    printf("\n");
    switch (getFileType(filename)) {
    case FILE_TYPE_ENCODE:
        encodeFileOption(filename);
        break;
    case FILE_TYPE_DECODE:
        decodeFileOption(filename);
        break;
    default:
        printf("       error : Failed to read file.\n");
    }
}

/*写入文件头信息（文件头含文件头标识和字符权重集）*/
int writeFileHeader(pNode root, FILE* output) {
    pNode nodelist[LIST_SIZE] = { NULL };
    Byte total = 0;
    int index;
    /*写入文件头标识*/
    fwrite(FILE_HEADER_FLAG, 2, 1, output);
    convertTreeToList(root, nodelist);
    /*
    * 为节省空间字符集总量存储为1个字节
    * 总量1用0表示，总量256用255表示
    * 所以总量 - 1
    */
    total = (Byte)(trimNodeList(nodelist, LIST_SIZE) - 1);
    /*写入字符集总数*/
    fwrite(&total, sizeof(total), 1, output);
    /*写入每个字符以及权重*/
    for (index = 0; index <= total; ++index) {
        pNode node = nodelist[index];
        Byte byte = (Byte)node->data;
        fwrite(&byte, sizeof(byte), 1, output);
        fwrite(&node->weight, sizeof(node->weight), 1, output);
    }
    /*返回写入的文件头总字节数*/
    return (total * 5 + 1 + 2);
}

/*读取文件头信息（读取字符权重集）*/
void readFileHeader(FILE* input, int times[]) {
    Byte total;
    int index;
    /*跳过文件头*/
    fseek(input, 2, SEEK_CUR);
    fread(&total, sizeof(total), 1, input);
    for (index = 0; index <= total; ++index) {
        Byte byte;
        int weight;
        fread(&byte, sizeof(byte), 1, input);
        fread(&weight, sizeof(weight), 1, input);
        times[byte] = weight;
    }
}

/*对文件进行编码*/
void toEncode(pNode root, FILE* input) {
    char filename[PATH_MAX] = { 0 };
    FILE* output = NULL;
    printf("    *******************************\n");
    printf("        # Write File #\n");
    printf("\n");
    printf("       write file name : ");
    scanf("%s", filename);
    output = fopen(filename, "wb");
    if (output) {
        int rawsize = countHuffmanTreeWeightTotal(root);
        int header_size = writeFileHeader(root, output);
        if (input) {
            struct timeval startstamp = startTimestamp();
            int compressedsize = encodeFileData(root, input, output);
            printf("\n");
            printf("       Elapsed Time      : %lf msc\n", endTimestamp(startstamp));
            printf("       Character Set     : %d Bytes\n", header_size);
            printf("       Compressed Data   : %d Bytes\n", compressedsize);
            printf("       Raw Data          : %d Bytes\n", rawsize);
            printf("       Compression Ratio : %.2lf%%\n", (compressedsize / (double)rawsize) * 100);
            printf("\n");
            printf("       Execute successfully.\n");
        } else {
            printf("       error : Failed to read file.\n");
        }
        fclose(output);
    } else {
        printf("       error : Failed to write file.\n");
    }
    printf("\n");
}


/*文件编码选项*/
void encodeFileOption(const char filename[]) {
    FILE* input = fopen(filename, "rb");
    if (input) {
        pNode root = buildHuffmanTreeFromFile(input);
        if (root) {
            do {
                int option;
                printf("    *******************************\n");
                printf("        # Encode File #\n");
                printf("\n");
                printf("       1 > Print Huffman Tree\n");
                printf("       2 > Print Huffman Code List\n");
                printf("       3 > Encode File\n");
                printf("       0 > Back\n");
                printf("\n");
                option = inputOption(0, 3);
                if (option == 0) break;
                switch (option) {
                case 1:
                    printHuffmanTree(stdout, root);
                    break;
                case 2:
                    printHuffmanList(stdout, root);
                    break;
                case 3:
                    /*重置文件指针到文件头*/
                    fseek(input, 0, SEEK_SET);
                    toEncode(root, input);
                    break;
                }
            } while (1);
            destroyHuffmanTree(root);
        } else {
            printf("       error : Failed to build Huffman tree.\n");
        }
        fclose(input);
    } else {
        printf("       error : Failed to read file.\n");
    }
}




/*文件解码选项*/
void decodeFileOption(const char filename[]) {
    FILE* input = fopen(filename, "rb");
    if (input) {
        int tell = 0;
        int times[LIST_SIZE] = { 0 };
        readFileHeader(input, times);
        /*记录文件数据区位置*/
        tell = (int)ftell(input);
        pNode root = buildHuffmanTree(times);
        if (root) {
            do {
                int option;
                printf("    *******************************\n");
                printf("        # Decode File #\n");
                printf("\n");
                printf("       1 > Print Huffman Tree\n");
                printf("       2 > Print Huffman Code List\n");
                printf("       3 > Decode File\n");
                printf("       0 > Back\n");
                printf("\n");
                option = inputOption(0, 3);
                if (option == 0) break;
                switch (option) {
                case 1:
                    printHuffmanTree(stdout, root);
                    break;
                case 2:
                    printHuffmanList(stdout, root);
                    break;
                case 3:
                    /*将文件指针定位到数据区*/
                    fseek(input, tell, SEEK_SET);
                    toDecode(root, input);
                    break;
                }
            } while (1);
            destroyHuffmanTree(root);
        } else {
            printf("       error : Failed to build Huffman tree.\n");
        }
        fclose(input);
    } else {
        printf("       error : Failed to read file.\n");
    }
}



/*测试选项*/
void inputDataToTestOption() {
    pNode root = inputDataToBuildHuffmanTreeOption();
    if (root) {
        do {
            int option;
            printf("    *******************************\n");
            printf("\n");
            printf("       1 > Print Huffman Tree\n");
            printf("       2 > Print Huffman Code List\n");
            printf("       0 > Back\n");
            printf("\n");
            option = inputOption(0, 2);
            if (option == 0)break;
            switch (option) {
            case 1:
                printHuffmanTree(stdout, root);
                break;
            case 2:
                printHuffmanList(stdout, root);
                break;
            }
        } while (1);
        destroyHuffmanTree(root);
    }
}

/*Huffman功能演示*/
void huffmanDemo() {
    do {
        int option;
        printf("    *******************************\n");
        printf("        # Huffman Tree Demo #\n");
        printf("\n");
        printf("       1 > Read file\n");
        printf("       2 > Input data to test\n");
        printf("       0 > Quit\n");
        printf("\n");
        option = inputOption(0, 2);
        if (option == 0) break;
        switch (option) {
        case 1:
            readFileOption();
            break;
        case 2:
            inputDataToTestOption();
            break;
        }
    } while (1);
}


