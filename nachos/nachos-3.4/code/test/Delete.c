#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 32

int main () {

    int stdin;
    char fileName[MAX_LENGTH];
    PrintString("\n\t\t\t#####- DELETE FILE -#####\n\n");
    PrintString(" ENTER NAME OF FILE: ");
    stdin = Open("stdin", 2); // Call open to input file name
    if (stdin != -1) {
	int len = Read(fileName, MAX_LENGTH, stdin); // Read file name 

	if(len <= 0) {
	    PrintString(" INVALID FILE NAME!!!\n");
	    return 0;
	} else {
	    if (Delete(fileName) == 0) {
		PrintString("\n DELETE FILE DONE!!\n");
	    } else {
		PrintString("\n DELETE FILE ERROR!!\n");
	    }
	}
	Close(stdin);
    }
    return 0;
}
