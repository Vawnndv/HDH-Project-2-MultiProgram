// pcb.cc, "Process Control Block"
// All rights reserved.

#include "pcb.h"
#include "utility.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"


extern void StartProcess_2(int id);

PCB::PCB(int id) {
    	if (id == 0)
		this->parentID = -1;
	else
		this->parentID = currentThread->processID;

	this->numwait = this->exitcode = this->boolBG = 0;
	this->thread = NULL;
	this->joinsem = new Semaphore("joinsem",0);
	this->exitsem = new Semaphore("exitsem",0);
	this->multex = new Semaphore("multex",1);
}
PCB::~PCB() {
	
	if(joinsem != NULL)
		delete this->joinsem;
	if(exitsem != NULL)
		delete this->exitsem;
	if(multex != NULL)
		delete this->multex;
	if(thread != NULL) {		
		thread->FreeSpace();
		thread->Finish();	
	}
}
int PCB::GetID(){ return this->thread->processID; }
int PCB::GetNumWait() { return this->numwait; }
int PCB::GetExitCode() { return this->exitcode; }

void PCB::SetExitCode(int ec){ this->exitcode = ec; }

// Process tranlation to block
// Wait for JoinRelease to continue exec
void PCB::JoinWait(){	
	joinsem->P();	// Tien trinh chuyen sang block va stop, wait JoinRelease de thuc hien tiep
}

// JoinRelease process calling JoinWait
void PCB::JoinRelease() { 
	joinsem->V();	// De giai phong tien trinh goi JoinWait
}

// Let process tranlation to block state
// Waiting for ExitRelease to continue exec
void PCB::ExitWait() {
	// Nhu Join 
	exitsem->P(); // Chuyen sang block va stop, wait ExitRelease de thuc hien tiep
}

// Release wating process
void PCB::ExitRelease() {
	exitsem->V(); // Giai phong tien trinh dang cho
}

void PCB::IncNumWait() {
	multex->P();
	++numwait;
	multex->V();
}

void PCB::DecNumWait() {
	multex->P();
	if(numwait > 0)
		--numwait;
	multex->V();
}

void PCB::SetFileName(char* fn){ strcpy(FileName,fn);}
char* PCB::GetFileName() { return this->FileName; }

int PCB::Exec(char* filename, int id) {  
	multex->P();	// Tranh tinh trang nap 2 tien trinh cung 1 luc

	this->thread = new Thread(filename);
	
	// Kiem tra thread da khoi tao thanh cong hay chua
	// Neu chua bao loi khong du bo nho, goi multex->V() va return
	if(this->thread == NULL){
		printf("\nPCB::Exec:: Not enough memory..!\n");
        	multex->V();
		return -1;
	}

	// Gan processID cua thread la id.
	this->thread->processID = id;
	// Gan parentID cua thread la processID cua thread goi thuc thi Exec
	this->parentID = currentThread->processID;
	// Goi Fork(StartProcess_2,id)=>cast thread thanh int, xu ly ham StartProcess ta cast Thread ve dung kieu cua no
 	this->thread->Fork(StartProcess_2,id);

    	multex->V();
	return id;
}
