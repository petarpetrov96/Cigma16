#include "assembler144.h"

Assembler144::Assembler144(const char* data, int length) : Assembler(data,length) {
}

char Assembler144::getRRR(char *instrName) {
    if(!strcmp(instrName,"add")) return '0';
    if(!strcmp(instrName,"sub")) return '1';
    if(!strcmp(instrName,"mul")) return '2';
    if(!strcmp(instrName,"div")) return '3';
    if(!strcmp(instrName,"cmp")) return '4';
    if(!strcmp(instrName,"inv")) return '5';
    if(!strcmp(instrName,"and")) return '6';
    if(!strcmp(instrName,"or")) return '7';
    if(!strcmp(instrName,"xor")) return '8';
    if(!strcmp(instrName,"shiftl")) return '9';
    if(!strcmp(instrName,"shiftr")) return 'a';
    if(!strcmp(instrName,"trap")) return 'b';
    return ' ';
}

char Assembler144::getRX(char *instrName) {
    if(!strcmp(instrName,"lea")) return '0';
    if(!strcmp(instrName,"load")) return '1';
    if(!strcmp(instrName,"store")) return '2';
    if(!strcmp(instrName,"jump")) return '3';
    if(!strcmp(instrName,"jumplt")) return '3';
    if(!strcmp(instrName,"jumple")) return '3';
    if(!strcmp(instrName,"jumpeq")) return '3';
    if(!strcmp(instrName,"jumpne")) return '3';
    if(!strcmp(instrName,"jumpge")) return '3';
    if(!strcmp(instrName,"jumpgt")) return '3';
    if(!strcmp(instrName,"jumpovf")) return '3';
    if(!strcmp(instrName,"jumpnovf")) return '3';
    if(!strcmp(instrName,"jumpco")) return '3';
    if(!strcmp(instrName,"jumpnco")) return '3';
    if(!strcmp(instrName,"jal")) return '4';
    return ' ';
}

char Assembler144::getJump(char *instrName) {
    if(!strcmp(instrName,"jump")) return '0';
    if(!strcmp(instrName,"jumplt")) return '1';
    if(!strcmp(instrName,"jumple")) return '2';
    if(!strcmp(instrName,"jumpeq")) return '3';
    if(!strcmp(instrName,"jumpne")) return '4';
    if(!strcmp(instrName,"jumpge")) return '5';
    if(!strcmp(instrName,"jumpgt")) return '6';
    if(!strcmp(instrName,"jumpovf")) return '7';
    if(!strcmp(instrName,"jumpnovf")) return '8';
    if(!strcmp(instrName,"jumpco")) return '9';
    if(!strcmp(instrName,"jumpnco")) return 'a';
    return ' ';
}

int Assembler144::getInstructionType(char *instrName) {
    if(!strcmp(instrName,"add") || !strcmp(instrName,"sub") || !strcmp(instrName,"mul") || !strcmp(instrName,"div") || !strcmp(instrName,"inv") || !strcmp(instrName,"and") || !strcmp(instrName,"or") || !strcmp(instrName,"xor") || !strcmp(instrName,"shiftl") || !strcmp(instrName,"shiftr") || !strcmp(instrName,"trap"))
        return 1;
    if(!strcmp(instrName,"cmp"))
        return 2;
    if(!strcmp(instrName,"lea") || !strcmp(instrName,"load") || !strcmp(instrName,"store") || !strcmp(instrName,"jal"))
        return 3;
    if(!strcmp(instrName,"jump") || !strcmp(instrName,"jumplt") || !strcmp(instrName,"jumple") || !strcmp(instrName,"jumpeq") || !strcmp(instrName,"jumpne") || !strcmp(instrName,"jumpge") || !strcmp(instrName,"jumpgt") || !strcmp(instrName,"jumpovf") || !strcmp(instrName,"jumpnovf") || !strcmp(instrName,"jumpco") || !strcmp(instrName,"jumpnco"))
        return 4;
    if(!strcmp(instrName,"data"))
        return 5;
    return -1;
}
