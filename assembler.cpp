#include "assembler.h"
#include "emulator.h"

Assembler::Assembler(const char* data, int length) {
    buffer=new char[length+1];
    memcpy(buffer,data,length);
    bufferLength=length;
    labelList=new char*[65536];
    labelAddresses=new unsigned short[65536];
    labelSize=0;
    fp=new char[262144];
    fp[0]='\0';
}

Assembler::~Assembler() {
    delete[] buffer;
    delete[] labelAddresses;
    for(int i=0;i<labelSize;i++)
        delete[] labelList[i];
    delete[] labelList;
    delete[] fp;
}

void Assembler::removeTabs(char *buffer, int length) {
    for(int i=0;i<length;i++)
        if(buffer[i]=='\t')
            buffer[i]=' ';
}

void Assembler::removeSpaces(char *buffer, int &length) {
    char* newBuffer=new char[length+1];
    int i,j=1;
    newBuffer[0]=buffer[0];
    for(i=1;i<length;i++) {
        if(buffer[i]==' ' && buffer[i-1]==' ')
            continue;
        else
            newBuffer[j++]=buffer[i];
    }
    newBuffer[j]='\0';
    strcpy(buffer,newBuffer);
    delete[] newBuffer;
    length=j;
}

void Assembler::fixNewLineCharacters(char *buffer, int &length) {
    char* newBuffer=new char[length+1];
    int i,j=0;
    for(i=0;i<length;i++) {
        if(i<length-1 && buffer[i]=='\r' && buffer[i+1]=='\n')
            newBuffer[j++]=buffer[++i];
        else if(buffer[i]=='\r')
            newBuffer[j++]='\n';
        else
            newBuffer[j++]=buffer[i];
    }
    newBuffer[j]='\0';
    memcpy(buffer,newBuffer,j+1);
    delete[] newBuffer;
    length=j;
}

void Assembler::removeComments(char *buffer, int &length) {
    char* newBuffer=new char[length+1];
    int i,j=0;
    for(i=0;i<length;i++) {
        if(buffer[i]==';') {
            while(buffer[i++]!='\n');
            newBuffer[j++]='\n';
            i--;
        }
        else
            newBuffer[j++]=buffer[i];
    }
    newBuffer[j]='\0';
    memcpy(buffer,newBuffer,j+1);
    delete[] newBuffer;
    length=j;
}

void Assembler::removeEmptyLines(char *buffer, int &length, int *actualLine) {
    char* newBuffer=new char[length+1];
    int* newLines=new int[length+1];
    int i,j=0;
    for(i=0;i<length;i++) {
        if(i>0 && buffer[i]=='\n' && buffer[i-1]=='\n')
            continue;
        else if(i>0 && i<length-1 && buffer[i]==' ' && buffer[i-1]=='\n' && buffer[i+1]=='\n') {
            i++;
            continue;
        }
        else if(i==0 && i<length-1 && buffer[i]=='\n' && buffer[i+1]=='\n')
            continue;
        else {
            newBuffer[j]=buffer[i];
            newLines[j++]=actualLine[i];
        }
    }
    if(newBuffer[j-1]=='\n' && newBuffer[j-2]==' ')
        j-=2;
    else if(newBuffer[j-1]=='\n')
        j--;
    newBuffer[j]='\0';
    memcpy(buffer,newBuffer,j+1);
    memcpy(actualLine,newLines,sizeof(int)*j);
    delete[] newBuffer;
    delete[] newLines;
    length=j;
}

QString Assembler::readAssembly() {
    QString returnString(fp);
    return returnString;
}

int Assembler::checkRRR3(char *arg, int argl, char &reg1, char &reg2, char &reg3) {
    char temp[64];
    int i=0;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg1=temp[0];
            i++;
        }
        else {
            reg1=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(arg[i]!=',') return 3;
    i++;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg2=temp[0];
            i++;
        }
        else {
            reg2=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(arg[i]!=',') return 3;
    i++;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg3=temp[0];
            i++;
        }
        else {
            reg3=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(i!=argl) return 4;
    return 0;
}

int Assembler::checkRRR2(char *arg, int argl, char &reg1, char &reg2) {
    char temp[64];
    int i=0;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg1=temp[0];
            i++;
        }
        else {
            reg1=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(arg[i]!=',') return 3;
    i++;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg2=temp[0];
            i++;
        }
        else {
            reg2=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(i!=argl) return 4;
    return 0;
}

int Assembler::checkRX1(char *arg, int argl, char &reg1, unsigned short &addr) {
    char temp[64];
    char lookfor[64];
    int lookforl=0;
    lookfor[lookforl]='\0';
    int i=0;
    while(arg[i]!='[' && i<argl) {
        lookfor[lookforl++]=arg[i++];
        lookfor[lookforl]='\0';
    }
    if(i==argl) return 4;
    i++;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg1=temp[0];
            i++;
        }
        else {
            reg1=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(arg[i]!=']') return 4;
    i++;
    if(i!=argl) return 4;
    if(lookfor[0]>='0' && lookfor[0]<='9') {
        int num=0;
        for(i=0;i<lookforl;i++) {
            if(lookfor[i]<'0' || lookfor[i]>'9')
                return 2;
            num=num*10+(lookfor[i]-'0');
        }
        addr=num;
    }
    else if(lookfor[0]=='$') {
        if(lookforl!=5)
            return 2;
        int num=0;
        for(i=1;i<lookforl;i++) {
            if(!((lookfor[i]>='0' && lookfor[i]<='9') || (lookfor[i]>='a' && lookfor[i]<='f')))
                return 2;
            num=num*16+Emulator::hex2dec(QChar(lookfor[i]));
        }
        addr=num;
    }
    else if(lookfor[0]=='-') {
        int num=0;
        for(i=1;i<lookforl;i++) {
            if(lookfor[i]<'0' || lookfor[i]>'9')
                return 2;
            num=num*10+(lookfor[i]-'0');
        }
        addr=-num;
    }
    else {
        addr=0xffff;
        for(i=0;i<labelSize;i++)
            if(strcmp(lookfor,labelList[i])==0) {
                addr=labelAddresses[i];
                break;
            }
        if(addr==0xffff)
            return 5;
    }
    return 0;
}

int Assembler::checkRX2(char *arg, int argl, char &reg1, char &reg2, unsigned short &addr) {
    char temp[64];
    char lookfor[64];
    int lookforl=0;
    lookfor[lookforl]='\0';
    int i=0;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg1=temp[0];
            i++;
        }
        else {
            reg1=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(arg[i]!=',') return 3;
    i++;
    while(arg[i]!='[' && i<argl) {
        lookfor[lookforl++]=arg[i++];
        lookfor[lookforl]='\0';
    }
    if(i==argl) return 4;
    i++;
    if(arg[i]!='R') return 1;
    i++;
    if(arg[i]>='0' && arg[i]<='9') {
        if(arg[i+1]>='0' && arg[i+1]<='9') {
            sprintf(temp,"%x",(arg[i]-'0')*10+(arg[i+1]-'0'));
            reg2=temp[0];
            i++;
        }
        else {
            reg2=arg[i];
        }
    }
    else
        return 2;
    i++;
    if(arg[i]!=']') return 4;
    i++;
    if(i!=argl) return 4;
    if(lookfor[0]>='0' && lookfor[0]<='9') {
        int num=0;
        for(i=0;i<lookforl;i++) {
            if(lookfor[i]<'0' || lookfor[i]>'9')
                return 2;
            num=num*10+(lookfor[i]-'0');
        }
        addr=num;
    }
    else if(lookfor[0]=='$') {
        if(lookforl!=5)
            return 2;
        int num=0;
        for(i=1;i<lookforl;i++) {
            if(!((lookfor[i]>='0' && lookfor[i]<='9') || (lookfor[i]>='a' && lookfor[i]<='f')))
                return 2;
            num=num*16+Emulator::hex2dec(QChar(lookfor[i]));
        }
        addr=num;
    }
    else if(lookfor[0]=='-') {
        int num=0;
        for(i=1;i<lookforl;i++) {
            if(lookfor[i]<'0' || lookfor[i]>'9')
                return 2;
            num=num*10+(lookfor[i]-'0');
        }
        addr=-num;
    }
    else {
        addr=0xffff;
        for(i=0;i<labelSize;i++)
            if(strcmp(lookfor,labelList[i])==0) {
                addr=labelAddresses[i];
                break;
            }
        if(addr==0xffff)
            return 5;
    }
    return 0;
}

void Assembler::printError(int result,int lineError,char* fp) {
    if(result==1) sprintf(fp,"Unexpected argument - R expected on line %d!\n",lineError);
    if(result==2) sprintf(fp,"Unexpected argument - register expected on line %d!\n",lineError);
    if(result==3) sprintf(fp,"Unexpected argument - comma expected on line %d!\n",lineError);
    if(result==4) sprintf(fp,"Unexpected/invalid argument - line %d!\n",lineError);
    if(result==5) sprintf(fp,"Memory address not found - line %d!\n",lineError);
}

bool Assembler::translateInstruction(char *instrName, char *instrArgs, int instrArgsLength, char *fp, int lineError) {
    int t=getInstructionType(instrName);
    int result;
    if(t==1) {
        char reg1,reg2,reg3;
        result=checkRRR3(instrArgs,instrArgsLength,reg1,reg2,reg3);
        if(result!=0) {
            printError(result,lineError,fp);
            return false;
        }
        sprintf(fp+strlen(fp),"%c%c%c%c",getRRR(instrName),reg1,reg2,reg3);
        return true;
    }
    if(t==2) {
        char reg1,reg2;
        result=checkRRR2(instrArgs,instrArgsLength,reg1,reg2);
        if(result!=0) {
            printError(result,lineError,fp);
            return false;
        }
        sprintf(fp+strlen(fp),"%c0%c%c",getRRR(instrName),reg1,reg2);
        return true;
    }
    if(t==3) {
        char reg1,reg2;
        unsigned short addr;
        result=checkRX2(instrArgs,instrArgsLength,reg1,reg2,addr);
        if(result!=0) {
            printError(result,lineError,fp);
            return false;
        }
        sprintf(fp+strlen(fp),"f%c%c%c",reg1,reg2,getRX(instrName));
        sprintf(fp+strlen(fp),"%04hx",addr);
        return true;
    }
    if(t==4) {
        char reg1;
        unsigned short addr;
        result=checkRX1(instrArgs,instrArgsLength,reg1,addr);
        if(result!=0) {
            printError(result,lineError,fp);
            return false;
        }
        sprintf(fp+strlen(fp),"f%c%c%c",getJump(instrName),reg1,getRX(instrName));
        sprintf(fp+strlen(fp),"%04hx",addr);
        return true;
    }
    if(t==5) {
        if(instrArgs[0]=='$') {
            if(instrArgsLength!=5) {
                sprintf(fp,"Invalid number of arguments supplied for data - line %d!\n",lineError);
                return false;
            }
            else
                sprintf(fp+strlen(fp),"%c%c%c%c",instrArgs[1],instrArgs[2],instrArgs[3],instrArgs[4]);
        }
        else {
            int ti=0;
            if(instrArgs[0]=='-') ti++;
            int num=0;
            while(ti<instrArgsLength) {
                if(instrArgs[ti]<'0' || instrArgs[ti]>'9') {
                    sprintf(fp,"Invalid character supplied for data - line %d!\n",lineError);
                    return false;
                }
                num=num*10+(instrArgs[ti++]-'0');
            }
            if(instrArgs[0]=='-') num=-num;
            char temp[64];
            sprintf(temp,"%04hx",(short)(num));
            sprintf(fp+strlen(fp),"%c%c%c%c",temp[0],temp[1],temp[2],temp[3]);
        }
        return true;
    }
    return false;
}

void Assembler::assemble() {
    int i,curLine=1;
    int* lines=new int[bufferLength];
    removeTabs(buffer,bufferLength);
    removeSpaces(buffer,bufferLength);
    fixNewLineCharacters(buffer,bufferLength);
    removeComments(buffer,bufferLength);
    for(i=0;i<bufferLength;i++) {
        lines[i]=curLine;
        if(buffer[i]=='\n')
            curLine++;
    }
    removeEmptyLines(buffer,bufferLength,lines);
    i=0;
    int memoryAddress=0;
    int error=0;
    while(i<bufferLength) {
        char labelName[1024];
        int labelLength=0;
        char instrName[1024];
        int instrLength=0;
        int state=0;
        //parse label name
        while(1) {
            if(buffer[i]==' ')
                break;
            if(buffer[i]=='\n' || i==bufferLength) {
                state=1;
                break;
            }
            labelName[labelLength++]=buffer[i];
            i++;
        }
        labelName[labelLength]='\0';
        if(labelLength!=0) {
            labelList[labelSize]=new char[1024];
            labelAddresses[labelSize]=memoryAddress;
            strcpy(labelList[labelSize],labelName);
            labelSize++;
        }
        if(state==1) {
            i++;
            continue;
        }
        i++;
        while(1) {
            if(buffer[i]==' ')
                break;
            if(buffer[i]=='\n' || i==bufferLength) {
                state=1;
                break;
            }
            instrName[instrLength++]=buffer[i];
            i++;
        }
        instrName[instrLength]='\0';
        if(state==1) {
            i++;
            continue;
        }
        int instrType=getInstructionType(instrName);
        if(instrType==-1) {
            sprintf(fp,"Invalid instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }
        else if(instrType==1 || instrType==2 || instrType==5) {
            memoryAddress++;
        }
        else {
            memoryAddress+=2;
        }
        while(buffer[i]!='\n' && i<bufferLength) i++;
        i++;
    }
    if(error)
        return;
    i=0;
    sprintf(fp,"ASM02");
    while(i<bufferLength) {
        char instrName[1024];
        int instrLength=0;
        char instrArgs[1024];
        int instrArgsLength=0;
        int state=0;
        //parse label name
        while(1) {
            if(buffer[i]==' ')
                break;
            if(buffer[i]=='\n' || i==bufferLength) {
                state=1;
                break;
            }
            i++;
        }
        if(state==1) {
            i++;
            continue;
        }
        i++;
        while(1) {
            if(buffer[i]==' ')
                break;
            if(buffer[i]=='\n' || i==bufferLength) {
                state=1;
                break;
            }
            instrName[instrLength++]=buffer[i];
            i++;
        }
        instrName[instrLength]='\0';
        if(state==1 && instrLength==0) {
            i++;
            continue;
        }
        if(state==1) {
            sprintf(fp,"Arguments expected for instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }
        int instrType=getInstructionType(instrName);
        if(instrType==-1) {
            sprintf(fp,"Invalid instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }
        i++;
        while(1) {
            if(buffer[i]==' ')
                break;
            if(buffer[i]=='\n' || i==bufferLength) {
                state=1;
                break;
            }
            instrArgs[instrArgsLength++]=buffer[i];
            i++;
        }
        instrArgs[instrArgsLength]='\0';
        if(state==1 && instrArgsLength==0) {
            sprintf(fp,"Arguments expected for instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }
        if(!translateInstruction(instrName,instrArgs,instrArgsLength,fp,lines[i])) {
            error=1;
            break;
        }
        while(buffer[i]!='\n' && i<bufferLength) i++;
        i++;
    }
    delete[] lines;
}
