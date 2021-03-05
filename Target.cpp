//
//  Target.cpp
//  AquaOS
//
//  Created by Kurt Schwarze on 16.03.19.
//  Copyright © 2019 Kurt Schwarze. All rights reserved.
//
#include "Target.h"
#include <stdio.h>

#include "AquaOS.h"
#include "EEPROM.h"

// TFT DRAWING PRIMITIVES

TFT tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void tftReset() {
    tft.reset();
    tft.begin(0x9341);
}

void tftRect(int x, int y, int w, int h, int c) {
    tft.fillRect(x, y, w, h, c);
}

void tftPixel(int x, int y, int c) {
    tft.drawPixel(x, y, c);
}

void tftVline(int x, int y, int w, int c) {
    tft.drawFastVLine(x, y, w, c);
}

void tfHline(int x, int y, int l, int c) {
    tft.drawFastHLine(x, y, l, c);
}

// TOUCHING

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
extern TFT tft;

int mouseEventX = 0;
int mouseEventY = 0;
unsigned char mouseState = 0;
unsigned char wasTouched = 0;
unsigned long lastTouch = 0;

/**
 Returns true if a touch happened.
 */
int listenToTouchesOnScreen() {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    int isTouching = p.z >= 10 && p.z <= 1000;
    
    if (isTouching) {
        lastTouch = millis();
        if (mouseState == 0) {
            mouseEventY = tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
            mouseEventX = tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
            wasTouched = 1;
            mouseState=1;
            return 1;
        }
    } else if (mouseState==1) {
        if (millis()-lastTouch>300)
            mouseState=0;
    }
    return 0;
}

/**
  Returns true if a touch happened since last call
 */
int touched() {
    listenToTouchesOnScreen();
    int result = wasTouched;
    wasTouched = 0;
    return result;
    /*
    int result = mouseUp;
    if (mouseUp) {
        mouseUp = 0;
        mouseDown = 0;
    }
    return result;
     */
}

int getTouchX() {
    return mouseEventX;
}

int getTouchY() {
    return mouseEventY;
}

// CONSOLE

int serialized = 0;

void ensureSerial()  {
//if (!serialized)  {
    //    Serial.begin(9600);
    //    serialized=1;
    //}
}

// CONSOLE

void console(const char* c) {
        ensureSerial();
    //    Serial.print(c);
}

void console(float f) {
    ensureSerial();
    //Serial.print(f);
}

void console(int i) {
    ensureSerial();
    //Serial.print(i);
}

void consoleln(const char* c) {
    ensureSerial();
    //Serial.println(c);
}

void consoleln(unsigned long c) {
    ensureSerial();
    //Serial.println(c);
}

void consoleln(float f) {
    ensureSerial();
    //Serial.println(f);
}

void consoleln(int i) {
    ensureSerial();
    //Serial.println(i);

}

void consoleln() {
    ensureSerial();
}

// FINISHING

void terminate() {
    for (;;)
        ;
}

// RAM

void writeToHeap(int a, unsigned char c) {
    // nothing done
}

// EPROM

void writeByteToEEPROM(int address, unsigned char value) {
    if (readByteFromEEPROM(address) == value) {
        return;
    }
    if (address < 0 || address >= EPROM_SIZE)
        fatalError(17, address);
    EEPROM.write(address, value);
}

void writeIntToEEPROM(int address, unsigned int value) {
    writeByteToEEPROM(address, value / 256);
    writeByteToEEPROM(address + 1, value % 256);
}

unsigned char readByteFromEEPROM(int address) {

    if (address < 0 || address >= EPROM_SIZE)
        fatalError(4944, address);
    return EEPROM.read(address);
}

unsigned int readIntFromEEPROM(int address) {
    return readByteFromEEPROM(address) * 256 + readByteFromEEPROM(address + 1);
}

int random(int r) {
    return rand() % r;
}
