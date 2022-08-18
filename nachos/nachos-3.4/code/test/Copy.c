#include "syscall.h"
#define MAX_LENGTH 32

int main() {
    //OpenFileId of source and dest file
    int srcFileId;
    int destFileId;
    //size of file
    int fileSize;
    int i;
    char c;
    char source[MAX_LENGTH];
    char dest[MAX_LENGTH];

    PrintString("\n\t\t\t#####- COPY FILE -#####\n\n");
    PrintString(" Enter source file: ");
    ReadString(source, MAX_LENGTH); // Call ReadString to read source file

    PrintString(" Enter destination file: ");
    ReadString(dest, MAX_LENGTH); // Call ReadString to read dest file
    srcFileId = Open(source, 1);

    if (srcFileId != -1) // Check file is open success
    {
	destFileId = CreateFile(dest);
	Close(destFileId);

	destFileId = Open(dest, 0);
	if (destFileId != -1) // Check file is open success
	{
	    // Seek to EOF to get size of file (fileSize)
	    fileSize = Seek(-1, srcFileId);

	    // Start copy
	    Seek(0, srcFileId); // Seek to begin of file
	    Seek(0, destFileId); // Seek to end of file
	    i = 0;

	    // Loop begin to end of source file
	    for (; i < fileSize; i++)
	    {
		Read(&c, 1, srcFileId); //Read a char of source file
		Write(&c, 1, destFileId); //Write a char to dest file
	    }

    	PrintString(" -> Copy successful.\n\n");
    	Close(destFileId);
    	}
    	else
    	{
  	    PrintString(" -> Create destination file successful!!!\n\n");
    	}

        Close(srcFileId);
    }
    else
    {
	PrintString("Error open file!!\n");
    }

    return 0;
}
