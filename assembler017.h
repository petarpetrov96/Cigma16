#include "assembler.h"
#ifndef ASSEMBLER017_H
#define ASSEMBLER017_H


class Assembler017 : public Assembler
{
public:
    explicit Assembler017(const char* data, int length);
private:
    int getInstructionType(char* instrName) override;
    char getRRR(char* instrName) override;
    char getRX(char* instrName) override;
    char getJump(char* instrName) override;
};

#endif // ASSEMBLER_H
