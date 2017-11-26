#include<string.h>
#include "emulator.h"
#ifndef ASSEMBLER_H
#define ASSEMBLER_H


class Assembler
{
public:
    Assembler(const char* data, int length);  // Constructor
    ~Assembler();     // Destructor
    void assemble();  // Function for assembling the data
    std::string readAssembly();  // Function for parsing the raw assembly
protected:
    // Formatting functions
    void removeTabs(char* assemblyCode, int length);
    void removeSpaces(char* assemblyCode, int& length);
    void fixNewLineCharacters(char* assemblyCode, int& length);
    void removeComments(char* assemblyCode, int& length);
    void removeEmptyLines(char* assemblyCode, int& length, int* actualLine);

    // Assembler functions
    void printError(int result,int lineError,char* fp);
    int checkRRR3(char* arg, int argl,char& reg1,char& reg2,char& reg3);  //function for validating RRR instructions with 3 arguments
    int checkRRR2(char* arg, int argl,char& reg1,char& reg2);             //function for validating RRR instructions with 2 arguments
    int checkRX1(char* arg,int argl,char& reg1,unsigned short& addr);     //function for validating RX instructions with 1 argument
    int checkRX2(char* arg,int argl,char& reg1,char& reg2,unsigned short& addr); //function for validating RX instructions with 2 arguments
    bool translateInstruction(char* instrName,char* instrArgs,int instrArgsLength,char* fp,int lineError);

    // Helper functions
    int findNextCharacter(char* arg, int argl, char character, int pos);
    int getRegister(char* arg, int start, int end, char& reg);
    int getMemoryLocation(char* arg, int start, int end, char& reg, unsigned short& addr);

    char* assemblyCode;             // Temporary memory used for parsing the assembly
    int assemblyCodeLength;         // Length of the temporary memory
    char** labelList;               // List of all labels in the assembly
    unsigned short* labelAddresses; // List of the addresses of all labels
    int labelListLength=0;          // Number of labels
    char* rawAssembly;              // Temporary memory used to hold the raw assembly
    virtual int getInstructionType(char* instrName)=0;
    virtual char getRRR(char* instrName)=0;
    virtual char getRX(char* instrName)=0;
    virtual char getJump(char* instrName)=0;
};

#endif // ASSEMBLER_H
