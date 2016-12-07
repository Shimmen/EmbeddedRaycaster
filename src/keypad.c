#include "keypad.h"

Keypad keypad_create(GPIO *gpio, GPIO_SECTION section, bool values_as_chars)
{
	Keypad keypad;
	keypad.gpio = gpio;
	keypad.section = section;
	keypad.values_as_chars = values_as_chars;

	// 4 MSb output, 4 LSb input
	keypad.gpio->mode |= (0x00005500 << section);

	// Set output pins as push-pull (pin value 0)
	uint16_t o_type = keypad.gpio->o_type & ((section == GPIO_SECTION_LOW) ? 0xFF0F : 0x0FFF);
	keypad.gpio->o_type = o_type;

	// Set input pins as pull-down
	keypad.gpio->pupd |= (0x000000AA << section);

	return keypad;
}

const uint8_t keyboard_char_lookup[4 * 4] = {
	'1', '2', '3', 'A',
	'4', '5', '6', 'B',
	'7', '8', '9', 'C',
	'*', '0', '#', 'D'
};

const uint8_t keyboard_val_lookup[4 * 4] = {
	1,  2, 3,  10,
	4,  5, 6,  11,
	7,  8, 9,  12,
	14, 0, 15, 13
};

void keypad_activate_row(Keypad *keypad, int row)
{
	if (row >= 0 && row < 4) {
		uint8_t row_select = 1 << (row + 4);
		uint8_t current_val = O_DATA_SECTION(keypad);
		uint8_t value = row_select | (current_val & 0x0F);

		if (keypad->section == GPIO_SECTION_LOW) {
			keypad->gpio->o_data_low = value;
		} else {
			keypad->gpio->o_data_high = value;
		}
	}
}

int keypad_read_column(const Keypad *keypad)
{
	uint8_t column = keypad->gpio->i_data >> (keypad->section / 2);
	for (int i = 0; i < 4; i++) {
		if ((column & 0x01) > 0) {
			return i;
		}
		column >>= 1;
	}
	return -1;
}

uint8_t keypad_get(Keypad *keypad)
{
	for (int row = 0; row < 4; row++) {

		keypad_activate_row(keypad, row);
		int column = keypad_read_column(keypad);

		if (column == -1) {
			continue;
		}

		uint32_t index = row * 4 + column;
		if (index < 16) {
			if (keypad->values_as_chars) {
				return keyboard_char_lookup[index];
			} else {
				return keyboard_val_lookup[index];
			}
		}
	}

	return 0xFF;
}
