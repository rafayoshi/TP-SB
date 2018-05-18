////////////////////////////// INCLUDED LIBRARIES //////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LABEL_NUM 1000

#define MAX_LABEL 20

#define WIDTH 8

#define DEPTH 128

#define MAX_OP_SIZE 17

#define MAX_LINHA 200

#define MAX_OPERACAO 17

typedef struct Label{
    char label[MAX_LABEL];
    int endereco;
} TipoLabel;

int Assemble(char const *nomeInput, char const *nomeOutput);


void GeraTabela(FILE *input, TipoLabel *tabela, int *totalLabel) {
    int posicaoAtual = 0;
    char labelAtual[MAX_LABEL];
    char buffer[MAX_LINHA];
    char op[MAX_OPERACAO];
    bool temDados = false;
    int tamanhoDados = 0;

    posicaoAtual = 0;
    while(!feof(input)) { //lemos o arquivo de input
        temDados = false;
        //le a linha
        fscanf(input, "%[^\n]\n", buffer); 
        //aqui, encontramos uma label
        if(buffer[0] == '_') {
            //pega o nome da label e o nome da primeira operaçao
            sscanf(buffer, "%[^:]: %s", labelAtual, op);
            //insere a label na tabela
            strcpy(tabela[*totalLabel].label, labelAtual);
            //insere o endereco da label na tabela
            tabela[*totalLabel].endereco = posicaoAtual;
            //move o leitor para a proxima label a ser encontrada
            *totalLabel++;
            //caso a label nao indique nada no campo de dados,
            //DESCOBRIR o que o codigo abaixo esta fazendo
            if(!strcmp(op, ".data")) {
                temDados = true;
                sscanf(buffer, "%[^:]: %s %d", labelAtual, op, &tamanhoDados);
            }
        }
        //DESCOBRIR o que o codigo abaixo esta fazendo
        if(temDados) {
            posicaoAtual += tamanhoDados;
        } else {
            posicaoAtual += 2;
        }
    }
    //volta o leitor do arquivo lá pra cima
    //isso nos evita de fechar e abrir o arquivo de novo
    fseek(input, 0, SEEK_SET);
}

char * RegistradorParaBinario(char * reg) {
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

void ResolveSinal(char * bin) {
    int i;

    for(i = 0; i < 8; i++) {
        if(bin[i] == '0')
            bin[i] = '1';
        else
            bin[i] = '0';
    }
    CarryIn(bin, 7);
}

void DecimalParaBinario(int numero, char * bin) {
    int i;
    int negativo;

    if(numero < 0) {
        negativo = 1;
        numero *= -1;
    } else {
        negativo = 0;
    }
    memset(bin, 0, 9);
    bin[8] = '\0';
    for(i = 7; i >= 0 ; i--) {
        bin[i] = (numero % 2) + '0';
        numero /= 2;
    }
    if(negativo)
        ResolveSinal(bin);
}

int Getendereco(char * label, TipoLabel * tabela, int totalLabel) {
    int i;

    for(i = 0; i < totalLabel; i++) {

        if(!strcmp(tabela[i].label, label))
            return tabela[i].endereco;
    }
    printf("Label not found %s\n", label);
    return -1;
}

void GeraBinario(FILE *input, FILE *output, TipoLabel *tabela, int totalLabel) {
    int instAtual; //salva o numero index da instrucao em decimal
    char inst[9];  //salva o numero da instrução em binário
    bool temLabel; //verifica se a linha encontrada se trata de uma label ou não
    char ignore[MAX_LABEL];// nome da label e o ignora
    char label[MAX_LABEL]; // nome da label
    char buffer[MAX_LINHA];// a linha inteira de comando
    char op[MAX_OPERACAO]; //operação
    char reg1[MAX_OPERACAO];
    char reg2[MAX_OPERACAO];
    char addr[MAX_OPERACAO];
    bool temDados; //verifica
    int tamanhoDados;
    int valor;
    char binvalor[MAX_OPERACAO];
    int i;

    instAtual = 0;
    while(!feof(input)) {
        //imprimimos ADDRESS RADIX
        temDados = false;
        DecimalParaBinario(instAtual,inst);
        fprintf(output, "%s : ", inst);
        //lemos o codigo
        fscanf(input, "%[^\n]\n", buffer);
        if(buffer[0] == '_') {
            temLabel = true;
        } else {
            temLabel = false;
        }
        //se tiver uma label, ignoramos
        if(temLabel){
            sscanf(buffer, "%[^:]: %s", ignore, op);
        } else {
            sscanf(buffer, "%s", op);
        }
        if(!strcmp(op, "exit")) {
            fprintf(output, "00000000;\n");
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "loadi")) {
            fprintf(output, "00001");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            if(!strcmp(label,"IO")){
                DecimalParaBinario(254, addr);
            } else if(label[0] < '0' || label[0] > '9') {
                DecimalParaBinario(Getendereco(label, tabela, totalLabel), addr);
            } else {
                DecimalParaBinario(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "storei")) {
            fprintf(output, "00010");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            if(!strcmp(label,"IO")){
                DecimalParaBinario(254, addr);
            } else if(label[0] < '0' || label[0] > '9') {
                DecimalParaBinario(Getendereco(label, tabela, totalLabel), addr);
            } else {
                DecimalParaBinario(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "add")) {
            fprintf(output, "00011");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
        } else if(!strcmp(op, "subtract")) {
            fprintf(output, "00100");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
        } else if(!strcmp(op, "multiply")) {
            fprintf(output, "00101");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
        } else if(!strcmp(op, "divide")) {
            fprintf(output, "00110");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
        } else if(!strcmp(op, "jump")) {
            fprintf(output, "00111000;\n");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, label);
            else
                sscanf(buffer, "%s %s", op, label);
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            if(label[0] < '0' || label[0] > '9') {
                DecimalParaBinario(Getendereco(label, tabela, totalLabel), addr);
            } else {
                DecimalParaBinario(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "jmpz")) {
            fprintf(output, "01000");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            if(label[0] < '0' || label[0] > '9') {
                DecimalParaBinario(Getendereco(label, tabela, totalLabel), addr);
            } else {
                DecimalParaBinario(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "jmpn")) {
            fprintf(output, "01001");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            if(label[0] < '0' || label[0] > '9') {
                DecimalParaBinario(Getendereco(label, tabela, totalLabel), addr);
            } else {
                DecimalParaBinario(atoi(label), addr);
            }
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "move")) {
            fprintf(output, "01010");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
        } else if(!strcmp(op, "load")) {
            fprintf(output, "01011");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
      fprintf(output, "%s : ", inst);
            if(!strcmp(reg2,"IO")){
                DecimalParaBinario(254, addr);
                fprintf(output, "%s;\n", addr);
            } else {
                fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
            }
        } else if(!strcmp(op, "store")) {
            fprintf(output, "01100");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            if(!strcmp(reg2,"IO")){
                DecimalParaBinario(254, addr);
                fprintf(output, "%s;\n", addr);
            } else {
                fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
            }
        } else if(!strcmp(op, "loadc")) {
            fprintf(output, "01101");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
                DecimalParaBinario(atoi(label), addr);
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "clear")) {
            fprintf(output, "01110");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, reg1);
            else
                sscanf(buffer, "%s %s", op, reg1);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "negate")) {
            fprintf(output, "01111");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, reg2);
            else
                sscanf(buffer, "%s %s %s", op, reg1, reg2);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "%s00000;\n", RegistradorParaBinario(reg2));
        } else if(!strcmp(op, "push")) {
            fprintf(output, "10000");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, reg1);
            else
                sscanf(buffer, "%s %s", op, reg1);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "pop")) {
            fprintf(output, "10001");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, reg1);
            else
                sscanf(buffer, "%s %s", op, reg1);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, "addi")) {
            fprintf(output, "10010");
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s %s", ignore, op, reg1, label);
            else
                sscanf(buffer, "%s %s %s", op, reg1, label);
            fprintf(output, "%s;\n", RegistradorParaBinario(reg1));
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            DecimalParaBinario(atoi(label), addr);
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "call")) {
            fprintf(output, "10011000;\n");
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            if(temLabel)
                sscanf(buffer, "%[^:]: %s %s", ignore, op, label);
            else
                sscanf(buffer, "%s %s", op, label);
            DecimalParaBinario(Getendereco(label, tabela, totalLabel), addr);
            fprintf(output, "%s;\n", addr);
        } else if(!strcmp(op, "return")) {
            fprintf(output, "10100000;\n");
            DecimalParaBinario(instAtual + 1,inst);
            fprintf(output, "%s : ", inst);
            fprintf(output, "00000000;\n");
        } else if(!strcmp(op, ".data")) {
            temDados = 1;
            sscanf(buffer, "%[^:]: %s %d %d", ignore, op, &tamanhoDados, &valor);
            for(i = 0; i < tamanhoDados - 1; i++) {
                fprintf(output, "00000000;\n");
                DecimalParaBinario(instAtual + i + 1,inst);
                fprintf(output, "%s : ", inst);
            }
            DecimalParaBinario(valor, binvalor);
            fprintf(output, "%s;\n", binvalor);
        }
        if(temDados)
            instAtual += tamanhoDados;
        else
            instAtual += 2;
    }
    if(instAtual < 255) {
        DecimalParaBinario(instAtual,inst);
        fprintf(output, "[%s..11111111] : 00000000;\n", inst);
    } else if(instAtual == 255) {
        fprintf(output, "11111111 : 00000000;\n");
    }
}

int Assemble(char const *nomeInput, char const *nomeOutput) {
    TipoLabel tabela[MAX_LABEL_NUM]; //abre uma tabela de labels
    int totalLabel; //indica a posicao de leitura de uma label na tabela
    //eh como se fosse um "i" do laço for
    FILE * input; //arquivo de entrada
    FILE * output; //arquivo de saida

    // Abrindo os arquivos
    input = fopen(nomeInput, "rt");
    output = fopen(nomeOutput, "wt");
    if(input == NULL) {
        printf("Error opening input file!");
        return(-1);
    }
    if(output == NULL) {
        printf("Error opening output file!");
        return(-1);
    }

    totalLabel = 0;
    //gera a tabela
    GeraTabela(input, tabela, &totalLabel);
        fprintf(output, "DEPTH = %d;\nWIDTH = %d;\n", DEPTH, WIDTH);
        fprintf(output, "ADDRESS_RADIX = HEX;\nDATA_RADIX = BIN;\nCONTENT\nBEGIN\n");
    GeraBinario(input, output, tabela, totalLabel);
        fprintf(output, "END;\n");

    fclose(input);
    fclose(output);
    return 0;
}
