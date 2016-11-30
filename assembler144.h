#include "assembler.h"
#ifndef ASSEMBLER144_H
#define ASSEMBLER144_H


class Assembler144 : public Assembler
{
public:
    explicit Assembler144(const char* data,int length);
private:
    int getInstructionType(char* instrName) override;
    char getRRR(char* instrName) override;
    char getRX(char* instrName) override;
    char getJump(char* instrName) override;
};

#endif // ASSEMBLER144_H
