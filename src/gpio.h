#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

//
// Base port address definitions
//
typedef enum
{
	GPIO_D = 0x40020C00,
	GPIO_E = 0x40021000
} GPIO_PORTS;

//
// GPIO low or high bits/bytes?
//
typedef enum
{
	GPIO_SECTION_LOW = 0,
	GPIO_SECTION_HIGH = 16
} GPIO_SECTION;

//
// Struct defining a standard GPIO port
//
typedef volatile struct gpio {

	// Pin modes (2 bits code for one pin):
	//  00 - input
	//  01 - output
	//  10 - alternative mode
	//  11 - analog
	uint32_t mode;

	// Pin output types:
	//   0 - push-pull
	//   1 - open drain
	uint16_t o_type;

	uint16_t reserved_01;

	// Output speed (2 bits code for one pin):
	//  00 - low speed
	//  01 - medium speed
	//  10 - fast speed
	//  11 - high speed
	uint32_t o_speed;

	// Pin pull-up/pull-down (2 bits code for one pin):
	//  00 - floating
	//  01 - pull-up
	//  10 - pull-down
	//  11 - reserved
	uint32_t pupd;

	//
	// Input data
	//
	union
	{
		uint16_t i_data;
		struct
		{
			uint8_t i_data_low;
			uint8_t i_data_high;
		};
	};

	uint16_t reserved_02;

	//
	// Output data
	//
	union
	{
		uint16_t o_data;
		struct
		{
			uint8_t o_data_low;
			uint8_t o_data_high;
		};
	};

	uint16_t reserved_03;

	// Bit set reset
	uint32_t bssr;

	// Port configuration lock register
	uint32_t lckr;

	// Alternate function low register
	uint32_t afrl;

	// Alternate function high register
	uint32_t afrh;

} GPIO;

//
// Create a GPIO object from a given base address
//
static inline GPIO *gpio_create(void *address)
{
	return (GPIO *)address;
}

//
// Help macros for accessing i_data_low/high and o_data_low/high.
// object_p must be an object containing a pointer to a GPIO and section value!
//
#define I_DATA_SECTION(object_p) ((object_p->section == GPIO_SECTION_LOW) ? object_p->gpio->i_data_low : object_p->gpio->i_data_high)
#define O_DATA_SECTION(object_p) ((object_p->section == GPIO_SECTION_LOW) ? object_p->gpio->o_data_low : object_p->gpio->o_data_high)

#endif // GPIO_H_
