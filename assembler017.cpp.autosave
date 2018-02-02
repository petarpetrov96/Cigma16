#include "assembler017.h"

/*
 * This file contains version-specific information about Sigma16 0.1.7
 * It just follows the original version of the architecture
 */

Assembler017::Assembler017(const char* data, int length) : Assembler(data,length) {
}

char Assembler017::getRRR(char *instrName) {
    if(!strcmp(instrName,"add")) return '0';
    if(!strcmp(instrName,"sub")) return '1';
    if(!strcmp(instrName,"mul")) return '2';
    if(!strcmp(instrName,"div")) return '3';
    if(!strcmp(instrName,"cmplt")) return '4';
    if(!strcmp(instrName,"cmpeq")) return '5';
    if(!strcmp(instrName,"cmpgt")) return '6';
    if(!strcmp(instrName,"inv")) return '7';
    if(!strcmp(instrName,"and")) return '8';
    if(!strcmp(instrName,"or")) return '9';
    if(!strcmp(instrName,"xor")) return 'a';
    if(!strcmp(instrName,"shiftl")) return 'b';
    if(!strcmp(instrName,"shiftr")) return 'c';
    if(!strcmp(instrName,"trap")) return 'd';
    return ' ';
}

char Assembler017::getRX(char *instrName) {
    if(!strcmp(instrName,"lea")) return '0';
    if(!strcmp(instrName,"load")) return '1';
    if(!strcmp(instrName,"store")) return '2';
    if(!strcmp(instrName,"jump")) return '3';
    if(!strcmp(instrName,"jumpf")) return '4';
    if(!strcmp(instrName,"jumpt")) return '5';
    if(!strcmp(instrName,"jal")) return '6';
    return ' ';
}

char Assembler017::getJump(char *instrName) {
    if(!strcmp(instrName,"jump"))
        return '0';
    return ' ';
}

int Assembler017::getInstructionType(char *instrName) {
    if(!strcmp(instrName,"add") || !strcmp(instrName,"sub") || !strcmp(instrName,"mul") || !strcmp(instrName,"div") || !strcmp(instrName,"cmplt") || !strcmp(instrName,"cmpeq") || !strcmp(instrName,"cmpgt") || !strcmp(instrName,"inv") || !strcmp(instrName,"and") || !strcmp(instrName,"or") || !strcmp(instrName,"xor") || !strcmp(instrName,"shiftl") || !strcmp(instrName,"shiftr") || !strcmp(instrName,"trap"))
        return 1;
    if(!strcmp(instrName,"lea") || !strcmp(instrName,"load") || !strcmp(instrName,"store") || !strcmp(instrName,"jumpf") || !strcmp(instrName,"jumpt") || !strcmp(instrName,"jal"))
        return 3;
    if(!strcmp(instrName,"jump"))
        return 4;
    if(!strcmp(instrName,"data"))
        return 5;
    return -1;
}
