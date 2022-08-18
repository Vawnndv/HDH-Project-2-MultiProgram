#include "syscall.h"
#define MAX_LENGTH 32
	
int main() {
    int openFileId;
    int fileSize;
    char c;
    char fileName[MAX_LENGTH];
    int i; //Index for loop
    PrintString("\n\t\t\t######- CAT FILE -######\n\n");
    PrintString(" Enter name of file: ");

    ReadString(fileName, MAX_LENGTH);
    openFileId = Open(fileName, 1); // Call Open file

    if (openFileId != -1) //Check error
    {
    	//Get size of file
	fileSize = Seek(-1, openFileId);
	i = 0;
	// Seek to begin file to read
	Seek(0, openFileId);

	PrintString("   Contents file:\n");
	for (; i < fileSize; i++)
	{
	    Read(&c, 1, openFileId);
	    PrintChar(c);
	}
	Close(openFileId);
    }
    else
    {
	PrintString(" -> File opening failed!!\n\n");
    }
    return 0;
}
