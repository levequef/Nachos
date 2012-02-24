// console.cc 
//	Routines to simulate a serial port to a console device.
//	A console has input (a keyboard) and output (a display).
//	These are each simulated by operations on UNIX files.
//	The simulated device is asynchronous,
//	so we have to invoke the interrupt handler (after a simulated
//	delay), to signal that a byte has arrived and/or that a written
//	byte has departed.
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"
#include "synchconsole.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *put_taken;
static Semaphore *get_taken;


static void ReadAvail(int arg) { readAvail->V();}
static void WriteDone(int arg) { writeDone->V();}
//----------------------------------------------------------------------
// SynchConsole::SynchConsole
// 	Initialize the simulation of a hardware console device.
//
//	"readFile" -- UNIX file simulating the keyboard (NULL -> use stdin)
//	"writeFile" -- UNIX file simulating the display (NULL -> use stdout)
// 	"readAvail" is the interrupt handler called when a character arrives
//		from the keyboard
// 	"writeDone" is the interrupt handler called when a character has
//		been output, so that it is ok to request the next char be
//		output
//----------------------------------------------------------------------

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail",0);
	writeDone = new Semaphore("write done",0);
	put_taken = new Semaphore("Put taken",1);
	get_taken = new Semaphore("Get taken",1);
	console = new Console(readFile,writeFile,ReadAvail,WriteDone,0);
}

//----------------------------------------------------------------------
// SynchConsole::~SynchConsole
// 	Clean up console emulation
//----------------------------------------------------------------------

SynchConsole::~SynchConsole()
{
    delete console;
    delete writeDone;
    delete readAvail; 
}

/*char SynchConsole::SynchGetChar()
{
	char c;
	get_taken->P(); //eviter les accès concurrents
   	readAvail->P(); // attendre l'entree de caractere
	c= console ->GetChar();//lire de caractere et le retourner
	get_taken->V(); //on libere le semaphore
	return c; 
	
}
*/
int SynchConsole::SynchGetChar()
{
	int c;
	get_taken->P();
   	readAvail->P();//attente d'un caractere
	c = console ->GetChar();//on le recupere
	get_taken->V();
	return c;
}

//----------------------------------------------------------------------
// SynchConsole::SynchPutChar()
// 	Write a character to the simulated display, schedule an interrupt 
//	to occur in the future, and return.
//----------------------------------------------------------------------

void
SynchConsole::SynchPutChar(char ch)
{
	put_taken->P(); //eviter les accès concurrents
	console ->PutChar(ch);  //on envoit le caractere
	writeDone->P();//on attend que l'ecriture soit faite
	put_taken->V();//on libere le semaphore
}

void SynchConsole::SynchPutString(const char s[])
{
	put_taken->P(); //eviter les accès concurrents
	int i=0;
   	char c ;
	c= s[i]; //on lit le premier caractere
   	while (c != '\0' && c!= EOF){ 
   		console ->PutChar(c); //on envoit le caractere
		writeDone->P();//on attend la fin de l'ecriture
		i++;
		c=s[i];
  	}
	console->PutChar('\0');
	writeDone->P();
	put_taken->V();
}

//----------------------------------------------------------------------
// SynchConsole::PutChar()
// 	Write a character to the simulated display, schedule an interrupt 
//	to occur in the future, and return.
//----------------------------------------------------------------------

void SynchConsole::SynchGetString(char *s, int n)
{
	get_taken->P();
	readAvail->P(); //attente d'entree
   	char c=console ->GetChar(); //on le recupere
	int i = 0;
	s[i] = c;
	i++;
   	while (i < (n-1) && c != EOF && c !='\n'&& c !='\0' && c!='\r'){//on s'arrete si EOF, on a atteint la taille, fin de caractere, retour a la ligne
									//,retour chariot
		readAvail->P();
		c = console ->GetChar();
		if (c != EOF && c !='\0') //si retour chariot ou retour a la ligne, on l'ecrit avant de s'arreter
		{			//on s'arrete si fin de fichier ou de chaine
			s[i] = c;
			i++;  
		} 
	}
	s[i] = '\0';
	get_taken->V();
}

void SynchConsole::SynchPutInt(int n)
{
	char c[10];
	snprintf(c,10,"%d",n); //on copie l'entier n dans c 
	synchconsole->SynchPutString(c);//on ecrit le nombre
	synchconsole->SynchPutChar('\n');
}

void SynchConsole::SynchGetInt(int *n)
{
	char c[10];
	synchconsole->SynchGetString(c,10);//on recupere le string 
	sscanf(c,"%d",n);//on enregistre le chiffre à l'endroit pointé par n
}
