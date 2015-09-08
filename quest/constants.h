// Settings
#define TIMEOUT 5000
#define TICK_DELAY 100
#define BUFFER_SIZE 128

// Bytes
#define STX 0x02
#define ETX 0x03
#define LF 0x0a
#define CR 0x0d

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

// Animations
#define AN_WAIT 0
#define AN_ACCESS_GRANTED 1
#define AN_ACCESS_DENIED 2
#define AN_READ_START 3
#define AN_READ_END 4

#define AN_QUEUE_SIZE 10
#define AN_ST_START 0
#define AN_ST_END -1