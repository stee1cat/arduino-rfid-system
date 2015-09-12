#include "rfid.h"

/**
 * Читает RFID-тег и помещает его в буфер
 *
 * @return bool
 */
bool readTag() {
  byte code[6];         // Код + контрольная сумма
  byte checksum = 0;    // Контрольная сумма
  byte tempByte = 0;    // Временная переменная для подсчёта контрольной суммы
  byte bytesRead = 0;   // Количество полученных байтов из RFID
  bool result = false;  // Флаг коррекности RFID-тега
  byte value = 0;       // Байт полученный из RFID
  if (RFID.available()) {
    value = RFID.read();
    // Начало передачи
    if (STX == value) {
      while (bytesRead < (RFID_TAG_SIZE + RFID_TAG_CSUM_SIZE)) {
        if (RFID.available() > 0) {
          value = RFID.read();
          if (STX == value || ETX == value || CR == value || LF == value) {
            // Ошибка, прекращаем чтение
            break;
          }
          if (bytesRead < RFID_TAG_SIZE) {
            tag[bytesRead] = value;
          }
          //
          if ((value >= '0') && (value <= '9')) {
            value = value - '0';
          }
          else if ((value >= 'A') && (value <= 'F')) {
            value = 10 + value - 'A';
          }
          // Каждоей второе HEX-значние
          if (bytesRead & 1 == 1) {
            // Освобождаем для текущего HEX-значения место,
            // сдвигом предыдущего на 4 бита влево
            code[bytesRead >> 1] = (value | (tempByte << 4));
            // Если получили байт контрольной суммы, то вычисляем её (XOR)
            if (bytesRead >> 1 != 5) {
              checksum ^= code[bytesRead >> 1];
            }
          }
          else {
            tempByte = value;
          }
          bytesRead++;
        }
        if (bytesRead == (RFID_TAG_SIZE + RFID_TAG_CSUM_SIZE) && code[5] == checksum) {
          result = true;
        }
      }
    }
  }
  return result;
}

/**
 * Выполняет поиск тега в списке отсканированных
 *
 * @param tag Искомый тег
 * @param tags Список отсканированных тегов
 * @return int
 */
int searchTag(unsigned char *tag, String *tags) {
  int result = -1;
  String search = toString(tag, RFID_TAG_SIZE);
  for (int index = 0; index < MAX_USERS; index++) {
    if (tags[index].equalsIgnoreCase(search)) {
      result = index;
    }
  }
  return result;
}

/**
 * Удаляет прочитанный тег из буфера
 *
 * @return void
 */
void resetTag() {
  for (int i = 0; i < RFID_TAG_SIZE; i++) {
    tag[i] = NULL;
  }
}