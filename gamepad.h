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

#endif // GAMEPAD_H
