// ptable.h 
//      Control process running include PCB table with MAX (10) element.
//      Constructor of PTable class will create parent process on 0 position.
//      From parent process, we will create other process by calling Exec().
// All rights reserved.

#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

#define MAX_PROCESS 10

class PTable {
private:
	int psize;
	BitMap *bm;			// danh dau cac vi tri da duoc su dung trong pcb
	PCB* pcb[MAX_PROCESS];
	Semaphore* bmsem;		// ngan chan truong hop nap 2 tien trinh cung luc

public:
	PTable(int = 10);		// Khoi tao size doi tuong pcb
					// De luu size process.
					// Gan gia tri ban dau la null.
	~PTable();			// Huy cac doi tuong da tao
		
	int ExecUpdate(char*);		// Xu ly cho system call SC_Exec
	int ExitUpdate(int);		// Xu ly cho system call SC_Exit
	int JoinUpdate(int);		// Xu ly cho system call SC_Join

	int GetFreeSlot();		// Tim free slot de luu thong tin cho tien trinh moi
	bool IsExist(int pid);		// Kiem tra ton tai cua processID
	void Remove(int pid);		// Khi tien trinh ket thuc, delete processID khoi mang quan ly no
	char* GetFileName(int id);	// return ten tien trinh

};
#endif // PTABLE_H

