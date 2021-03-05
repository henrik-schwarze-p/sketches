
#ifndef Target_h
#define Target_h

#define MAX_NUMBER_OF_BUTTONS 2
#define MAX_NUMBER_OF_DEBUG_BUTTONS 0

#define TFT Elegoo_TFTLCD
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MAXY 920
#define TS_MINY 70

#define HR 320
#define VR 240

#include <stdint.h>
#include <stdlib.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define EPROM_SIZE 4096

// TFT SCREEN

void tftReset();
void tftRect(int x, int y, int w, int h, int c);
void tftPixel(int x, int y, int c);
void tftVline(int x, int y, int w, int c);
void tfHline(int x, int y, int l, int c);

// SERIALIZED INT POINTERS

int   pointerToInt(void* a);
void* intToPointer(int i);

// TOUCH SCREEN

int listenToTouchesOnScreen();
int getTouchX();
int getTouchY();
int touched();

// CONSOLE

void consoleln(unsigned long c);
void consoleln(const char* c);
void consoleln(float f);
void consoleln(int i);
void consoleln();
void console(const char* c);
void console(float f);
void console(int i);

// FINISHING

void terminate();

// RAM

void writeToHeap(int a, unsigned char c);

// EPROM
void          www(int a, unsigned char c);
unsigned char rrr(int address);

void writeByteToEEPROM(int address, unsigned char value);

void writeIntToEEPROM(int address, unsigned int value);

unsigned char readByteFromEEPROM(int address);

unsigned int readIntFromEEPROM(int address);

int random(int r);
#endif /* Target_h */
