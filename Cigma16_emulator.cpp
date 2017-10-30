#include<stdio.h>
#include<string>
#include "emulator.h"

void addToTrace(Emulator &emulator, FILE* f) {
	char temp[50];
    std::string string;
    sprintf(temp,"%d",emulator.executedInstructions);
    string.append(temp);
    string.append(".  ");
    string.append(emulator.lastOperation.c_str());
    string.append(" ");
    string.append(emulator.lastOperands.c_str());
    string.append("   Effect: ");
    string.append(emulator.lastEffect.c_str());
    string.append("   pc=");
    sprintf(temp,"%04hx",emulator.pc);
    string.append(temp);
    string.append(" ir=");
    sprintf(temp,"%04hx",emulator.ir);
    string.append(temp);
    string.append(" adr=");
    sprintf(temp,"%04hx",emulator.adr);
    string.append(temp);
    string.append(" dat=");
    sprintf(temp,"%04hx",emulator.dat);
    string.append(temp);
	fprintf(f,"%s\n",string.c_str());
}

int main(int argc, char* argv[]) {
	if(argc!=2 && argc!=4 && argc!=6) {
		fprintf(stderr,"Usage: c16emu input.asm [-o output.trace -v [017/144]]\n");
		return -1;
	}
	char inputFile[1024];
	inputFile[0]='\0';
	char outputFile[1024];
	outputFile[0]='\0';
	char dumpFile[1024];
	char* version=(char*)"017";
	int currArg=1;
	while(currArg<argc) {
		if(!strcmp(argv[currArg],"-o")) {
			if(currArg+1<argc) {
				strcpy(outputFile,argv[currArg+1]);
			}
			else {
				fprintf(stderr,"Usage: c16emu input.asm [-o output.trace -v [017/144]]\n");
				return -1;
			}
			currArg+=2;
		}
		else if(!strcmp(argv[currArg],"-v")) {
			if(currArg+1<argc) {
				version = argv[currArg+1];
			}
			else {
				fprintf(stderr,"Usage: c16emu input.asm [-o output.trace -v [017/144]]\n");
				return -1;
			}
			currArg+=2;
		}
		else {
			strcpy(inputFile,argv[currArg]);
			currArg++;
		}
	}
	if(inputFile[0]=='\0') {
		fprintf(stderr,"Usage: c16emu input.asm [-o output.trace -v [017/144]]\n");
		return -1;
	}
	if(outputFile[0]=='\0') {
		strcpy(outputFile,inputFile);
		strcat(outputFile,".trace");
	}
	strcpy(dumpFile,inputFile);
	strcat(dumpFile,".dump");
	if(strcmp(version,"017") && strcmp(version,"144")) {
		fprintf(stderr,"Usage: c16emu input.asm [-o output.trace -v [017/144]]\n");
		return -1;
	}
	FILE* input=fopen(inputFile,"r");
	if(input==NULL) {
		fprintf(stderr,"Could not open %s\n",inputFile);
		return -1;
	}
	FILE* output=fopen(outputFile,"w");
	if(output==NULL) {
		fprintf(stderr,"Could not open %s\n",outputFile);
		fclose(input);
		return -1;
	}
	FILE* dump=fopen(dumpFile,"w");
	if(dump==NULL) {
		fprintf(stderr,"Could not open %s\n",dumpFile);
		fclose(input);
		fclose(output);
		return -1;
	}
	char* buffer=new char[10485760];
	int length=fread(buffer,sizeof(char),1048576,input);
	buffer[length]='\0';
	std::string memory(buffer);
	Emulator emulator;
	emulator.loadMemory(memory);
	if(!strcmp(version,"017")) {
		while(emulator.step017()) {
			addToTrace(emulator,output);
			printf("%s",emulator.getOutput().c_str());
			if(emulator.pc==65535)
				break;
		}
	}
	else {
		while(emulator.step144()) {
			addToTrace(emulator,output);
			printf("%s",emulator.getOutput().c_str());
			if(emulator.pc==65535)
				break;
		}
	}
	fprintf(dump,"Register File:\n%s\n",emulator.getRegisterFile().c_str());
	fprintf(dump,"Memory File:\n%s\n",emulator.getMemoryFile().c_str());
	fclose(input);
	fclose(output);
	fclose(dump);
	return 0;
}