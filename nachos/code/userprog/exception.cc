// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synchconsole.h"	

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}
void copyStringFromMachine(int from, char *to, unsigned size){
	unsigned int i = 0;
	int val;
	char c;
	int f = from;
	do{
	 	machine->ReadMem(f,1,&val); //on lit la mémoire
		c = (char) val;
		to[i] = c;//on l'engistre a l'endroit
		i++; f++;
	}while(i < size && c != EOF && c != '\0');
	to[i] = '\0'; //on force l'ecrit d'un caractere de fin de chaine
}

void writeStringonMachine(int debut, char* tableau, int size){
	int d = debut;
	int i =0;
	while (i<size && tableau[i] != '\0' && tableau[i] != EOF)
	{
		machine->WriteMem(d,1,(int)tableau[i]); //on ecrit dans la memoire
		i++;
		d++;
	}
	machine->WriteMem(d,1,(int)'\0'); //on force l'ecriture de fin de chaine
}

//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------
void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);
    int adresse;
    int adr_string;
    int taille;
    char* tmp ;
    char* string;
    int retour;
    int valeur;
    int entier;
    if (which == SyscallException){
    	switch(type){
		case SC_Halt :
			DEBUG ('a', "Shutdown, initiated by user program.\n");
			printf("Shutdown, exception %d de type %d \n",which,type);
	  		interrupt->Halt ();
			break;
		case SC_Exit :
			retour = machine->ReadRegister(4); //on recupere la valeur du registre 4 (endroit ou est stocke la valeur de retour après modification
							//dans le fichier /test/start.S
			printf("\nla valeur retournee du programme est %u\n",retour); //on affiche la valeur
			interrupt->Halt ();//on arrete la machine
			break;
		case SC_PutChar :
			synchconsole->SynchPutChar((char)machine->ReadRegister (4));//le premier argument est dans le registre 4, on la recupere et on 
											//l'envoie a la fonction
			break;
		case SC_GetChar :
			machine->WriteRegister(2,(int)synchconsole->SynchGetChar()); //on recupere la valeur et on la stocke dans le registre utilise
											//pour  le retour d'une valeur de fonction
			break;
		case SC_SynchPutString :
			 adresse = machine->ReadRegister(4); //on recupere l'adresse où est la chaine
			 tmp = new char[MAX_STRING_SIZE+1];//on cree le buffer
			 copyStringFromMachine(adresse,tmp,MAX_STRING_SIZE);//on recupere la chaine de la memoire MIPS
			 synchconsole->SynchPutString(tmp); //on ecrit la chaine 
			 delete[] tmp;//on supprime le buffer temporaire
			break;
		case SC_SynchGetString :
			adr_string = machine->ReadRegister(4);//on recupere l'adresse de la chaine
			taille = machine->ReadRegister(5);//la taille de la chaine
			string = new char[MAX_STRING_SIZE+1];
			if (taille>MAX_STRING_SIZE){ taille = MAX_STRING_SIZE;}
			synchconsole->SynchGetString(string,taille);//on recupere la chaine 
			writeStringonMachine(adr_string,string,taille);//on l'ecrit dans le memoire MIPS
			delete[] string;
			break;
		case SC_SynchPutInt : 
			valeur = machine->ReadRegister(4); //on recupere la valeur
			synchconsole->SynchPutInt(valeur);//on l'ecrit
			break;
		case SC_SynchGetInt :
			adresse = machine->ReadRegister(4); //on recupere l'adresse ou doit etre enregistre la valeur
			synchconsole->SynchGetInt(&entier);//on recupere la valeur et on l'ecrit
			machine->WriteMem(adresse,sizeof(int),entier);//on ecrit dans la memoire mips
			break;
		default :
		printf ("Unexpected user mode exception %d %d\n", which, type);
	  	ASSERT (FALSE);	
	}
    }
    // LB: Do not forget to increment the pc before returning!
    	UpdatePC ();
    // End of addition
/*
    if ((which == SyscallException) && (type == SC_Halt))
      {
	  DEBUG ('a', "Shutdown, initiated by user program.\n");
	  interrupt->Halt ();
      }
    else
      {
	  printf ("Unexpected user mode exception %d %d\n", which, type);
	  ASSERT (FALSE);
      }
*/
}
