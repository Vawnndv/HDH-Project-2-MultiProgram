#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32 // Do dai quy uoc cho file name

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
	
// Tang program counter - Doi thanh ghi ve sau 4 bytes
void IncreasePC()
{
	machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
	machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
	machine->WriteRegister(NextPCReg, machine->ReadRegister(NextPCReg) + 4);
}
	
// char* User2System
// Input: Space addr User(int), limit of buffer(int)
// Output: buffer(char*) 
// Purpose: copy buffer from User memory space to System memory space
char* User2System(int virtAddr, int limit) {
    int i; // index
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit + 1];
    if (kernelBuf == NULL)
	return kernelBuf;

    memset(kernelBuf, 0, limit+1);
    
    for (i=0; i<limit; i++) {
	machine->ReadMem(virtAddr + i, 1, &oneChar);
	kernelBuf[i] = (char)oneChar;
	if (oneChar == 0)
	    break;
    }
    return kernelBuf;
}
	
	
// int System2User
// Input: Space addr User(int),limit if buffer(int), buffer(char*)
// Output: Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char* buffer){
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
	oneChar = (int)buffer[i];
	machine->WriteMem(virtAddr + i, 1, oneChar);
	i++;
	} while (i < len && oneChar != 0);
	return i;
}
	
	
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which) {
    case NoException:
        interrupt->setStatus(SystemMode);
        DEBUG('a', "\nEverything OK !!!. Switch to System Mode\n");
        break;
    case PageFaultException:
        DEBUG('a', "\nNo valid translation found");
        printf("\n\nNo valid translation found.\n");
        printf("The Exception type: %d occurs\n", which);
        ASSERT(false);
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "\nWrite attempted to page marked \"read only\"");
        printf("\n\nWrite attempted to page marked \"read only\"\n");
        printf("The Exception type: %d occurs\n", which);
        ASSERT(false);
        interrupt->Halt();
        break;
    case BusErrorException:
        DEBUG('a', "\nTranslation result invalid physical address");
        printf("\n\nTranslation result invalid physical address.\n");
        printf("The Exception type: %d occurs\n", which);
        ASSERT(false);
        interrupt->Halt();
        break;
    case AddressErrorException:
        DEBUG('a', "\nUnaligned reference or one that was beyond the end of the address space");
        printf("\n\nUnaligned reference or one that was beyond the end of the address space\n");
        printf("The Exception type: %d occurs\n", which);
        ASSERT(false);
        interrupt->Halt();
        break;
    case OverflowException:
        DEBUG('a', "\nInteger overflow in add or sub");
        printf("\n\nInteger overflow in add or sub.\n");
        printf("The Exception type: %d occurs\n", which);
        ASSERT(false);
        interrupt->Halt();
        break;
    case IllegalInstrException:
        DEBUG('a', "\nUnimplemented or reserved instr");
        printf("\n\nUnimplemented or reserved instr.\n");
        printf("The Exception type: %d occurs\n", which);
        ASSERT(false);
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        DEBUG('a', "\nNumber Exception Types");
        printf("\n\nNumber Exception Types.\n");
        printf("The Exception type: %d occurs\n", which);
        ASSERT(false);
        interrupt->Halt();
        break;
    case SyscallException:
	switch (type){
	case SC_Halt:
		// Input: None
		// Ouput: Thong bao dung chuong trinh
		// Purpose: Dung chuong trinh
		DEBUG('a', "\nShutdown, initiated by user program. ");
		printf("\nShutdown, initiated by user program. ");
		interrupt->Halt();
		return;
	
	case SC_ReadChar: {
		// Input: None
		// Output: 1 character
		// Purpose: Doc 1 ky tu do nguoi dung nhap
		int maxBytes = 255;
		char* buffer = new char[255];
		int numBytes = gSynchConsole->Read(buffer, maxBytes);
	
		// Case input more than 1 character -> Invalid
		if(numBytes > 1) {
		    printf("Chi duoc nhap duy nhat 1 ky tu!");
		    DEBUG('a', "\nERROR: Chi duoc nhap duy nhat 1 ky tu!");
		    machine->WriteRegister(2, 0);
		}
		else if(numBytes == 0) { //Ky tu rong
		    printf("Ky tu rong!");
		    DEBUG('a', "\nERROR: Ky tu rong!");
		    machine->WriteRegister(2, 0);
		}
		else {
		    //Chuoi vua lay co dung 1 ky tu, lay ky tu tai index = 0, return vao reg2
		    char c = buffer[0];
		    machine->WriteRegister(2, c);
		}
		delete buffer;
		//IncreasePC(); // error system
		//return;
		break;
	}
	
	case SC_PrintChar: {
		// SC_PrintChar
		// Input: mot ky tu tu reg 4
		// Output: mot ky tu
		// Purpose: in mot ky tu ra man hinh
		char c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
		gSynchConsole->Write(&c, 1); // In ky tu tu bien c, 1 byte
		break;
	}
	
	case SC_ReadString: {
		// Input: Buffer(char*), do dai toi da cua chuoi nhap vao(int)
		// Output: Khong co
		// Cong dung: Doc vao mot chuoi voi tham so la buffer va do dai toi da
		int virtAddr, length;
		char* buffer;
		// Lay dia chi tham so buffer truyen vao tu reg4
		virtAddr = machine->ReadRegister(4); 
		// Lay length cua chuoi nhap vao tu reg5
		length = machine->ReadRegister(5); 
		// Copy chuoi tu User memory space sang System memory space
		buffer = User2System(virtAddr, length); 
		// Goi ham Read cua SynchConsole de doc chuoi
		gSynchConsole->Read(buffer, length); 
		// Copy chuoi tu System memory space sang User memory space
		System2User(virtAddr, length, buffer); 
		delete buffer;
		IncreasePC(); // Tang Program Counter
		return;
	}
	
	case SC_PrintString: {
		// Input: buffer(char*)
		// Output: Chuoi doc duoc tu buffer(char*)
		// Cong dung: Xuat mot chuoi (buffer) truyen vao ra man hinh
		int virtAddr;
		char* buffer;
		// Lay dia chi cua tham so buffer tu reg4
		virtAddr = machine->ReadRegister(4); 
		// Copy chuoi User memory space sang System memory spacepace voi buffer dai 255 ki tu
		buffer = User2System(virtAddr, 255); 
		int length = 0;
		// Dem do dai that cua chuoi
		while (buffer[length] != 0) length++; 
		// Goi ham Write cua SynchConsole de in chuoi
		gSynchConsole->Write(buffer, length + 1); 
		delete buffer;
		break;
	}
	
	case SC_Create: {
		// Input: Dia chi tu User memory  spacua ten file
		// Output: -1 = Loi, 0 = Thanh cong
		// Chuc nang: Tao ra file voi tham so la ten file
		int virtAddr;
		char* filename;
		DEBUG('a', "\n SC_CreateFile call ...");
		DEBUG('a', "\n Reading virtual address of filename");
	
		virtAddr = machine->ReadRegister(4); //Doc dia chi cua file tu thanh ghi R4
		DEBUG('a', "\n Reading filename.");
	
		//Sao chep khong gian bo nho User sang System, voi do dai toi da la (32 + 1) bytes
		filename = User2System(virtAddr, MaxFileLength + 1);
		if (strlen(filename) == 0) {
		    printf("\n File name is not valid");
		    DEBUG('a', "\n File name is not valid");
		    machine->WriteRegister(2, -1); //Loi return -1 vo reg2
		    break;
		}
		//Neu khong doc duoc
		if (filename == NULL) {
		    printf("\n Not enough memory in system");
		    DEBUG('a', "\n Not enough memory in system");
		    machine->WriteRegister(2, -1); //Loi return -1 vo reg2
		    delete filename;
		    break;
		}
		DEBUG('a', "\n Finish reading filename.");
		//Tao file bang ham Create cua fileSystem, tra ve ket qua
		if (!fileSystem->Create(filename, 0)) {
		     //Tao file that bai
		    printf("\n Error create file '%s'", filename);
		    machine->WriteRegister(2, -1);
		    delete filename;
		    break;
		}
		//Tao file thanh cong
		machine->WriteRegister(2, 0);
		delete filename;
		//IncreasePC(); //Day thanh ghi lui ve sau de tiep tuc ghi
		//return;
		break;
	}
	
	case SC_Open: {
		// Input: arg1: Dia chi cua chuoi name, arg2: type
		// Output: Tra ve OpenFileID = thanh cong, -1 = loi
		// Chuc nang: Tra ve ID cua file.	
		//OpenFileID Open(char *name, int type)
	
		// Lay dia chi cua filename tu reg4
		int virtAddr = machine->ReadRegister(4); 
		// Lay type tu reg5
		int type = machine->ReadRegister(5); 
		char* filename;
		// Copy chuoi User memory space sang System memory space voi filename dai MaxFileLength
		filename = User2System(virtAddr, MaxFileLength); 
		//Kiem tra xem OS con mo dc file khong
		int freeSlot = fileSystem->FindFreeSlot();
		// Xu li khi con slot trong
		if (freeSlot != -1) {
		    // Truong hop type=0 (read&write) and type=1 (readonly)
		    if (type == 0 || type == 1) {
			if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) {
			    machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
			}
		    }
		    else if (type == 2) { // Xu li stdin voi type = 2
			    machine->WriteRegister(2, 0); //tra ve OpenFileID
		    }
		    else { // xu li stdout voi type = 3
			    machine->WriteRegister(2, 1); //tra ve OpenFileID
		    }
		    delete[] filename;
		    break;
		}
		machine->WriteRegister(2, -1); //Khong mo duoc file loi return -1
		delete[] filename;
		break;
	}
	
	case SC_Close: {
		// Input: OpenFileId id
		// Output: success 0; failed -1
		// Purpose: Dong file voi id cua file
		int id = machine->ReadRegister(4); // Lay id cua file tu reg4
		if (id >= 0 && id <= 9) { // Xu li khi id nam trong khoang [0, 9]
		    if (fileSystem->openf[id]) { // Mo file thanh cong
			delete fileSystem->openf[id]; //Xoa vung nho luu tru file
			fileSystem->openf[id] = NULL; //Gan vung nho NULL
			machine->WriteRegister(2, 0);
			break;
		    }
		}
		machine->WriteRegister(2, -1);
		break;
	}
	case SC_Read: {
		// Input: buffer(char*), size(int), id(OpenFileId)
		// Output: -1: Loi, So byte read thuc su: Thanh cong, -2: Cuoi file
		// Cong dung: Doc file voi tham so buffer, kich thuoc cho phep va id cua file
		// int Read(char *buffer, int size, OpenFileId id);

		// Lay dia chi cua buffer tu reg4
		int virtAddr = machine->ReadRegister(4); 
		// Lay charcount tu reg5
		int charcount = machine->ReadRegister(5); 
		// Lay id cua file tu reg6
		int id = machine->ReadRegister(6); 
		int OldPos;
		int NewPos;
		char *buffer;
		// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
		if (id < 0 || id > 9) {
		    printf("\nError: Unable to read file cause file's id invalid.");
		    machine->WriteRegister(2, -1); // Loi ghi -1 vao reg2
		    IncreasePC();
		    return;
		}
		// Kiem tra file co ton tai khong
		if (fileSystem->openf[id] == NULL) {
		    printf("\nError: File is not exist.");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Xet truong hop doc file stdout (type = 3) thi tra ve -1
		if (fileSystem->openf[id]->type == 3) {
		    printf("\nError: Unable to read file stdout.");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Kiem tra thanh cong thi lay vi tri OldPos
		OldPos = fileSystem->openf[id]->GetCurrentPos(); 
		// Copy chuoi tu User memory space sang System memory space voi buffer dai charcount
		buffer = User2System(virtAddr, charcount); 
		// Xet truong hop doc file stdin (type quy uoc la 2)
		if (fileSystem->openf[id]->type == 2) {
		    // Su dung ham Read cua lop SynchConsole de tra ve so byte thuc su doc duoc
		    int size = gSynchConsole->Read(buffer, charcount);
		    // Copy chuoi tu System Space sang User Space voi buffer co do dai la so byte thuc su
		    System2User(virtAddr, size, buffer); 
		    machine->WriteRegister(2, size); // Tra ve so byte thuc su doc duoc
		    delete buffer;
		    IncreasePC();
		    return;
		}
		// Xet truong hop doc file binh thuong thi tra ve so byte thuc su
		if ((fileSystem->openf[id]->Read(buffer, charcount)) > 0) {
		    // So byte thuc su = NewPos - OldPos
		    NewPos = fileSystem->openf[id]->GetCurrentPos();
		    // Copy chuoi tu System sang User voi buffer co do dai la so byte thuc su
		    System2User(virtAddr, NewPos - OldPos, buffer);
		    machine->WriteRegister(2, NewPos - OldPos);
		}
		else {
		    // Truong hop con lai la doc file co noi dung la NULL tra ve -2
		    printf("\nFile is empty.");
		    machine->WriteRegister(2, -2);
		}
		delete buffer;
		IncreasePC();
		return;
	}
	
	case SC_Write: {
		// Input: buffer(char*), charcount(int), id(OpenFileID)
		// Output: -1: Loi, So byte write thuc su: Thanh cong, -2: Thanh cong
		// Cong dung: Ghi file voi tham so la buffer, so ky tu cho phep va id cua file

		// Lay dia chi buffer tu reg4
		int virtAddr = machine->ReadRegister(4); 
		// Lay charcount tu reg5
		int charcount = machine->ReadRegister(5); 
		// Lay id cua file tu reg6
		int id = machine->ReadRegister(6); 
		int OldPos;
		int NewPos;
		char *buffer;
		// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong [0, 9]
		if (id < 0 || id > 9) {
		    printf("\n\nError: Unable to write cause file's id invalid");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Kiem tra file co ton tai khong
		if (fileSystem->openf[id] == NULL) {
		    printf("\n\nError: File is not exist.");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Xet truong hop only read (type=1) hoac file stdin (type=2) thi tra ve -1
		if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2) {
		    printf("\n\nError: Can not write file stdin or read only.");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Kiem tra thanh cong thi lay vi tri OldPos
		OldPos = fileSystem->openf[id]->GetCurrentPos(); 
		// Copy chuoi tu User memory space sang System memory space voi buffer dai charcount
		buffer = User2System(virtAddr, charcount); 
		// Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su
		if (fileSystem->openf[id]->type == 0) {
		    if ((fileSystem->openf[id]->Write(buffer, charcount)) > 0) {
			// So byte thuc su = NewPos - OldPos
			NewPos = fileSystem->openf[id]->GetCurrentPos();
			machine->WriteRegister(2, NewPos - OldPos);
			delete buffer;
			IncreasePC();
			return;
		    }
		}
		// Xet truong hop ghi file stdout (type=3)
		if (fileSystem->openf[id]->type == 3) {
		    int i = 0;
		    while (buffer[i] != 0 && buffer[i] != '\n') { 
			// Vong lap de write den khi gap ky tu '\n'
			gSynchConsole->Write(buffer + i, 1); // Su dung ham Write cua lop SynchConsole
			i++;
		    }
		    buffer[i] = '\n';
		    gSynchConsole->Write(buffer + i, 1); // Write ky tu '\n'
		    machine->WriteRegister(2, i - 1); // Tra ve so byte thuc su write duoc
		    delete buffer;
		    IncreasePC();
		    return;
		}
	}
	
	case SC_Seek: {
		// Input: pos(int), id(OpenFileId)
		// Output: -1: Loi, pos: Thanh cong
		// Cong dung: Di chuyen pointer den vi tri thich hop trong file voi tham so la pos va id cua file
		// Lay pos can chuyen con tro den trong file o reg4
		int pos = machine->ReadRegister(4); 
		// Lay id cua file o reg5
		int id = machine->ReadRegister(5); 
		// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong [0, 9]
		if (id < 0 || id > 9) {
		    printf("\nCan not seek cause id is invalid.");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Kiem tra file co ton tai khong
		if (fileSystem->openf[id] == NULL) {
		    printf("\nSeek failed. File is not exist.");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Kiem tra co goi Seek tren console khong
		if (id == 0 || id == 1) {
		    printf("\nCan not seek on file console.");
		    machine->WriteRegister(2, -1);
		    IncreasePC();
		    return;
		}
		// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
		// if (pos == -1) pos = fileSystem->openf[id]->Length();
		pos = (pos == -1) ? fileSystem->openf[id]->Length() : pos;
		if (pos > fileSystem->openf[id]->Length() || pos < 0) {
		    // Kiem tra pos co hop le khong
		    printf("\nKhong the seek file den vi tri nay.");
		    machine->WriteRegister(2, -1);
		}
		else {
		    // Neu hop le thi tra ve pos thuc su trong file
		    fileSystem->openf[id]->Seek(pos);
		    machine->WriteRegister(2, pos);
		}
		IncreasePC();
		return;
	}
	
	case SC_Delete: {
		int virtAddr;
        	char* filename;
		DEBUG('a', "\n System Call DeleteFile ...");
		DEBUG('a', "\n Reading virtual address of filename");
		virtAddr = machine->ReadRegister(4); //Read addr file from reg4
		DEBUG('a', "\n Reading filename.");
		int id = machine->ReadRegister(4);
		if (fileSystem->openf[id] != NULL) //neu mo file thanh cong
		{
		        delete fileSystem->openf[id]; //Xoa vung nho luu tru file
		        fileSystem->openf[id] = NULL; //Gan vung nho NULL
		        printf("\n File is opening");
		        DEBUG('a', "\n File is opening");
		        machine->WriteRegister(2, -1);
		        return;
		}
		// Copy from User memory space to System memory space with max (32+1) bytes
		filename = User2System(virtAddr, MaxFileLength + 1);
		// No file's name ?
		if (strlen(filename) == 0) {
		        printf("\n File name is not valid");
		        DEBUG('a', "\n File name is not valid");
		        machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
		        return;
		}

		if (!fileSystem->Remove(filename)) {
		        // Delete failed
		        printf("\n Error delete file '%s'", filename);
		        machine->WriteRegister(2, -1);
		        delete filename;
		        return;
		}
		machine->WriteRegister(2, 0);
		delete filename;
		break;  
	}
	case SC_Exec: {
		// Input: virtAddr(int)
		// Output: Fail return -1, Success: return id cua thread dang chay
		// SpaceId Exec(char *name);
		int virtAddr;
		virtAddr = machine->ReadRegister(4); // doc dia chi ten chuong trinh tu reg4
		char* name;
		name = User2System(virtAddr, MaxFileLength + 1); // Lay ten chuong trinh, nap vao kernel
	
		if(name == NULL) {
			DEBUG('a', "\n Not enough memory in System");
			printf("\n Not enough memory in System");
			machine->WriteRegister(2, -1);
			//IncreasePC();
			return;
		}
		OpenFile *oFile = fileSystem->Open(name);
		if (oFile == NULL) {
			printf("\nExec:: Can't open this file.");
			machine->WriteRegister(2,-1);
			IncreasePC();
			return;
		}
	
		delete oFile;
	
		// Return child process id
		int id = pTab->ExecUpdate(name);
		machine->WriteRegister(2,id);
	
		delete[] name;
		IncreasePC();
		return;
	}
	case SC_Join: {
		// int Join(SpaceId id)
		// Input: id dia chi cua thread
		// Output:
		int id = machine->ReadRegister(4);
	
		int res = pTab->JoinUpdate(id);
	
		machine->WriteRegister(2, res);
		IncreasePC();
		return;
	}
	case SC_Exit: {
		//void Exit(int status);
		// Input: status code
		int exitStatus = machine->ReadRegister(4);
	
		if(exitStatus != 0) {
			IncreasePC();
			return;
		}
	
		int res = pTab->ExitUpdate(exitStatus);
		//machine->WriteRegister(2, res);
	
		currentThread->FreeSpace();
		currentThread->Finish();
		IncreasePC();
		return;
	}	
	default:
	break;
	}
	IncreasePC();
    }
}
