#include "com.h"
#include "rgb.h"
#include "rfid.h"
#include "quest.h"
#include "constants.h"

// RGB
rgb_matrix RGBMatrix(RGB_N_X, RGB_N_Y, SPI_SDI_PIN, SPI_SCLK_PIN, RGB_LATCH_PIN);

// Буфер для приёма данных
int dataLength = 0;
unsigned char buffer[BUFFER_SIZE];

// RFID
SoftwareSerial RFID(RFID_RX_PIN, RFID_TX_PIN);
unsigned char tag[RFID_TAG_SIZE];
String tags[MAX_USERS];

double sendingTime = 0;
int currentState = ST_WAITING;
unsigned int numberOfUser = 1;
unsigned int numberOfAuth = 0;

bool doorIsOpen = false;

void setup() {
  Serial.begin(9600);
  RFID.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
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
  String message = toString(buffer, dataLength);
  String command = parseCommand(message);
  String param = parseParam(message);
  if (command.equals("users") && param.length()) {
    commandInit(param.toInt());
  }
  if (command.equals("access") && param.length()) {
    commandAccess(param.toInt());
  }
  clearBuffer();
}

/**
 * Выполняет инициализацию
 *
 * @param users Количество пользователей
 */
void commandInit(unsigned int users) {
  String empty = "";
  numberOfUser = (users >= 1 && users <= MAX_USERS)? users: 1;
  numberOfAuth = 0;
  doorIsOpen = false;
  RGBMatrix.clear();
  digitalWrite(RELAY_PIN, LOW);
  currentState = ST_WAITING;
  for (int i = 0; i < MAX_USERS; i++) {
    tags[i] = empty;
  }
}

/**
 * Обрабатывает результат предоставления доступа
 *
 * @param result
 */
void commandAccess(int result) {
  if (result == 1) {
    // Если авторизуется новый пользователь, то добавляем ключ в список
    if (searchTag(tag, tags) == -1) {
      tags[numberOfAuth] = toString(tag, RFID_TAG_SIZE);
      numberOfAuth++;
    }
    if (numberOfAuth == numberOfUser) {
      doorIsOpen = true;
      digitalWrite(RELAY_PIN, HIGH);
    }
    animation(AN_ACCESS_GRANTED);
  }
  else {
    animation(AN_ACCESS_DENIED);
  }
  resetTag();
}

/**
 * Конвертация unsigned char в объект String
 *
 * @param string
 * @param length
 * @return String
 */
String toString(unsigned char *string, int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    result.concat((char) string[i]);
  }
  return result;
}

String parseCommand(String message) {
  String command = message.substring(0, message.indexOf(' '));
  command.toLowerCase();
  command.trim();
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