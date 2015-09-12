#ifndef RFID_H

#define RFID_H

#include <arduino.h>
#include <SoftwareSerial.h>
#include "quest.h"
#include "constants.h"

extern SoftwareSerial RFID;
extern unsigned char tag[RFID_TAG_SIZE];

bool readTag();
int searchTag(unsigned char *tag, String *tags);
void resetTag();

#endif