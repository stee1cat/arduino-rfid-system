#include "com.h"
#include "rgb.h"
#include "rfid.h"
#include "quest.h"
#include "constants.h"

// RGB
rgb_matrix RGBMatrix(RGB_N_X, RGB_N_Y, SPI_SDI_PIN, SPI_SCLK_PIN, RGB_LATCH_PIN);
unsigned long time = 0;
unsigned int tick = 0;
unsigned char frameCounter = 0;
int currentAnimation = AN_WAIT;
bool animationChanged = false;
int animationState = AN_ST_END;

// Буфер для приёма данных
int dataLength = 0;
unsigned char buffer[BUFFER_SIZE];

// RFID
SoftwareSerial RFID(RFID_RX_PIN, RFID_TX_PIN);
unsigned char tag[RFID_TAG_SIZE];

double sendingTime = 0;
int currentState = ST_WAITING;
int numberOfUser = 1;

void setup() {
  Serial.begin(9600);
  RFID.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  RGBMatrix.set_cur(0, 0);
  RGBMatrix.display(renderLoop);
}

/**
 * @return void
 */
void renderLoop() {
  unsigned long timeStart = frameStart();
  bool tagValid = false;
  if (currentState == ST_WAITING) {
    animation(AN_WAIT);
    tagValid = readTag();
    if (tagValid) {
      animation(AN_READ_START);
      currentState = ST_TAG_READ;
    }
    else {
      if (read()) {
        runCommand();
      }
    }
  }
  else if (currentState == ST_TAG_READ) {
    send("CHECK", tag, RFID_TAG_SIZE);
    clearBuffer();
    clearTag();
    currentState = ST_TAG_SEND;
    sendingTime = millis();
  }
  else if (currentState == ST_TAG_SEND && !isRequestTimeout()) {
    if (read()) {
      animation(AN_READ_END);
      currentState = ST_ACTION_START;
    }
  }
  // Таймаут
  else if (currentState == ST_TAG_SEND && isRequestTimeout()) {
    animation(AN_READ_END);
    currentState = ST_WAITING;
    sendingTime = 0;
  }
  else if (currentState == ST_ACTION_START) {
    runCommand();
    if (isAnimationEnd()) {
      currentState = ST_ACTION_END;
    }
  }
  else if (currentState == ST_ACTION_END) {
    clearBuffer();
    currentState = ST_WAITING;
  }
  else {
    currentState = ST_WAITING;
  }
  display();
  frameEnd(timeStart);
}

void runCommand() {
  String message = getMessage();
  String command = parseCommand(message);
  String param = parseParam(message);
  if (command.equals("players") && param.length()) {
    numberOfUser = param.toInt();
  }
  if (command.equals("access") && param.length()) {
    commandAccess(param.toInt());
  }
  clearBuffer();
}

void commandAccess(int result) {
  if (result) {
    animation(AN_ACCESS_GRANTED);
  }
  else {
    animation(AN_ACCESS_DENIED);
  }
}

String getMessage() {
  String result = "";
  for (int i = 0; i < dataLength; i++) {
    result.concat((char) buffer[i]);
  }
  return result;
}

String parseCommand(String message) {
  String command = message.substring(0, message.indexOf(' '));
  command.toLowerCase();
  return command;
}

String parseParam(String message) {
  String param = message.substring(message.indexOf(' '));
  param.trim();
  return param;
}

/**
 * Проверяет оконечно ли время ожидания
 *
 * @return {Boolean} [description]
 */
bool isRequestTimeout() {
  return (millis() - sendingTime) >= TIMEOUT;
}