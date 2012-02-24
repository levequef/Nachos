// synchconsole.h 
//	Data structures to simulate the behavior of a terminal
//	I/O device.  A terminal has two parts -- a keyboard input,
//	and a display output, each of which produces/accepts 
//	characters sequentially.
//
//	The console hardware device is asynchronous.  When a character is
//	written to the device, the routine returns immediately, and an 
//	interrupt handler is called later when the I/O completes.
//	For reads, an interrupt handler is called when a character arrives. 
//
//	The user of the device can specify the routines to be called when 
//	the read/write interrupts occur.  There is a separate interrupt
//	for read and write, and the device is "duplex" -- a character
//	can be outgoing and incoming at the same time.
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYNCCONSOLE_H
#define SYNCCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"


// The following class defines a hardware console device.
// Input and output to the device is simulated by reading 
// and writing to UNIX files ("readFile" and "writeFile").
//
// Since the device is asynchronous, the interrupt handler "readAvail" 
// is called when a character has arrived, ready to be read in.
// The interrupt handler "writeDone" is called when an output character 
// has been "put", so that the next character can be written.

class SynchConsole {
  public:
    SynchConsole(char *readFile, char *writeFile);
				// initialize the hardware console device
    ~SynchConsole();			// clean up console emulation

// external interface -- Nachos kernel code can call these
    void SynchPutChar(const char ch);	// Write "ch" to the console display, 
				// and return immediately.  "writeHandler" 
				// is called when the I/O completes. 

    //char SynchGetChar();	   	// Poll the console input.  If a char is 
				// available, return it.  Otherwise, return EOF.
    				// "readHandler" is called whenever there is 
				// a char to be gotten
	int SynchGetChar();
    void SynchPutString(const char *s);	// Write "s" to the console display, 
				// and return immediately.  "writeHandler" 
				// is called when the I/O completes. 

    void SynchGetString(char *s,int n);
    
    void SynchPutInt(int n);
    
    void SynchGetInt(int *n);
private : 
	Console *console;
	
};

#endif // SYNCHCONSOLE_H
