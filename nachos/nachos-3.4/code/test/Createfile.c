#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 32

int main () {
    
    int stdin;
    char fileName[MAX_LENGTH];
    PrintString("\n\t\t\t#####- CREATE FILE -#####\n\n");
    PrintString(" ENTER NAME OF FILE: ");

    stdin = Open("stdin", 2); // Call open to input file name
    if (stdin != -1) {
	int len = Read(fileName, MAX_LENGTH, stdin); // Read file name 

	if(len <= 0) {
	    PrintString(" INVALID FILE NAME!!!\n");
	    PrintString(" CREATE WITH THE FORMAT FILE NAME!!!\n");
	    if (CreateFile("Newfile.txt") == 0) {	
		PrintString("\n CREATE FILE DONE!!\n");
	    } else {
		PrintString("\n CREATE FILE ERROR!!\n");
	    }
	} else {
	    if (CreateFile(fileName) == 0) {
		PrintString("\n CREATE FILE DONE!!\n");
	    } else {
		PrintString("\n CREATE FILE ERROR!!\n");
	    }
	}
	Close(stdin);
    }
    return 0;
}
