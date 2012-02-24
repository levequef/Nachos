#include "syscall.h"

void print(){
	char c = GetChar();
	PutChar(c);

}

int main (){

	print();
	return 3;
}
