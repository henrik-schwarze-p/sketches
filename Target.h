
#ifndef Target_h
#define Target_h
unsigned int millis();

#define MAX_NUMBER_OF_BUTTONS 2
#define MAX_NUMBER_OF_DEBUG_BUTTONS 10

#define HR (320 * 3)
#define VR (240 * 3)

#include <stdint.h>
#include <stdlib.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define EPROM_SIZE 4096

long random(int c);


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

int  listenToTouchesOnScreen();
int  getTouchX();
int  getTouchY();
int  touched();
void setTouchX(int x);
void setTouchY(int x);
void setMouseUp(int up);

// CONSOLE

void consoleln(unsigned long c);
void consoleln(const char* c);
void consoleln(float f);
void consoleln(int i);
void consoleln();
void console(const char* c);
void console(float f);
void console(int i);

// STATIC/DYNAMIC READING

void        useStaticStrings();
void        useDynamicStrings();
char        pgm_read_byte(const char* a);
const int   pgm_read_word_near(const int* a);
char        pgm_read_byte_near(const unsigned char* a);
const char* PSTR(const char* a);

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

char pgm_read_byte(const char* a);
const int pgm_read_word_near(const int* a);
char pgm_read_byte_near(const unsigned char* a);
const char* PSTR(const char* a);

#endif /* Target_h */
