#include<string.h>
#include "emulator.h"
#ifndef ASSEMBLER_H
#define ASSEMBLER_H


class Assembler
{
public:
    Assembler(const char* data, int length);  //constructor
    ~Assembler();  //destructor
    void assemble();  //function for assembling the data
    std::string readAssembly();  //function for parsing the raw assembly
protected:
    void removeTabs(char* buffer, int length);
    void removeSpaces(char* buffer, int& length);
    void fixNewLineCharacters(char* buffer, int& length);
    void removeComments(char* buffer, int& length);
    void removeEmptyLines(char* buffer, int& length, int* actualLine);
    void printError(int result,int lineError,char* fp);
    int checkRRR3(char* arg, int argl,char& reg1,char& reg2,char& reg3);  //function for validating RRR instructions with 3 arguments
    int checkRRR2(char* arg, int argl,char& reg1,char& reg2);             //function for validating RRR instructions with 2 arguments
    int checkRX1(char* arg,int argl,char& reg1,unsigned short& addr);     //function for validating RX instructions with 1 argument
    int checkRX2(char* arg,int argl,char& reg1,char& reg2,unsigned short& addr); //function for validating RX instructions with 2 arguments
    bool translateInstruction(char* instrName,char* instrArgs,int instrArgsLength,char* fp,int lineError);
    char* buffer;      //temporary memory used for parsing the assembly
    int bufferLength;  //length of the temporary memory
    char** labelList;  //list of all labels in the assembly
    unsigned short* labelAddresses; //list of the addresses of all labels
    int labelSize=0;   //count of all the addresses of all labels
    char* fp;          //temporary memory used to hold the raw assembly
    virtual int getInstructionType(char* instrName)=0;
    virtual char getRRR(char* instrName)=0;
    virtual char getRX(char* instrName)=0;
    virtual char getJump(char* instrName)=0;
};

#endif // ASSEMBLER_H
