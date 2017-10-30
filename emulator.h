#include<string>
#include<string.h>
#ifndef EMULATOR_H
#define EMULATOR_H


class Emulator
{
public:
    Emulator();
    unsigned short pc;         //pc register
    unsigned short ir;         //ir register
    unsigned short adr;        //adr register
    unsigned short dat;        //dat register
    int leftAffectedBegin;     //beginning of the selection in the left memory tab
    int leftAffectedEnd;       //ending of the selection in the left memory tab
    int rightAffectedBegin;    //beginning of the selection in the right memory tab
    int rightAffectedEnd;      //ending of the selection in the right memory tab
    std::string getRegisterFile(); //function to parse the register file for the emulator tab
    std::string getMemoryFile();   //function to parse the memory file for the emulator tab
    std::string getMemoryAddress(int address);  //function for parsing a single memory address for the emulator tab
    bool isMemoryModified();   //function for checking if more than one line of memory has been modified
    int isMemoryAddressModified();  //function for returning the exact memory address that has been modified
    void reset();              //function for resetting the emulator
    void loadMemory(std::string &data);  //function for loading a raw assembly data into the memory
    static int hex2dec(char c);     //function for quick conversion between hexadecimal and decimal
    bool step017();            //function for emulating a single instruction for the 0.1.7 architecture
    bool step144();            //function for emulating a single instruction for the 1.4.4 architecture
    int executedInstructions;  //total count of executed instructions
    std::string lastOperation;     //string holding the last operation for the emulator tab
    std::string lastOperands;      //string holding the last operands for the emulator tab
    std::string lastEffect;        //string holding the last effect for the emulator tab
    std::string lastType;          //string holding the last type for the emulator tab
    std::string getInstructionList(int type);   //function for parsing the instruction list for the selected architecture
    bool isHalted();           //function for checking if the emulation has been halted
    std::string getOutput();       //function for returning the output for the I/O in the emulator tab

private:
    unsigned char memory[65536][4];  //memory values representation
    std::string memoryFile;        //string holding the memory file representation in the emulator tab
    short registers[16];       //registers values representation
    bool modified;             //is the whole memory modified
    int modifiedAddress;       //the last modified memory address
    bool halted;               //is the emulation halted
    bool paused;               //is the emulation paused
    bool flagGt=false;         //comparison flag for the 1.4.4 architecture
    bool flagEq=false;         //comparison flag for the 1.4.4 architecture
    bool flagLt=false;         //comparison flag for the 1.4.4 architecture
    bool flagCo=false;         //comparison flag for the 1.4.4 architecture
    bool flagOv=false;         //comparison flag for the 1.4.4 architecture
    std::string output;            //string holding the output of the last instruction (non-empty only after a trap instruction)
    std::string instructions017[21]={"add","sub","mul","div","cmplt","cmpeq","cmpgt","inv","and","or","xor","shiftl","shiftr","trap","lea","load","store","jump","jumpf","jumpt","jal"};
    std::string instructions144[27]={"add","sub","mul","div","cmp","inv","and","or","xor","shiftl","shiftr","trap","lea","load","store","jump","jumplt","jumple","jumpeq","jumpne","jumpge","jumpgt","jumpovf","jumpnovf","jumpco","jumpnco","jal"};
};

#endif // EMULATOR_H
