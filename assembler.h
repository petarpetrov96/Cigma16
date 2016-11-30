#include<string.h>
#include<QString>
#include "emulator.h"
#ifndef ASSEMBLER_H
#define ASSEMBLER_H


class Assembler
{
public:
    Assembler(const char* data, int length);
    ~Assembler();
    void assemble();
    QString readAssembly();
protected:
    void removeTabs(char* buffer, int length);
    void removeSpaces(char* buffer, int& length);
    void fixNewLineCharacters(char* buffer, int& length);
    void removeComments(char* buffer, int& length);
    void removeEmptyLines(char* buffer, int& length, int* actualLine);
    void printError(int result,int lineError,char* fp);
    int checkRRR3(char* arg, int argl,char& reg1,char& reg2,char& reg3);
    int checkRRR2(char* arg, int argl,char& reg1,char& reg2);
    int checkRX1(char* arg,int argl,char& reg1,unsigned short& addr);
    int checkRX2(char* arg,int argl,char& reg1,char& reg2,unsigned short& addr);
    bool translateInstruction(char* instrName,int instrLength,char* instrArgs,int instrArgsLength,char* fp,int lineError);
    char* buffer;
    int bufferLength;
    char** labelList;
    unsigned short* labelAddresses;
    int labelSize=0;
    char* fp;
    virtual int getInstructionType(char* instrName)=0;
    virtual char getRRR(char* instrName)=0;
    virtual char getRX(char* instrName)=0;
    virtual char getJump(char* instrName)=0;
};

#endif // ASSEMBLER_H
