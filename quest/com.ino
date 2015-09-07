/**
 * Отправляет команду в COM-порт
 *
 * @param const char *command Имя команды
 * @param const unsigned char *data Дополнительные данные
 * @param int length Длина дополнительных данных
 * @return void
 */
void send(const char *command, const unsigned char *data, int length) {
  Serial.print(command);
  if (length > 0) {
    Serial.print(' ');
    for (int i = 0; i < length; i++) {
      Serial.write(data[i]);
    }
  }
  Serial.write(CR);
}

/**
 * Читает данные из COM-порта в буфер
 *
 * @return void
 */
void read() {
  byte value;
  if (Serial.available() > 0) {
    while (Serial.available()) {
      value = Serial.read();
      if (BUFFER_SIZE == dataLength || CR == value) {
        break;
      }
      else {
        buffer[dataLength] = value;
      }
      dataLength++;
    }
    Serial.flush();
  }
}