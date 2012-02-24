#include "syscall.h"

void print(const char c[])
{
	SynchPutString(c);
}

int main(){
	print("aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbb\n");
	return 5;
}
