//
//  Target.cpp
//  AquaOS
//
//  Created by Kurt Schwarze on 16.03.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//
#include "Target.h"
#include <SDL/SDL.h>
#include <stdio.h>

#include "AquaOS.h"

extern SDL_Surface* surface;

// TFT SCREEN


long random(int c) {
    return rand()%c;
}

void tftReset() {
}

void _setPixel(int x, int y, int color);
void tftRect(int x, int y, int w, int h, int c) {
    int rh = w;
    int rw = h;
    int rx = HR - y - h;
    int ry = x;
    // extremely inefficient
    for (int i = rx; i < rx + rw; i++)
        for (int j = ry; j < ry + rh; j++)
            _setPixel(i, j, c);
}

void _setPixel(int x, int y, int color) {
    int r = (color >> 11) * 255 / 31;
    int g = ((color >> 5) % 64) * 255 / 63;
    int b = (color % 32) * 255 / 31;

    *((Uint32*)surface->pixels + y * 320 * 3 + x) = SDL_MapRGBA(surface->format, r, g, b, 255);
}

void tftPixel(int x, int y, int c) {
    _setPixel(HR - y - 1, x, c);
}

void tftVline(int x, int y, int w, int c) {
    for (int i = 0; i < w; i++)
        _setPixel(HR - y - i - 1, x, c);
}

void tfHline(int x, int y, int l, int c) {
    for (int i = 0; i < l; i++)
        _setPixel(HR - y - 1, x + i, c);
}

// TOUCH SCREEN

int listenToTouchesOnScreen() {
    return 0;
}

int           px = 0;
int           py = 0;
unsigned char mouseDown = 0;
unsigned char mouseUp = 0;

int touched() {
    listenToTouchesOnScreen();
    int result = mouseUp;
    if (mouseUp) {
        mouseUp = 0;
        mouseDown = 0;
    }
    return result;
}

int getTouchX() {
    return px;
}

int getTouchY() {
    return py;
}

void setTouchX(int x) {
    px = x;
}

void setTouchY(int y) {
    py = y;
}

void setMouseUp(int up) {
    mouseUp = up;
}

// CONSOLE

void console(const char* c) {
    printf("%s", c);
}

void console(float f) {
    printf("%f", f);
}

void console(int i) {
    printf("%d", i);
}

void consoleln(const char* c) {
    printf("%s\n", c);
}

void consoleln(unsigned long c) {
    printf("%lu\n", c);
}

void consoleln(float f) {
    printf("%f\n", f);
}

void consoleln(int i) {
    printf("%d\n", i);
}

void consoleln() {
    printf("\n");
}

// STATIC READING

void useStaticStrings() {
}

void useDynamicStrings() {
}

// FINISHING

void terminate() {
    exit(1);
}

// RAM

int heap[10000];
int _eprom[10000];

void writeToHeap(int a, unsigned char c) {
    heap[a] = c;
}

// EPROM

FILE* outfile = NULL;

void writeByteToEEPROM(int a, unsigned char c) {
    if (a < 0 || a >= EPROM_SIZE)
        fatalError(8, a);
    if (outfile == NULL) {
        unsigned char c[4096];
        for (int i = 0; i < 4096; i++)
            c[i] = 0;
        outfile = fopen("/file.txt", "wb");
        fwrite(&c, 1, 4096, outfile);
    }
    fseek(outfile, a, SEEK_SET);
    fwrite(&c, 1, 1, outfile);
    fflush(outfile);
    _eprom[a] = c;
}

void writeIntToEEPROM(int address, unsigned int value) {
    writeByteToEEPROM(address, value / 256);
    writeByteToEEPROM(address + 1, value % 256);
}

unsigned char readByteFromEEPROM(int address) {
    if (address < 0 || address >= EPROM_SIZE)
        fatalError(18, address);
    return (unsigned char)_eprom[address];
}

unsigned int readIntFromEEPROM(int address) {
    return readByteFromEEPROM(address) * 256 + readByteFromEEPROM(address + 1);
}

char pgm_read_byte(const char* a) {
    return *a;
}

const int pgm_read_word_near(const int* a) {
    return *a;
}

char pgm_read_byte_near(const unsigned char* a) {
    return *a;
}

const char* PSTR(const char* a) {
    return a;
}
