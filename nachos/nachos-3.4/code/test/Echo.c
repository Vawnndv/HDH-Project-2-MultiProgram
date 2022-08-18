#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 32

int main () {

    int stdin;
    int stdout;
    char buffer[MAX_LENGTH];
    int len;
    PrintString("\n\t\t\t#######- ECHO -#######\n\n");
    PrintString("Enter: ");

    stdin = Open("stdin", 2);
    if (stdin != -1) {
	len = Read(buffer, MAX_LENGTH, stdin);

	if (len != -1 && len != -2) {// Check error of EOF
	    stdout = Open("stdout", 3);
	    if (stdout != -1) {
		PrintString("Echo: ");
		Write(buffer, len, stdout); 
		Close(stdout);
	    }
	}
	Close(stdin);
    }
    PrintString("\n\n");
    return 0;
}
