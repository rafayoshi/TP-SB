////////////////////////////////////////////////////////////////////////////////
///
/// File:       assembler.h
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

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

///////////////////////////////// DEFINITIONS //////////////////////////////////

#define MAX_NUMBER_LABEL 1000

#define MAX_LABEL_SIZE 20

#define WIDTH 8

#define DEPTH 256

#define MAX_LINE_SIZE 200

#define MAX_OP_SIZE 17

////////////////////////////// TYPES AND STRUCTS ///////////////////////////////

typedef struct Label{
    char label[MAX_LABEL_SIZE];
    int address;
} LabelType;

////////////////////////////////// FUNCTIONS ///////////////////////////////////

int Assemble(char const *inputName, char const *outputName);

#endif  /* _ASSEMBLER_H_ */
