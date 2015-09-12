#ifndef RGB_H

#define RGB_H

#include <SPI.h>
#include <rgb_matrix.h>
#include <arduino.h>
#include "constants.h"

typedef void (*FunctionCallback)();

extern rgb_matrix RGBMatrix;
extern unsigned int numberOfUser;
extern unsigned int numberOfAuth;

unsigned long frameStart();
void frameEnd(unsigned long timeStart);
void animation(int animation);
void animationEnd();
bool isAnimationStart();
bool isAnimationEnd();
void display();
void rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color, uint8_t index);

void animationWait();
void animationAccessGranted();
void animationAccessDenied();
void animationReadStart();
void animationReadEnd();

#endif