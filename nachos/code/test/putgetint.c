#include "syscall.h"

void print(){
	int entier = 10;
	SynchPutInt(entier);
	SynchGetInt(&entier);
	SynchPutInt(entier);
}

int main(){
	print();
	return 3;
}
