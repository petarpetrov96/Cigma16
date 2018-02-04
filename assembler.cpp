#include "assembler.h"
#include "emulator.h"

/*
 * Constructor
 */
Assembler::Assembler(const char* data, int length) {

    // Copying the program code into the buffer
    assemblyCode=new char[length+1];
    memcpy(assemblyCode,data,length);
    assemblyCode[length]='\0';
    assemblyCodeLength=length;

    // Creating an empty label list along with addresses for each label
    labelList=new char*[65536];
    labelAddresses=new unsigned short[65536];
    labelListLength=0;

    // Initializing an empty array to hold the rawAssembly
    rawAssembly=new char[262144];
    rawAssembly[0]='\0';
}

/*
 * Destructor
 */
Assembler::~Assembler() {

    // Deleting all used memory
    delete[] assemblyCode;
    delete[] labelAddresses;
    for(int i=0;i<labelListLength;i++)
        delete[] labelList[i];
    delete[] labelList;
    delete[] rawAssembly;
}

/*
 * Replaces all tabs with spaces in "assemblyCode"
 */
void Assembler::removeTabs(char *assemblyCode, int length) {
    for(int i=0;i<length;i++)
        if(assemblyCode[i]=='\t')
            assemblyCode[i]=' ';
}

/*
 * Removes all double spaces in "assemblyCode"
 */
void Assembler::removeSpaces(char *assemblyCode, int &length) {

    // Create empty buffer memory for the returned string and copy the first element
    char* buffer=new char[length+1];
    int i,j=1;
    buffer[0]=assemblyCode[0];

    // Go through "assemblyCode" and copy all characters that are not
    // preceeded by a space
    for(i=1;i<length;i++) {
        if(assemblyCode[i]==' ' && assemblyCode[i-1]==' ')
            continue;
        else
            buffer[j++]=assemblyCode[i];
    }

    // Terminate the string
    buffer[j]='\0';

    // Copy the temporary buffer to "assemblyCode" and delete it from the heap
    strcpy(assemblyCode,buffer);
    delete[] buffer;
    length=j;
}

/*
 * Fixes all new characters to \n
 */
void Assembler::fixNewLineCharacters(char *assemblyCode, int &length) {

    // Create empty buffer memory for the returned string
    char* buffer=new char[length+1];
    int i,j=0;

    // Copy all characters except for \r
    // Replace \r\n with \n and single \r with \n
    for(i=0;i<length;i++) {
        if(i<length-1 && assemblyCode[i]=='\r' && assemblyCode[i+1]=='\n')
            buffer[j++]=assemblyCode[++i];
        else if(assemblyCode[i]=='\r')
            buffer[j++]='\n';
        else
            buffer[j++]=assemblyCode[i];
    }

    // Terminate the string
    buffer[j]='\0';

    // Copy the temporary buffer to "assemblyCode" and delete it from the heap
    memcpy(assemblyCode,buffer,j+1);
    delete[] buffer;
    length=j;
}

/*
 * Removes all comments
 */
void Assembler::removeComments(char *assemblyCode, int &length) {

    // Create empty buffer memory for the returned string
    char* buffer=new char[length+1];
    int i,j=0;

    // Copy everything except the characters between ';' and '\n'
    for(i=0;i<length;i++) {
        if(assemblyCode[i]==';') {

            // Start from the i-th character and keep on until we reach a new line
            // or the end of the "assemblyCode"
            while(assemblyCode[i++]!='\n' && i<length);
            if(i<length) buffer[j++]='\n';

            // Go back one character since i will increase again in the for
            i--;
        }
        else
            buffer[j++]=assemblyCode[i];
    }

    // Terminate the string
    buffer[j]='\0';

    // Copy the temporary buffer to "assemblyCode" and delete it from the heap
    memcpy(assemblyCode,buffer,j+1);
    delete[] buffer;
    length=j;
}

/*
 * Removes all empty lines
 */
void Assembler::removeEmptyLines(char *assemblyCode, int &length, int *actualLine) {

    // Create empty buffer memory for the returned string
    // and keep track of what line every character belonged to
    // (used for error messages)
    char* buffer=new char[length+1];
    int* oldLineNumbers=new int[length+1];
    int i,j=0;

    // Remove all lines that consist of just '\n' or ' ' and '\n'
    for(i=0;i<length;i++) {
        if(i>0 && assemblyCode[i]=='\n' && assemblyCode[i-1]=='\n')
            continue;
        else if(i>0 && i<length-1 && assemblyCode[i]==' ' && assemblyCode[i-1]=='\n' && assemblyCode[i+1]=='\n') {
            i++;
            continue;
        }
        else if(i==0 && i<length-1 && assemblyCode[i]=='\n' && assemblyCode[i+1]=='\n')
            continue;
        else {
            buffer[j]=assemblyCode[i];
            oldLineNumbers[j++]=actualLine[i];
        }
    }

    // Remove if the end of the file is a new line followed by just a space character
    // or if the end of the file is just a new line
    if(buffer[j-1]=='\n' && buffer[j-2]==' ')
        j-=2;
    else if(buffer[j-1]=='\n')
        j--;

    // Terminate the string
    buffer[j]='\0';

    // Copy the temporary buffer to "assemblyCode",
    // the corresponding line numbers to "actualLine"
    // and delete them from the heap
    memcpy(assemblyCode,buffer,j+1);
    memcpy(actualLine,oldLineNumbers,sizeof(int)*j);
    delete[] buffer;
    delete[] oldLineNumbers;
    length=j;
}

/*
 * Returns an std::string containing the compiled assembly
 */
std::string Assembler::readAssembly() {
    return std::string(rawAssembly);
}

/*
 * Checks the formatting of an RRR instruction with 3 arguments
 * Returns an error number, check printError for explanation
 */
int Assembler::checkRRR3(char *arg, int argl, char &reg1, char &reg2, char &reg3) {
    int result;
    int comma1,comma2;
    comma1=findNextCharacter(arg,argl,',',0);
    if(comma1==-1) {
        return 4;
    }
    comma2=findNextCharacter(arg,argl,',',comma1+1);
    if(comma2==-1) {
        return 4;
    }
    result=getRegister(arg,0,comma1,reg1);
    if(result!=0) {
        return result;
    }
    result=getRegister(arg,comma1+1,comma2,reg2);
    if(result!=0) {
        return result;
    }
    result=getRegister(arg,comma2+1,argl,reg3);
    if(result!=0) {
        return result;
    }
    return 0;
}

/*
 * Checks the formatting of an RRR instruction with 2 arguments
 * Returns an error number, check printError for explanation
 */
int Assembler::checkRRR2(char *arg, int argl, char &reg1, char &reg2) {
    int result;
    int comma;
    comma=findNextCharacter(arg,argl,',',0);
    if(comma==-1) {
        return 4;
    }
    result=getRegister(arg,0,comma,reg1);
    if(result!=0) {
        return result;
    }
    result=getRegister(arg,comma+1,argl,reg2);
    if(result!=0) {
        return result;
    }
    return 0;
}

/*
 * Checks the formatting of an RX instruction with 1 argument
 * Returns an error number, check printError for explanation
 */
int Assembler::checkRX1(char *arg, int argl, char &reg1, unsigned short &addr) {
    int result;
    result=getMemoryLocation(arg,0,argl,reg1,addr);
    if(result!=0) {
        return result;
    }
    return 0;
}

/*
 * Checks the formatting of an RX instruction with 2 arguments
 * Returns an error number, check printError for explanation
 */
int Assembler::checkRX2(char *arg, int argl, char &reg1, char &reg2, unsigned short &addr) {
    int result;
    int comma;
    comma=findNextCharacter(arg,argl,',',0);
    if(comma==-1) {
        return 4;
    }
    result=getRegister(arg,0,comma,reg1);
    if(result!=0) {
        return result;
    }
    result=getMemoryLocation(arg,comma+1,argl,reg2,addr);
    if(result!=0) {
        return result;
    }
    return 0;
}

/*
 * Converts an integer error to a meaningful string with an added line number
 */
void Assembler::printError(int result,int lineError,char* fp) {
    if(result==1) sprintf(fp,"Unexpected argument - R expected on line %d!\n",lineError);
    if(result==2) sprintf(fp,"Unexpected argument - register expected on line %d!\n",lineError);
    if(result==3) sprintf(fp,"Unexpected argument - comma expected on line %d!\n",lineError);
    if(result==4) sprintf(fp,"Unexpected/invalid argument - line %d!\n",lineError);
    if(result==5) sprintf(fp,"Memory address not found - line %d!\n",lineError);
}

/*
 * Translates an instruction and its arguments to a compiled hexadecimal representation
 */
bool Assembler::translateInstruction(char *instrName, char *instrArgs, int instrArgsLength, char *fp, int lineError) {
    // getInstructionType returns the type of instruction where
    // 1 =  RRR instruction with 3 arguments (e.g. add R1,R2,R3)
    // 2 =  RRR instruction with 2 arguments (e.g. cmp R1,R2)
    // 3 =   RX instruction with 2 arguments (e.g. lea R1,label[R2])
    // 4 =   RX instruction with 1 argument  (e.g. jump label[R1])
    // 5 = data instruction
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

/*
 * Assembles the "assemblyCode" passed to the class
 * and stores the result in "rawAssembly"
 */
void Assembler::assemble() {
    int i,curLine=1;
    int* lines=new int[assemblyCodeLength]; // Temporary array that keeps track of which line each character was on, used in showing errors

    // Converts the human-readable assembly to a format the machine understands better
    removeTabs(assemblyCode,assemblyCodeLength); // Replaces tabs with spaces
    removeSpaces(assemblyCode,assemblyCodeLength); // Removes all double spaces
    fixNewLineCharacters(assemblyCode,assemblyCodeLength); // Replaces \r\n and \r with \n
    removeComments(assemblyCode,assemblyCodeLength); // Removes all comments

    // Fill the line information array
    for(i=0;i<assemblyCodeLength;i++) {
        lines[i]=curLine;
        if(assemblyCode[i]=='\n')
            curLine++;
    }

    removeEmptyLines(assemblyCode,assemblyCodeLength,lines); // Removes empty lines

    i=0;
    int memoryAddress=0;
    int error=0;

    // Fetches all label names and makes sure instructions are valid
    // Works line by line
    while(i<assemblyCodeLength) {
        char labelName[1024];
        int labelLength=0;
        char instrName[1024];
        int instrLength=0;
        int state=0;

        // Parses label name
        while(1) {
            if(assemblyCode[i]==' ')
                break;
            if(assemblyCode[i]=='\n' || i==assemblyCodeLength) {
                state=1;
                break;
            }
            labelName[labelLength++]=assemblyCode[i];
            i++;
        }
        labelName[labelLength]='\0';

        // Error checking
        if(labelLength!=0) {
            labelList[labelListLength]=new char[1024];
            labelAddresses[labelListLength]=memoryAddress;
            strcpy(labelList[labelListLength],labelName);
            labelListLength++;
        }

        // In case of a line ending, the instruction must be on the next line
        if(state==1) {
            i++;
            continue;
        }

        // Skips the space after the label
        i++;

        // Parses instruction name
        while(1) {
            if(assemblyCode[i]==' ')
                break;
            if(assemblyCode[i]=='\n' || i==assemblyCodeLength) {
                state=1;
                break;
            }
            instrName[instrLength++]=assemblyCode[i];
            i++;
        }
        instrName[instrLength]='\0';

        // In case of a line ending or end of file, we let the next while-loop handle the error
        if(state==1) {
            i++;
            continue;
        }

        // Error checking
        int instrType=getInstructionType(instrName);
        if(instrType==-1) {
            if(i>=assemblyCodeLength) {
                i=assemblyCodeLength-1;
            }
            sprintf(rawAssembly,"Invalid instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }
        else if(instrType==1 || instrType==2 || instrType==5) {
            memoryAddress++;
        }
        else {
            memoryAddress+=2;
        }

        // Skipping the rest of the line
        while(assemblyCode[i]!='\n' && i<assemblyCodeLength) i++;

        // Skip the \n
        i++;
    }

    // Error checking
    if(error) {
        delete[] lines;
        return;
    }

    // Back to the first character
    i=0;
    sprintf(rawAssembly,"ASM02");

    // Convert assembly to machine code
    while(i<assemblyCodeLength) {
        char instrName[1024];
        int instrLength=0;
        char instrArgs[1024];
        int instrArgsLength=0;
        int state=0;

        // Parses label name
        while(1) {
            if(assemblyCode[i]==' ')
                break;
            if(assemblyCode[i]=='\n' || i==assemblyCodeLength) {
                state=1;
                break;
            }
            i++;
        }

        // In case of a line ending, the instruction is on the next row
        if(state==1) {
            i++;
            continue;
        }

        // Skip the space after the label name
        i++;

        // Parses instruction name
        while(1) {
            if(assemblyCode[i]==' ')
                break;
            if(assemblyCode[i]=='\n' || i==assemblyCodeLength) {
                state=1;
                break;
            }
            instrName[instrLength++]=assemblyCode[i];
            i++;
        }
        instrName[instrLength]='\0';

        // In case of an empty instruction, just skip the line (very weird corner case)
        if(state==1 && instrLength==0) {
            i++;
            continue;
        }

        // In case of a non-empty instruction with arguments
        if(state==1) {
            if(i>=assemblyCodeLength) {
                i=assemblyCodeLength-1;
            }
            sprintf(rawAssembly,"Arguments expected for instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }

        // Error checking
        int instrType=getInstructionType(instrName);
        if(instrType==-1) {
            if(i>=assemblyCodeLength) {
                i=assemblyCodeLength-1;
            }
            sprintf(rawAssembly,"Invalid instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }

        // Skip the space after the instruction name
        i++;

        // Parses instruction arguments
        while(1) {
            if(assemblyCode[i]==' ')
                break;
            if(assemblyCode[i]=='\n' || i==assemblyCodeLength) {
                state=1;
                break;
            }
            instrArgs[instrArgsLength++]=assemblyCode[i];
            i++;
        }
        instrArgs[instrArgsLength]='\0';

        // Error checking
        if(state==1 && instrArgsLength==0) {
            if(i>=assemblyCodeLength) {
                i=assemblyCodeLength-1;
            }
            sprintf(rawAssembly,"Arguments expected for instruction %s on line %d\n",instrName,lines[i]);
            error=1;
            break;
        }

        // Tries to translate the instruction to machine code
        if(!translateInstruction(instrName,instrArgs,instrArgsLength,rawAssembly,lines[i])) {
            error=1;
            break;
        }

        // Skips the rest of the line (corner cases again)
        while(assemblyCode[i]!='\n' && i<assemblyCodeLength) i++;

        // Skips the new line character
        i++;
    }

    // Memory management
    delete[] lines;
}

/*
 * Finds the first occurence of "character" between "pos" and "argl" in "arg"
 */
int Assembler::findNextCharacter(char *arg, int argl, char character, int pos=0) {
    while(pos<argl) {
        if(arg[pos++]==character) {
            return pos-1;
        }
    }
    return -1;
}

/*
 * Converts a string representation of a register (Rxx) to a hexadecimal number
 * Returns 0 on success and non-zero on failure, check printError for more details
 */
int Assembler::getRegister(char *arg, int start, int end, char& reg) {
    int length=end-start;
    if(length<2 || length>3) {
        return 4;
    }
    if(arg[start]!='R') {
        return 1;
    }
    if(isdigit(arg[start+1])) {
        if(length==3 && isdigit(arg[start+2])) {
            int decimal=(arg[start+1]-'0')*10+(arg[start+2]-'0');
            if(decimal<0 || decimal>15) {
                return 4;
            }
            char temp[10];
            sprintf(temp,"%x",decimal);
            reg=temp[0];
        }
        else if(length==2) {
            reg=arg[start+1];
        }
        else {
            return 2;
        }
    }
    else {
        return 2;
    }
    return 0;
}

/*
 * Converts a string representation of a memory location (label[Rxx]) to a hexadecimal number and an address location
 * Returns 0 on success and non-zero on failure, check printError for more details
 */
int Assembler::getMemoryLocation(char *arg, int start, int end, char &reg, unsigned short &addr) {
    if(end-start<5) { // a[R0]
        return 4;
    }

    int openbracket=findNextCharacter(arg,end,'[',start);
    if(openbracket==-1) {
        return 4;
    }

    int closebracket=findNextCharacter(arg,end,']',openbracket+1);
    if(closebracket==-1) {
        return 4;
    }

    char lookFor[1024];
    int lookForLength=0;
    for(int i=start;i<openbracket;i++) {
        lookFor[lookForLength++]=arg[i];
    }
    lookFor[lookForLength]='\0';

    int result;
    if((result=getRegister(arg,openbracket+1,closebracket,reg))!=0) {
        return result;
    }

    if(lookForLength==0) {
        return 4;
    }
    if(isdigit(lookFor[0])) {
        int returnNumber=0;
        for(int i=0;i<lookForLength;i++) {
            if(!isdigit(lookFor[i])) {
                return 4;
            }
            returnNumber=returnNumber*10+(lookFor[i]-'0');
        }
        addr=returnNumber;
    }
    else if(lookFor[0]=='$') {
        if(lookForLength!=5) {
            return 4;
        }
        int returnNumber=0;
        for(int i=1;i<lookForLength;i++) {
            if(!(isdigit(lookFor[i]) || (lookFor[i]>='a' && lookFor[i]<='f'))) {
                return 4;
            }
            returnNumber=returnNumber*16+Emulator::hex2dec(lookFor[i]);
        }
        addr=returnNumber;
    }
    else if(lookFor[0]=='-') {
        int returnNumber=0;
        for(int i=1;i<lookForLength;i++) {
            if(!isdigit(lookFor[i])) {
                return 4;
            }
            returnNumber=returnNumber*10+(lookFor[i]-'0');
        }
        addr=-returnNumber;
    }
    else {
        addr=0xffff;
        for(int i=0;i<labelListLength;i++) {
            if(strcmp(lookFor,labelList[i])==0) {
                addr=labelAddresses[i];
                break;
            }
        }
        if(addr==0xffff) {
            return 5;
        }
    }
    return 0;
}
