#ifndef EMU_H
#define EMU_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned short int dbyte;

extern byte a;
extern byte x;
extern byte y;
extern byte flags;
extern dbyte pgc;
extern byte mem[65536];
extern byte s;
extern byte cycles;
extern int lastAccess;
extern byte* rom;
extern dbyte romLow;
extern dbyte romHigh;
extern byte romPresent;

void branch(byte b);
void addFlags(byte p);
void subFlags(byte b);
void cmpFlags(char c, byte b);
void ldFlags(char c);
int runcmd(void);
int loadData(char* fileName, dbyte low, dbyte high, byte asRom);
void reset();

#endif