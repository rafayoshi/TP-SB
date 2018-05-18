////////////////////////////////////////////////////////////////////////////////
///
/// File:       main.c
/// Date:       2017-05-04
///
/// Authors:    Antonio Cortes Rodrigues
///             Marcelo Luiz Harry Diniz Lemos
///             Pedro Henrique Martins Brito Aguiar
///
/// Description:
///     Main function of the assembler.
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////// INCLUDED LIBRARIES //////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"

///////////////////////////////////// MAIN /////////////////////////////////////

int main(int argc, char const *argv[]) {
    // Check number of arguments
    if(argc != 3) {
        printf("Error: Incompatible number of arguments!\n");
        exit(-1);
    }

    return Assemble(argv[1], argv[2]);
}
