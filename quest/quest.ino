#include <SoftwareSerial.h>
#include <SPI.h>
#include <rgb_matrix.h>

// Settings
#define TIMEOUT 5000
#define BUFFER_SIZE 64
#define TICK_DELAY 25
#define LED_PIN 13

// Bytes
#define STX 0x02
#define ETX 0x03
#define CR 0x0d
#define LF 0x0a

// RFID
#define RFID_TAG_SIZE 10
#define RFID_TAG_CSUM_SIZE 2
#define RFID_RX_PIN 2
#define RFID_TX_PIN 3

// FSM States
#define ST_WAITING 0
#define ST_TAG_READ 1
#define ST_TAG_READ_END 2
#define ST_TAG_SEND 3
#define ST_ACTION_START 4
#define ST_ACTION_END 5

// Hardware SPI
#define SPI_SDI_PIN 11
#define SPI_SCLK_PIN 12

// RGB Matrix
#define RGB_N_X 1
#define RGB_N_Y 1
#define RGB_LATCH_PIN 8

unsigned long time = 0;
unsigned int tick = 0;
unsigned char frameCounter = 0;

rgb_matrix RGBMatrix(RGB_N_X, RGB_N_Y, SPI_SDI_PIN, SPI_SCLK_PIN, RGB_LATCH_PIN);

// Animations
#define AN_WAIT 0
#define AN_ACCESS_GRANTED 1
#define AN_ACCESS_DENIED 2
#define AN_READ_START 3
#define AN_READ_END 4

#define AN_QUEUE_SIZE 10
#define AN_ST_START 0
#define AN_ST_END -1

void animationWait();
void animationAccessGranted();
void animationAccessDenied();
void animationReadStart();
void animationReadEnd();

typedef void (*FunctionCallback)();
FunctionCallback animations[] = {&animationWait, &animationAccessGranted, &animationAccessDenied, &animationReadStart, &animationReadEnd};

int currentAnimation = AN_WAIT;
int animationState = AN_ST_END;
bool animationChanged = false;

// Буфер для приёма данных
int dataLength = 0;
unsigned char buffer[BUFFER_SIZE];

double sendingTime = 0;
int currentState = ST_WAITING;

SoftwareSerial RFID(RFID_RX_PIN, RFID_TX_PIN);

void setup() {
  Serial.begin(9600);
  RFID.begin(9600);
  pinMode(LED_PIN, OUTPUT);
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
  if (currentState == ST_WAITING) {
    animation(AN_WAIT);
    readTag();
    if (dataLength) {
      animation(AN_READ_START);
      currentState = ST_TAG_READ;
    }
  }
  else if (currentState == ST_TAG_READ) {
    send("CHECK", buffer, dataLength);
    clearBuffer();
    currentState = ST_TAG_SEND;
    sendingTime = millis();
  }
  else if (currentState == ST_TAG_SEND && !isRequestTimeout()) {
    read();
    if (dataLength) {
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
    if (buffer[0] == 0x31) {
      animation(AN_ACCESS_GRANTED);
    }
    else {
      animation(AN_ACCESS_DENIED);
    }
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

/**
 * Проверяет оконечно ли время ожидания
 *
 * @return {Boolean} [description]
 */
bool isRequestTimeout() {
  return (millis() - sendingTime) >= TIMEOUT;
}

/**
 * Очещает буфер данных
 *
 * @return void
 */
void clearBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = NULL;
  }
  dataLength = 0;
}