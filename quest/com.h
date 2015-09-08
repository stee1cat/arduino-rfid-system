#ifndef COM_H

#define COM_H

#include <arduino.h>
#include <constants.h>

extern int dataLength;
extern unsigned char buffer[BUFFER_SIZE];

void send(const char *command, const unsigned char *data, int length);
bool read();
void clearBuffer();

#endif