unsigned long frameStart() {
  frameCounter++;
  return micros();
}

void frameEnd(unsigned long timeStart) {
  unsigned long timeEnd = micros();
  if (timeStart < timeEnd) {
    if (timeEnd - timeStart < 500) {
      delayMicroseconds(500 - (timeEnd - timeStart));
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

void animationWait() {
  if (tick %5 == 0) {
    RGBMatrix.clear();
  }
}

void animationReadStart() {
  if (isAnimationStart()) {
    RGBMatrix.clear();
    rectangle(0, 0, 7, 7, BLUE, TOP_LAYER);
    animationState++;
  }
}

void animationReadEnd() {
  animationEnd();
}

void animationAccessGranted() {
  if (isAnimationStart()) {
    RGBMatrix.clear();
    rectangle(0, 0, 7, 7, GREEN, TOP_LAYER);
    animationState++;
  }
  if (tick % 20 == 0) {
    animationEnd();
  }
}

void animationAccessDenied() {
  if (isAnimationStart()) {
    RGBMatrix.clear();
    rectangle(0, 0, 7, 7, RED, TOP_LAYER);
    animationState++;
  }
  if (tick % 20 == 0) {
    animationEnd();
  }
}