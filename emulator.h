#include <QString>
#ifndef EMULATOR_H
#define EMULATOR_H


class Emulator
{
public:
    Emulator();
    unsigned short pc;
    unsigned short ir;
    unsigned short adr;
    unsigned short dat;
    int leftAffectedBegin;
    int leftAffectedEnd;
    int rightAffectedBegin;
    int rightAffectedEnd;
    QString getRegisterFile();
    QString getMemoryFile();
    QString getMemoryAddress(int address);
    bool isMemoryModified();
    int isMemoryAddressModified();
    void reset();
    void loadMemory(QString& data);
    static int hex2dec(QChar c);
    bool step017();
    bool step144();
    int executedInstructions;
    QString lastOperation;
    QString lastOperands;
    QString lastEffect;
    QString lastType;
    QString getInstructionList(int type);
    bool isHalted();
    QString getOutput();

private:
    unsigned char memory[65536][4];
    QString memoryFile;
    short registers[16];
    bool modified;
    int modifiedAddress;
    bool halted;
    bool paused;
    bool flagGt=false;
    bool flagEq=false;
    bool flagLt=false;
    bool flagCo=false;
    bool flagOv=false;
    QString output;
    QString instructions017[21]={"add","sub","mul","div","cmplt","cmpeq","cmpgt","inv","and","or","xor","shiftl","shiftr","trap","lea","load","store","jump","jumpf","jumpt","jal"};
    QString instructions144[27]={"add","sub","mul","div","cmp","inv","and","or","xor","shiftl","shiftr","trap","lea","load","store","jump","jumplt","jumple","jumpeq","jumpne","jumpge","jumpgt","jumpovf","jumpnovf","jumpco","jumpnco","jal"};
};

#endif // EMULATOR_H
