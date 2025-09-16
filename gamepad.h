#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Arduino.h>
#include "config.h"
#include "gamepad_pinout.h"
#include "gamepad_assignment.h"

// --------------------------------------------------------
// Function prototypes
// --------------------------------------------------------

void setupGamepad();
void loopGamepad();
void printGamepad(const char* msg);
void printGamepad(const String& msg);
void printGamepadF(const char* format, ...);

#endif // GAMEPAD_H
