#include "rgb.h"

FunctionCallback animations[] = {
  &animationWait,
  &animationAccessGranted,
  &animationAccessDenied,
  &animationReadStart,
  &animationReadEnd
};

unsigned long time = 0;
unsigned int tick = 0;
unsigned char frameCounter = 0;
int currentAnimation = AN_WAIT;
bool animationChanged = false;
int animationState = AN_ST_END;

unsigned long frameStart() {
  frameCounter++;
  return micros();
}

void frameEnd(unsigned long timeStart) {
  unsigned long timeEnd = micros();
  if (timeStart < timeEnd) {
    if (timeEnd - timeStart < SYNC_DELAY) {
      delayMicroseconds(SYNC_DELAY - (timeEnd - timeStart));
    }
  }
}

void animation(int animation) {
  if (currentAnimation != animation) {
    currentAnimation = animation;
    animationState = AN_ST_START;
    animationChanged = true;
  }
}

void animationEnd() {
  animationState = AN_ST_END;
}

bool isAnimationStart() {
  return animationState == AN_ST_START;
}

bool isAnimationEnd() {
  return animationState == AN_ST_END;
}

void display() {
  int animationsCount = sizeof(animations) / sizeof(animations[0]);
  if (((frameCounter) % 10 == 0) || animationChanged) {
    if ((millis() - time >= TICK_DELAY) || animationChanged) {
      time = millis();
      tick++;
      if (currentAnimation < animationsCount) {
        animations[currentAnimation]();
      }
      animationChanged = false;
    }
  }
}

void rectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color, uint8_t index) {
  int16_t maxY = max(y0, y1);
  int16_t minY = min(y0, y1);
  for (int i = minY; i <= maxY; i++) {
    RGBMatrix.plot(x0, i, x1, i, 1, color, index);
  }
}

void blink(uint8_t color) {
  if (isAnimationStart()) {
    RGBMatrix.clear();
  }
  animationState++;
  if (animationState % 2 == 0) {
    rectangle(0, 0, 7, 7, color, TOP_LAYER);
  }
  else {
    RGBMatrix.clear();
  }
  if (animationState == 10) {
    RGBMatrix.clear();
    animationEnd();
  }
}

void animationWait() {
  int height = 0;
  if (tick % 5 == 0) {
    if (numberOfAuth) {
      if (numberOfAuth != animationState) {
        RGBMatrix.clear();
        animationState = numberOfAuth;
        for (int i = 0; i < numberOfAuth; i++) {
          if (i == 0) {
            rectangle(0, 0, 2, 2, GREEN, TOP_LAYER);
          }
          else if (i == 1) {
            rectangle(0, 5, 2, 7, GREEN, TOP_LAYER);
          }
          else if (i == 2) {
            rectangle(5, 0, 7, 2, GREEN, TOP_LAYER);
          }
          else if (i == 3) {
            rectangle(5, 5, 7, 7, GREEN, TOP_LAYER);
          }
        }
      }
    }
  }
}

void animationReadStart() {
  if (isAnimationStart()) {
    animationState++;
  }
}

void animationReadEnd() {
  animationEnd();
}

void animationAccessGranted() {
  blink(GREEN);
}

void animationAccessDenied() {
  blink(RED);
}