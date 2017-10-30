#include "emulator.h"

Emulator::Emulator() {
    reset();
}

bool Emulator::isMemoryModified() {
    return modified;
}

int Emulator::isMemoryAddressModified() {
    return modifiedAddress;
}

void Emulator::reset() {
    //resetting the pc/ir/adr/dat registers
    pc=0;
    ir=0;
    adr=0;
    dat=0;
    //resetting the registers and memory
    memset(registers,0,16*sizeof(short));
    memset(memory,0,65536*4*sizeof(char));
    //setting the modified tag of the memory
    modified=1;
    modifiedAddress=0;
    //resetting selections
    leftAffectedBegin=0;
    leftAffectedEnd=0;
    rightAffectedBegin=0;
    rightAffectedEnd=0;
    //resetting the halted/paused flags
    halted=false;
    paused=false;
    //resetting the exectutedInstructions
    executedInstructions=0;
    //resetting all strings
    lastEffect="";
    lastOperands="";
    lastOperation="";
    lastType="";
    output="";
}

std::string Emulator::getRegisterFile() {
    std::string result;
    char temp[50];
    for(int i=0;i<16;i++) {
        result.append("R");
        sprintf(temp,"%d",i);
        result.append(temp);
        if(i<10)
            result.append("  ");
        else
            result.append(" ");
        sprintf(temp,"%04hx",registers[i]);
        result.append(temp);
        result.append("\n");
    }
    return result;
}

std::string Emulator::getMemoryFile() {
    char* result=new char[65536*11+1];
    for(int i=0;i<65536;i++) {
        sprintf(result+i*11,"%04hx  %x%x%x%x\n",i,memory[i][0],memory[i][1],memory[i][2],memory[i][3]);
    }
    result[65536*11]='\0';
    memoryFile=std::string(result);
    delete[] result;
    modified=0;
    return memoryFile;
}

std::string Emulator::getMemoryAddress(int address) {
    char result[12];
    sprintf(result,"%04hx  %x%x%x%x\n",address,memory[address][0],memory[address][1],memory[address][2],memory[address][3]);
    modifiedAddress=-1;
    return std::string(result);
}

void Emulator::loadMemory(std::string &data) {
    if(data.substr(0,5).compare("ASM02"))
        return;
    for(unsigned int i=5;i<data.length();i+=4) {
        memory[(i-5)/4][0]=hex2dec(data[i]);
        memory[(i-5)/4][1]=hex2dec(data[i+1]);
        memory[(i-5)/4][2]=hex2dec(data[i+2]);
        memory[(i-5)/4][3]=hex2dec(data[i+3]);
    }
    modified=true;
}

int Emulator::hex2dec(char c) {
    int resp[256];
    resp['0']=0;
    resp['1']=1;
    resp['2']=2;
    resp['3']=3;
    resp['4']=4;
    resp['5']=5;
    resp['6']=6;
    resp['7']=7;
    resp['8']=8;
    resp['9']=9;
    resp['a']=10;
    resp['b']=11;
    resp['c']=12;
    resp['d']=13;
    resp['e']=14;
    resp['f']=15;
    return resp[int(c)];
}

bool Emulator::step017() {
    if(halted) {
        return false;
    }
    char temp[50];
    bool isRX=false;
    ir=memory[pc][0]*4096+memory[pc][1]*256+memory[pc][2]*16+memory[pc][3];
    leftAffectedBegin=pc;
    leftAffectedEnd=pc+1;
    rightAffectedBegin=0;
    rightAffectedEnd=0;
    if(memory[pc][0]<=13) {
        lastOperation=instructions017[memory[pc][0]];
        lastType="RRR";
        lastOperands="";
        lastOperands.append("R");
        sprintf(temp,"%d",memory[pc][1]);
        lastOperands.append(temp);
        lastOperands.append(",R");
        sprintf(temp,"%d",memory[pc][2]);
        lastOperands.append(temp);
        lastOperands.append(",R");
        sprintf(temp,"%d",memory[pc][3]);
        lastOperands.append(temp);
    }
    else if(memory[pc][0]==15) {
        lastOperation=instructions017[14+memory[pc][3]];
        lastType="RX";
    }
    if(memory[pc][0]==0) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])+(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==1) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])-(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==2) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])*(unsigned short)(registers[memory[pc][3]]);
        registers[15]=0;
    }
    else if(memory[pc][0]==3) {
        if(registers[memory[pc][3]]!=0) {
            unsigned short rem=(unsigned short)(registers[memory[pc][2]])%(unsigned short)(registers[memory[pc][3]]);
            registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])/(unsigned short)(registers[memory[pc][3]]);
            registers[15]=rem;
        }
        else {
            lastEffect="DIVISION BY ZERO";
            halted=true;
        }
    }
    else if(memory[pc][0]==4) {
        registers[memory[pc][1]]=registers[memory[pc][2]]<registers[memory[pc][3]];
    }
    else if(memory[pc][0]==5) {
        registers[memory[pc][1]]=registers[memory[pc][2]]==registers[memory[pc][3]];
    }
    else if(memory[pc][0]==6) {
        registers[memory[pc][1]]=registers[memory[pc][2]]>registers[memory[pc][3]];
    }
    else if(memory[pc][0]==7) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])^0b1111;
    }
    else if(memory[pc][0]==8) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])&(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==9) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])|(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==10) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])^(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==11) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])<<(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==12) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])>>(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==13) {
        if(registers[memory[pc][1]]==0) {
            halted=true;
            executedInstructions++;
            lastEffect="Halted";
            return false;
        }
        else if(registers[memory[pc][1]]==2) {
            int startt=registers[memory[pc][2]];
            int endt=startt+registers[memory[pc][3]];
            for(int t=startt;t<endt;t++) {
                output.push_back(char(memory[t][0]*4096+memory[t][1]*256+memory[t][2]*16+memory[t][3]));
            }
            lastEffect="Write";
        }
    }
    else if(memory[pc][0]==15) {
        isRX=true;
        adr=memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3]+registers[memory[pc][2]];
        if(memory[pc][3]==3) {
            lastOperands="";
            lastOperands.append("$");
            sprintf(temp,"%04hx",adr-registers[memory[pc][2]]);
            lastOperands.append(temp);
            lastOperands.append("[R");
            sprintf(temp,"%d",memory[pc][2]);
            lastOperands.append(temp);
            lastOperands.append("]");
        }
        else {
            lastOperands="";
            lastOperands.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastOperands.append(temp);
            lastOperands.append(",");
            lastOperands.append("$");
            sprintf(temp,"%04hx",adr-registers[memory[pc][2]]);
            lastOperands.append(temp);
            lastOperands.append("[R");
            sprintf(temp,"%d",memory[pc][2]);
            lastOperands.append(temp);
            lastOperands.append("]");
        }
        if(memory[pc][3]==0) {
            registers[memory[pc][1]]=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            lastEffect="";
            lastEffect.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastEffect.append(temp);
            lastEffect.append(" = ");
            sprintf(temp,"%04hx",registers[memory[pc][1]]);
            lastEffect.append(temp);
            pc+=2;
        }
        else if(memory[pc][3]==1) {
            int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            registers[memory[pc][1]]=(unsigned short)(memory[addr][0]*4096+memory[addr][1]*256+memory[addr][2]*16+memory[addr][3]);
            lastEffect="";
            lastEffect.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastEffect.append(temp);
            lastEffect.append(" = ");
            sprintf(temp,"%04hx",registers[memory[pc][1]]);
            lastEffect.append(temp);
            pc+=2;
        }
        else if(memory[pc][3]==2) {
            int mem0,mem1,mem2,mem3;
            unsigned short t;
            t=registers[memory[pc][1]];
            dat=t;
            mem0=(unsigned char)(t/4096);
            mem1=(unsigned char)((t/256)%16);
            mem2=(unsigned char)((t/16)%16);
            mem3=(unsigned char)(t%16);
            int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            rightAffectedBegin=addr;
            rightAffectedEnd=addr+1;
            memory[addr][0]=mem0;
            memory[addr][1]=mem1;
            memory[addr][2]=mem2;
            memory[addr][3]=mem3;
            modifiedAddress=addr;
            lastEffect="";
            lastEffect.append("mem[");
            sprintf(temp,"%04hx",addr);
            lastEffect.append(temp);
            lastEffect.append("] = ");
            sprintf(temp,"%04hx",dat);
            lastEffect.append(temp);
            pc+=2;
        }
        else if(memory[pc][3]==3) {
            int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            lastEffect="";
            lastEffect.append("jump to ");
            sprintf(temp,"%04hx",addr);
            lastEffect.append(temp);
            pc=addr;
        }
        else if(memory[pc][3]==4) {
            if(!registers[memory[pc][1]]) {
                int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
                lastEffect="";
                lastEffect.append("jump to ");
                sprintf(temp,"%04hx",addr);
                lastEffect.append(temp);
                pc=addr;
            }
            else {
                lastEffect="no jump";
                pc+=2;
            }
        }
        else if(memory[pc][3]==5) {
            if(registers[memory[pc][1]]) {
                int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
                lastEffect="";
                lastEffect.append("jump to ");
                sprintf(temp,"%04hx",addr);
                lastEffect.append(temp);
                pc=addr;
            }
            else {
                lastEffect="no jump";
                pc+=2;
            }
        }
        else if(memory[pc][3]==6) {
            registers[memory[pc][1]]=pc+2;
            int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            lastEffect="";
            lastEffect.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastEffect.append(temp);
            lastEffect.append(" = ");
            sprintf(temp,"%04hx",pc+2);
            lastEffect.append(temp);
            lastEffect.append(" , jump to ");
            sprintf(temp,"%04hx",addr);
            lastEffect.append(temp);
            pc=addr;
        }
        leftAffectedEnd++;
    }
    if(!isRX) {
        if(lastEffect!="DIVISION BY ZERO") {
            lastEffect="";
            lastEffect.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastEffect.append(temp);
            lastEffect.append(" = ");
            sprintf(temp,"%04hx",registers[memory[pc][1]]);
            lastEffect.append(temp);
        }
        pc++;
    }
    registers[0]=0;
    executedInstructions++;
    return true;
}

bool Emulator::step144() {
    if(halted) {
        return false;
    }
    char temp[50];
    bool isRX=false;
    ir=memory[pc][0]*4096+memory[pc][1]*256+memory[pc][2]*16+memory[pc][3];
    leftAffectedBegin=pc;
    leftAffectedEnd=pc+1;
    rightAffectedBegin=0;
    rightAffectedEnd=0;
    if(memory[pc][0]<=11) {
        lastOperation=instructions144[memory[pc][0]];
        lastType="RRR";
        lastOperands="";
        lastOperands.append("R");
        if(memory[pc][0]!=4) {
            sprintf(temp,"%d",memory[pc][1]);
            lastOperands.append(temp);
            lastOperands.append(",R");
        }
        sprintf(temp,"%d",memory[pc][2]);
        lastOperands.append(temp);
        lastOperands.append(",R");
        sprintf(temp,"%d",memory[pc][3]);
        lastOperands.append(temp);
    }
    else if(memory[pc][0]==15) {
        if(memory[pc][3]<=2)
            lastOperation=instructions144[12+memory[pc][3]];
        else if(memory[pc][3]==3)
            lastOperation=instructions144[15+memory[pc][1]];
        else if(memory[pc][3]==4)
            lastOperation=instructions144[26];
        lastType="RX";
    }
    if(memory[pc][0]==0) {
        registers[15]=0;
        registers[15]|=(1<<5);
        if((unsigned short)(registers[memory[pc][2]])>>15==1 && (unsigned short)(registers[memory[pc][3]])>>15==1)
            registers[15]|=(1<<4);
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])+(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==1) {
        registers[15]=0;
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])-(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==2) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])*(unsigned short)(registers[memory[pc][3]]);
        registers[15]=0;
    }
    else if(memory[pc][0]==3) {
        if(registers[memory[pc][3]]!=0) {
            unsigned short rem=(unsigned short)(registers[memory[pc][2]])%(unsigned short)(registers[memory[pc][3]]);
            registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])/(unsigned short)(registers[memory[pc][3]]);
            registers[15]=rem;
        }
        else {
            lastEffect="DIVISION BY ZERO";
            halted=true;
        }
    }
    else if(memory[pc][0]==4) {
        if(registers[memory[pc][2]]>registers[memory[pc][3]])
            flagGt=true;
        else
            flagGt=false;
        if(registers[memory[pc][2]]==registers[memory[pc][3]])
            flagEq=true;
        else
            flagEq=false;
        if(registers[memory[pc][2]]<registers[memory[pc][3]])
            flagLt=true;
        else
            flagLt=false;
        registers[15]=flagGt|(flagEq<<1)|(flagLt<<2)|(flagCo<<4)|(flagOv<<5);
    }
    else if(memory[pc][0]==5) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])^0b1111;
    }
    else if(memory[pc][0]==6) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])&(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==7) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])|(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==8) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])^(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==9) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])<<(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==10) {
        registers[memory[pc][1]]=(unsigned short)(registers[memory[pc][2]])>>(unsigned short)(registers[memory[pc][3]]);
    }
    else if(memory[pc][0]==11) {
        if(registers[memory[pc][1]]==0) {
            halted=true;
            executedInstructions++;
            lastEffect="Halted";
            return false;
        }
        else if(registers[memory[pc][1]]==2) {
            int startt=registers[memory[pc][2]];
            int endt=startt+registers[memory[pc][3]];
            for(int t=startt;t<endt;t++) {
                output.push_back(char(memory[t][0]*4096+memory[t][1]*256+memory[t][2]*16+memory[t][3]));
            }
            lastEffect="Write";
        }
    }
    else if(memory[pc][0]==15) {
        isRX=true;
        adr=memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3]+registers[memory[pc][2]];
        if(memory[pc][3]==3) {
            lastOperands="";
            lastOperands.append("$");
            sprintf(temp,"%04hx",adr-registers[memory[pc][2]]);
            lastOperands.append(temp);
            lastOperands.append("[R");
            sprintf(temp,"%d",memory[pc][2]);
            lastOperands.append(temp);
            lastOperands.append("]");
        }
        else {
            lastOperands="";
            lastOperands.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastOperands.append(temp);
            lastOperands.append(",");
            lastOperands.append("$");
            sprintf(temp,"%04hx",adr-registers[memory[pc][2]]);
            lastOperands.append(temp);
            lastOperands.append("[R");
            sprintf(temp,"%d",memory[pc][2]);
            lastOperands.append(temp);
            lastOperands.append("]");
        }
        if(memory[pc][3]==0) {
            registers[memory[pc][1]]=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            lastEffect="";
            lastEffect.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastEffect.append(temp);
            lastEffect.append(" = ");
            sprintf(temp,"%04hx",registers[memory[pc][1]]);
            lastEffect.append(temp);
            pc+=2;
        }
        else if(memory[pc][3]==1) {
            int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            registers[memory[pc][1]]=(unsigned short)(memory[addr][0]*4096+memory[addr][1]*256+memory[addr][2]*16+memory[addr][3]);
            lastEffect="";
            lastEffect.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastEffect.append(temp);
            lastEffect.append(" = ");
            sprintf(temp,"%04hx",registers[memory[pc][1]]);
            lastEffect.append(temp);
            pc+=2;
        }
        else if(memory[pc][3]==2) {
            int mem0,mem1,mem2,mem3;
            unsigned short t;
            t=registers[memory[pc][1]];
            dat=t;
            mem0=(unsigned char)(t/4096);
            mem1=(unsigned char)((t/256)%16);
            mem2=(unsigned char)((t/16)%16);
            mem3=(unsigned char)(t%16);
            int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            rightAffectedBegin=addr;
            rightAffectedEnd=addr+1;
            memory[addr][0]=mem0;
            memory[addr][1]=mem1;
            memory[addr][2]=mem2;
            memory[addr][3]=mem3;
            modifiedAddress=addr;
            lastEffect="";
            lastEffect.append("mem[");
            sprintf(temp,"%04hx",addr);
            lastEffect.append(temp);
            lastEffect.append("] = ");
            sprintf(temp,"%04hx",dat);
            lastEffect.append(temp);
            pc+=2;
        }
        else if(memory[pc][3]==3) {
            if(memory[pc][1]==0 ||
              (memory[pc][1]==1 && flagLt) ||
              (memory[pc][1]==2 && (flagLt || flagEq)) ||
              (memory[pc][1]==3 && flagEq) ||
              (memory[pc][1]==4 && !flagEq) ||
              (memory[pc][1]==5 && (flagGt || flagEq)) ||
              (memory[pc][1]==6 && flagGt) ||
              (memory[pc][1]==7 && flagOv) ||
              (memory[pc][1]==8 && !flagOv) ||
              (memory[pc][1]==9 && flagCo) ||
              (memory[pc][1]==10 && !flagCo)) {
                int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
                lastEffect="";
                lastEffect.append("jump to ");
                sprintf(temp,"%04hx",addr);
                lastEffect.append(temp);
                pc=addr;
            }
            else if(memory[pc][1]==11) {
                registers[memory[pc][1]]=pc+2;
                int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
                lastEffect="";
                lastEffect.append("R");
                sprintf(temp,"%d",memory[pc][1]);
                lastEffect.append(temp);
                lastEffect.append(" = ");
                sprintf(temp,"%04hx",pc+2);
                lastEffect.append(temp);
                lastEffect.append(" , jump to ");
                sprintf(temp,"%04hx",addr);
                lastEffect.append(temp);
                pc=addr;
            }
            else {
                lastEffect="no jump";
                pc+=2;
            }
        }
        else if(memory[pc][3]==4) {
            registers[memory[pc][1]]=pc+2;
            int addr=(memory[pc+1][0]*4096+memory[pc+1][1]*256+memory[pc+1][2]*16+memory[pc+1][3])+registers[memory[pc][2]];
            lastEffect="";
            lastEffect.append("R");
            sprintf(temp,"%d",memory[pc][1]);
            lastEffect.append(temp);
            lastEffect.append(" = ");
            sprintf(temp,"%04hx",pc+2);
            lastEffect.append(temp);
            lastEffect.append(" , jump to ");
            sprintf(temp,"%04hx",addr);
            lastEffect.append(temp);
            pc=addr;
        }
        leftAffectedEnd++;
    }
    if(!isRX) {
        if(lastEffect!="DIVISION BY ZERO") {
            lastEffect="";
            lastEffect.append("R");
            if(memory[pc][0]!=4)
                sprintf(temp,"%d",memory[pc][1]);
            else
                sprintf(temp,"15");
            lastEffect.append(temp);
            lastEffect.append(" = ");
            if(memory[pc][0]!=4)
                sprintf(temp,"%04hx",registers[memory[pc][1]]);
            else
                sprintf(temp,"%04hx",registers[15]);
            lastEffect.append(temp);
        }
        pc++;
    }
    registers[0]=0;
    executedInstructions++;
    return true;
}

bool Emulator::isHalted() {
    return halted;
}

std::string Emulator::getOutput() {
    std::string returnString(output);
    output="";
    return returnString;
}

std::string Emulator::getInstructionList(int type) {
    std::string returnString="";
    if(type==1)
        for(int i=0;i<21;i++) {
            returnString.append(instructions017[i]);
            returnString.append("\n");
        }
    else if(type==2)
        for(int i=0;i<27;i++) {
            returnString.append(instructions144[i]);
            returnString.append("\n");
        }
    return returnString;
}
