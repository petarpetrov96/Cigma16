#include<stdio.h>
#include<string.h>
#include "assembler017.h"
#include "assembler144.h"
int main(int argc, char* argv[]) {
	if(argc!=2 && argc!=4 && argc!=6) {
		fprintf(stderr,"Usage: c16asm input.asm.txt [-o output.asm -v [017/144]]\n");
		return -1;
	}
	char inputFile[1024];
	inputFile[0]='\0';
	char outputFile[1024];
	outputFile[0]='\0';
	char* version=(char*)"017";
	int currArg=1;
	while(currArg<argc) {
		if(!strcmp(argv[currArg],"-o")) {
			if(currArg+1<argc) {
				strcpy(outputFile,argv[currArg+1]);
			}
			else {
				fprintf(stderr,"Usage: c16asm input.asm.txt [-o output.asm -v [017/144]]\n");
				return -1;
			}
			currArg+=2;
		}
		else if(!strcmp(argv[currArg],"-v")) {
			if(currArg+1<argc) {
				version = argv[currArg+1];
			}
			else {
				fprintf(stderr,"Usage: c16asm input.asm.txt [-o output.asm -v [017/144]]\n");
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
		fprintf(stderr,"Usage: c16asm input.asm.txt [-o output.asm -v [017/144]]\n");
		return -1;
	}
	if(outputFile[0]=='\0') {
		strcpy(outputFile,inputFile);
		if(!strcmp(inputFile+strlen(inputFile)-8,".asm.txt")) {
			outputFile[strlen(outputFile)-4]='\0';
		}
		else {
			
			strcat(outputFile,".asm");
		}
	}
	if(strcmp(version,"017") && strcmp(version,"144")) {
		fprintf(stderr,"Usage: c16asm input.asm.txt [-o output.asm -v [017/144]]\n");
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
	char* buffer=new char[10485760];
	int length=fread(buffer,sizeof(char),1048576,input);
	if(!strcmp(version,"017")) {
		Assembler017 assembler(buffer,length);
		assembler.assemble();
		strcpy(buffer,assembler.readAssembly().c_str());
	}
	else {
		Assembler144 assembler(buffer,length);
		assembler.assemble();
		strcpy(buffer,assembler.readAssembly().c_str());
	}
	char beginning[6];
	memcpy(beginning,buffer,5);
	beginning[5]='\0';
	if(strcmp(beginning,"ASM03")) {
		fprintf(stderr,"%s",buffer);
	}
	else {
		fprintf(output,"%s",buffer);
	}
	delete[] buffer;
	fclose(input);
	fclose(output);
	return 0;
}