////////////////////////////////////////////////////////////////////////////////
///
/// File:       assembler.c
/// Date:       2017-05-04
///
/// Authors:    Antonio Cortes Rodrigues
///             Marcelo Luiz Harry Diniz Lemos
///             Pedro Henrique Martins Brito Aguiar
///
/// Description:
///     [file description]
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////// INCLUDED LIBRARIES //////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"

////////////////////////////////// FUNCTIONS ///////////////////////////////////

void InitializeOutput(FILE * output) {
    fprintf(output, "WIDTH = %d;\nDEPTH = %d;\n", WIDTH, DEPTH);
    fprintf(output, "ADDRESS_RADIX = BIN;\nDATA_RADIX = BIN;\nCONTENT\nBEGIN\n");
}

void FinalizeOutput(FILE * output) {
    fprintf(output, "END;\n");
}

void GenerateLookupTable(FILE *input, LabelType *lookupTable, int *totalLabel) {
    int currentPosition;
    char currentLabel[MAX_LABEL_SIZE];
    char buffer[MAX_LINE_SIZE];
    char op[MAX_OP_SIZE];
    int isData;
    int dataSize;

    currentPosition = 0;
    while(!feof(input)) {
        isData = 0;
        fscanf(input, "%[^\n]\n", buffer);
        if(buffer[0] == '_') {
            sscanf(buffer, "%[^:]: %s", currentLabel, op);
            strcpy(lookupTable[*totalLabel].label, currentLabel);
            lookupTable[*totalLabel].address = currentPosition;
            *totalLabel += 1;
            if(!strcmp(op, ".data")) {
                isData = 1;
                sscanf(buffer, "%[^:]: %s %d", currentLabel, op, &dataSize);
            }
        }
        if(isData) {
            currentPosition += dataSize;
        } else {
            currentPosition += 2;
        }
    }
    fseek(input, 0, SEEK_SET);
}

char * RegisterToBinary(char * reg) {
    switch (reg[1]) {
        case '0':
            return "000";
        case '1':
            return "001";
        case '2':
            return "010";
        case '3':
            return "011";
        case '4':
            return "100";
        case '5':
            return "101";
        case '6':
            return "110";
        case '7':
            return "111";
        default:
            return "-1";
    }
}

void CarryIn(char * bin, int i) {
    if(i < 0)
        return;
    if(bin[i] == '0') {
        bin[i] = '1';
        return;
    } else {
        bin[i] = '0';
        CarryIn(bin, i - 1);
    }
}

void SignedBinary(char * bin) {
    int i;

    for(i = 0; i < 8; i++) {
        if(bin[i] == '0')
            bin[i] = '1';
        else
            bin[i] = '0';
    }
    CarryIn(bin, 7);
}

void DecimalToBinary(int number, char * bin) {
    int i;
    int isNegative;

    if(number < 0) {
        isNegative = 1;
        number *= -1;
    } else {
        isNegative = 0;
    }
    memset(bin, 0, 9);
    bin[8] = '\0';
    for(i = 7; i >= 0 ; i--) {
        bin[i] = (number % 2) + '0';
        number /= 2;
    }
    if(isNegative)
        SignedBinary(bin);
}

int GetAddress(char * label, LabelType * lookupTable, int totalLabel) {
    int i;

    for(i = 0; i < totalLabel; i++) {

        if(!strcmp(lookupTable[i].label, label))
            return lookupTable[i].address;
    }
    printf("Label not found %s\n", label);
    return -1;
}

void GenerateBinary(FILE *input, FILE *output, LabelType *lookupTable, int totalLabel) {
    int currentInstruction;
    char inst[9];
    int hasLabel;
    char ignore[MAX_LABEL_SIZE];
    char label[MAX_LABEL_SIZE];
    char buffer[MAX_LINE_SIZE];
    char op[MAX_OP_SIZE];
    char reg1[MAX_OP_SIZE];
    char reg2[MAX_OP_SIZE];
    char addr[MAX_OP_SIZE];
    int isData;
    int dataSize;
    int value;
    char binValue[MAX_OP_SIZE];
    int i;

    currentInstruction = 0;
    while(!feof(input)) {
        isData = 0;
        DecimalToBinary(currentInstruction,inst);
        fprintf(output, "%s : ", inst);
        fscanf(input, "%[^\n]\n", buffer);
        if(buffer[0] == '_') {
            hasLabel = 1;
        } else {
            hasLabel = 0;
        }
        if(hasLabel){
            sscanf(buffer, "%[^:]: %s", ignore, op);
        } else {
            sscanf(buffer, "%s", op);
        }
        if(!strcmp(op, "exit")) {
            fprintf(output, "00000000;\n");
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "loadi")) {
            fprintf(output, "00001");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            if(!strcmp(label,"IO")){
                DecimalToBinary(254, addr);
            } else if(label[0] < '0' || label[0] > '9') {
                DecimalToBinary(GetAddress(label, lookupTable, totalLabel), addr);
            } else {
                DecimalToBinary(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "storei")) {
            fprintf(output, "00010");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            if(!strcmp(label,"IO")){
                DecimalToBinary(254, addr);
            } else if(label[0] < '0' || label[0] > '9') {
                DecimalToBinary(GetAddress(label, lookupTable, totalLabel), addr);
            } else {
                DecimalToBinary(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "add")) {
            fprintf(output, "00011");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
        } else if(!strcmp(op, "subtract")) {
            fprintf(output, "00100");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
        } else if(!strcmp(op, "multiply")) {
            fprintf(output, "00101");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
        } else if(!strcmp(op, "divide")) {
            fprintf(output, "00110");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
        } else if(!strcmp(op, "jump")) {
            fprintf(output, "00111000;\n");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, label);
            else
                sscanf(buffer, "%s %s", op, label);
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            if(label[0] < '0' || label[0] > '9') {
                DecimalToBinary(GetAddress(label, lookupTable, totalLabel), addr);
            } else {
                DecimalToBinary(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "jmpz")) {
            fprintf(output, "01000");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            if(label[0] < '0' || label[0] > '9') {
                DecimalToBinary(GetAddress(label, lookupTable, totalLabel), addr);
            } else {
                DecimalToBinary(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "jmpn")) {
            fprintf(output, "01001");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            if(label[0] < '0' || label[0] > '9') {
                DecimalToBinary(GetAddress(label, lookupTable, totalLabel), addr);
            } else {
                DecimalToBinary(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "move")) {
            fprintf(output, "01010");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
        } else if(!strcmp(op, "load")) {
            fprintf(output, "01011");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
      fprintf(output, "%s : ", inst);
            if(!strcmp(reg2,"IO")){
                DecimalToBinary(254, addr);
                fprintf(output, "%s;\n", addr);
            } else {
                fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
            }
        } else if(!strcmp(op, "store")) {
            fprintf(output, "01100");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            if(!strcmp(reg2,"IO")){
                DecimalToBinary(254, addr);
                fprintf(output, "%s;\n", addr);
            } else {
                fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
            }
        } else if(!strcmp(op, "loadc")) {
            fprintf(output, "01101");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
                DecimalToBinary(atoi(label), addr);
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "clear")) {
            fprintf(output, "01110");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, reg1);
            else
                sscanf(buffer, "%s %s", op, reg1);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "negate")) {
            fprintf(output, "01111");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegisterToBinary(reg2));
        } else if(!strcmp(op, "push")) {
            fprintf(output, "10000");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, reg1);
            else
                sscanf(buffer, "%s %s", op, reg1);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "pop")) {
            fprintf(output, "10001");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, reg1);
            else
                sscanf(buffer, "%s %s", op, reg1);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "addi")) {
            fprintf(output, "10010");
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegisterToBinary(reg1));
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            DecimalToBinary(atoi(label), addr);
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "call")) {
            fprintf(output, "10011000;\n");
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            if(hasLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, label);
            else
                sscanf(buffer, "%s %s", op, label);
            DecimalToBinary(GetAddress(label, lookupTable, totalLabel), addr);
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "return")) {
            fprintf(output, "10100000;\n");
            DecimalToBinary(currentInstruction + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, ".data")) {
            isData = 1;
            sscanf(buffer, "%[^:]: %s %d %d", ignore, op, &dataSize, &value);
            for(i = 0; i < dataSize - 1; i++) {
                fprintf(output, "00000000;\n");
                DecimalToBinary(currentInstruction + i + 1,inst);
                fprintf(output, "%s : ", inst);
            }
            DecimalToBinary(value, binValue);
            fprintf(output, "%s;\n", binValue);
        }
        if(isData)
            currentInstruction += dataSize;
        else
            currentInstruction += 2;
    }
    if(currentInstruction < 255) {
        DecimalToBinary(currentInstruction,inst);
        fprintf(output, "[%s..11111111] : 00000000;\n", inst);
    } else if(currentInstruction == 255) {
        fprintf(output, "11111111 : 00000000;\n");
    }
}

int Assemble(char const *inputName, char const *outputName) {
    LabelType lookupTable[MAX_NUMBER_LABEL];    // Label table
    int totalLabel;
    FILE * input;
    FILE * output;

    // Open files
    input = fopen(inputName, "rt");
    output = fopen(outputName, "wt");
    if(input == NULL) {
        printf("Error opening input file!");
        return(-1);
    }
    if(output == NULL) {
        printf("Error opening output file!");
        return(-1);
    }

    totalLabel = 0;
    GenerateLookupTable(input, lookupTable, &totalLabel);
    InitializeOutput(output);
    GenerateBinary(input, output, lookupTable, totalLabel);
    FinalizeOutput(output);

    fclose(input);
    fclose(output);
    return 0;
}
