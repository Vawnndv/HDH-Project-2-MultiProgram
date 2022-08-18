// pcb.h 
//  Process Control Block
//	Save all information to control the process
// All rights reserved.
#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

// Process Control Block
class PCB {
private:
	Semaphore* joinsem;	// semaphore cho join	
	Semaphore* exitsem;	// semaphore cho exit
	Semaphore* multex;	// semaphore cho truy xuat doc quyen  

	int exitcode;
	int numwait;		// number of process that have joined
				// so tien trinh da tham gia
	char FileName[32];	// chua ten tien trinh
	Thread* thread;		// tien trinh cua chuong trinh
public:
	int parentID;		// kiem tra neu la tien trinh cha
	char boolBG;		// kiem tra neu la tien trinh nen (background)

    
	PCB(int = 0);		// Contructor
	~PCB();			// Destructor

	int Exec(char*, int);	// Tao mot thread moi
	int GetID();		// Return processID cua tien trinh goi thuc hien
	int GetNumWait();	// Tra ve so luong tien trinh cho


	void JoinWait();	// 1. Tien trinh cha doi tien trinh con ket thuc 
	void JoinRelease();	// 2. Bao cho tien trinh cha thuc hien

	void ExitRelease();	// 3. Cho phep tien trinh con ket thuc
	void ExitWait();	// 4. Tien trinh con ket thuc
	
	void IncNumWait();	// Tang so tien trinh wait
	void DecNumWait();	// Giam so tien trinh wait

	void SetExitCode(int);	// Set exitcode cho tien trinh
	int GetExitCode();	// Return exitcode

	void SetFileName(char*);// Set ten tien trinh
	char* GetFileName();	// Return ten tien trinh
};

#endif // PCB_H
