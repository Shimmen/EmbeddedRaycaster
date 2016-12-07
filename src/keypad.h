#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"

typedef struct keypad {
	GPIO *gpio;
	GPIO_SECTION section;
	bool values_as_chars;
} Keypad;

//
// Create a keypad from the given GPIO and section. If values_as_chars is set to true, the return value of
// keypad get will be the ASCII code for the pressed character. If false it will simply be a value from 0 to 15
// indicating what button was pressed.
//
Keypad keypad_create(GPIO *gpio, GPIO_SECTION section, bool values_as_chars);

//
// Get the (first) currently pressed key, char or value, depending on if values_as_chars is true or false.
//
uint8_t keypad_get(Keypad *keypad);

#endif // KEYPAD_H_