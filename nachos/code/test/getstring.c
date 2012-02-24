#include "syscall.h"

void print(){
	char chaine[120];
	SynchGetString(chaine,120);
	SynchPutString(chaine);
	
}

int main(){
	print();
	return 4;
}
