#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Arduino.h>
#include "composite_hid.h"
#include "config.h"


// --------------------------------------------------------
// Function prototypes
// --------------------------------------------------------

void setupGamepad();
void loopGamepad();
void printGamepad(const char* msg);
void printGamepad(const String& msg);
void printGamepad(const char* format, ...);

#endif // GAMEPAD_H
