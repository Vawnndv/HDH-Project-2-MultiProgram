#include "ptable.h"
#include "system.h"
#include "openfile.h"

#define For(i,a,b) for (int i = (a); i < b; ++i)

PTable::PTable(int size) {
	if (size < 0) return;

	psize = size;
	bm = new BitMap(size);
	bmsem = new Semaphore("bmsem",1);

	For(i,0,MAX_PROCESS){
		pcb[i] = 0;
	}

	bm->Mark(0);

	pcb[0] = new PCB(0);
	pcb[0]->SetFileName("./test/scheduler");
	pcb[0]->parentID = -1;
}

PTable::~PTable() {
	if(bm != 0) delete bm;
    
	For(i,0,psize) {
		if(pcb[i] != 0)
			delete pcb[i];
	}
	if( bmsem != 0)
		delete bmsem;
}


int PTable::ExecUpdate(char* name) {
	bmsem->P();	// Trach tinh trang nap 2 tien trinh cung 1 luc
	
	// Kiem tra tinh hop le cua chuong trinh "name"
	// Kiem tra ton tai bang cach goi phuong thuc Open cua lop fileSystem
	if (name == NULL) {
		printf("\nPTable::Exec : Can't not execute name is NULL.\n");
		bmsem->V();
		return -1;
	}

	
	// So sanh ten chuong trinh va ten cua currentThread, dam bao chuong trinh khong goi thuc thi chinh no
	if(strcmp(name,"./test/scheduler") == 0 || strcmp(name,currentThread->getName()) == 0) {
		printf("\nPTable::Exec : Can't not execute itself.\n");		
		bmsem->V();
		return -1;
	}	

	// Tim slot trong trong bang ptable
	int index = this->GetFreeSlot();


	// Truong hop khong con slot trong -> Bao loi
	if(index < 0) {
		printf("\nPTable::Exec :There is no free slot.\n");
		bmsem->V();
		return -1;
	}

	// Neu co slot trong thi khoi tao 1 new PCB voi processID chinh la index cua slot trong
	pcb[index] = new PCB(index);
	pcb[index]->SetFileName(name);

	// parrentID la processID cua currentThread
	pcb[index]->parentID = currentThread->processID;

	// Goi thuc thi phuong thuc Exec cua lop PCB
	int pid = pcb[index]->Exec(name,index);

	bmsem->V();
	return pid;
}

int PTable::JoinUpdate(int id) {
	// Kiem tra tinh hop le cua processID id
	if(id < 0) {
		printf("\nPTable::JoinUpdate : id = %d", id);
		return -1;
	}
	// Kiem tra tien trinh goi Join co phai parent cua tien trinh
	if(currentThread->processID != pcb[id]->parentID) {
		printf("\nPTable::JoinUpdate Can't join in process which is not it's parent process.\n");
		return -1;
	}


	// Tang numwait va goi JoinWait de wait tien trinh con thuc hien
	pcb[pcb[id]->parentID]->IncNumWait(); 
	pcb[id]->JoinWait();
	// Sau khi tien trinh con thuc hien xong, tien trinh duoc giai phong

	//pcb[id]->boolBG = 1;
	
	int ec = pcb[id]->GetExitCode();	// Xu ly exitcode
	pcb[id]->ExitRelease();			// Cho phep tien trinh con thoat

    	// Successfully
	return ec;
}

int PTable::ExitUpdate(int exitcode) {              

	// Neu tien trinh goi la main process thif Halt()
	int id = currentThread->processID;
	if(id == 0) {
		currentThread->FreeSpace();		
		interrupt->Halt();
		return 0;
	}
	
	// Kiem tra id co ton tai hay khong
        if(IsExist(id) == false) {
		printf("\nPTable::ExitUpdate: This %d is not exist. Try again?", id);
		return -1;
	}
    

	// Nguoc lai goi SetExitCode de cai exitcode cho tien trinh goi sau do giam tien trinh doi
	pcb[id]->SetExitCode(exitcode);
	pcb[pcb[id]->parentID]->DecNumWait();

	pcb[id]->JoinRelease();	// Giai phong tien trinh cha dang doi (neu co)
	pcb[id]->ExitWait();	// Xin tien trinh cha cho phep thoat

	Remove(id);
	return exitcode; 

}

// Tim slot trong de luu tru tien trinh moi
int PTable::GetFreeSlot() {
	return bm->Find();
}

// Kiem tra process ID co ton tai hay khong
bool PTable::IsExist(int pid) {
	return bm->Test(pid);
}

// Xoa processID khoi bang khi tien trinh ket thuc
void PTable::Remove(int pid) {
	bm->Clear(pid);
	if(pcb[pid] != 0)
		delete pcb[pid];
}

char* PTable::GetFileName(int id) {
	return (pcb[id]->GetFileName());
}

