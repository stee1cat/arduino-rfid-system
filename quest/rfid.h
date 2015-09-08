#ifndef RFID_H

#define RFID_H

#include <arduino.h>
#include <SoftwareSerial.h>
#include "constants.h"

extern SoftwareSerial RFID;
extern unsigned char tag[RFID_TAG_SIZE];

bool readTag();
void clearTag();

#endif