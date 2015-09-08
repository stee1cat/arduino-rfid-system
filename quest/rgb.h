#ifndef RGB_H

#define RGB_H

#include <SPI.h>
#include <rgb_matrix.h>
#include "constants.h"

typedef void (*FunctionCallback)();

extern rgb_matrix RGBMatrix;
extern unsigned long time;
extern unsigned int tick;
extern unsigned char frameCounter;
extern int currentAnimation;
extern bool animationChanged;
extern int animationState;

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