#include "graphics_display.h"

#include <stdbool.h>

#include "timer.h"

enum
{
	B_RS     = 0x01, /* Data (1) or instruction (0) */
	B_RW     = 0x02, /* Read (1) or write (0) */

	B_SELECT = 0x04, /* Should always be 0 for this display! */

	B_CS1    = 0x08, /* Select controller 1 (left screen half) */
	B_CS2    = 0x10, /* Select controller 2 (right screen half) */

	B_RESET  = 0x20, /* Write 1 to this pin to reset display */
	B_E      = 0x40, /* Clock pulse E */

	B_BUSY   = 0x80  /* Mask for busy bit. Busy=1, Ready=0 */
};

enum
{
	CMD_DISPLAY_OFF   = 0x3E,
	CMD_DISPLAY_ON    = 0x3F,

	CMD_DISPLAY_START = 0xC0,

	CMD_SET_ADDR      = 0x40,
	CMD_SET_PAGE      = 0xB8
};

//
// Specific type for selecting controller
//
typedef enum
{
	CS1     = B_CS1,
	CS2     = B_CS2,
	CS_BOTH = B_CS1|B_CS2,
	CS_NONE = 0
} ControllerSelect;

void graphics_display_bit_set(GraphicsDisplay *display, uint8_t bits)
{
	display->gpio->o_data_low &= (~B_SELECT);
	display->gpio->o_data_low |= (bits);
	
}

void graphics_display_bit_clr(GraphicsDisplay *display, uint8_t bits)
{
	display->gpio->o_data_low &= (~B_SELECT);
	display->gpio->o_data_low &= (~bits);
}

void graphics_display_select_controller(GraphicsDisplay *display, ControllerSelect cs)
{
	if (cs == CS_NONE) {
		graphics_display_bit_clr(display, CS1|CS2);
		return;
	}
	if (cs == CS1) {
		graphics_display_bit_set(display, CS1);
		graphics_display_bit_clr(display, CS2);
		return;
	}
	if (cs == CS2) {
		graphics_display_bit_set(display, CS2);
		graphics_display_bit_clr(display, CS1);
		return;
	}
	if (cs == CS_BOTH) {
		graphics_display_bit_set(display, CS_BOTH);
		return;
	}
}

void graphics_display_wait_until_ready(GraphicsDisplay *display)
{
	graphics_display_bit_clr(display, B_E);

	// Set data bits (bit 8-15) to input pins, so that we can read the status
	display->gpio->mode = 0x00005555;

	graphics_display_bit_clr(display, B_RS);
	graphics_display_bit_set(display, B_RW);
	delay_500ns();

	while (1) {
		graphics_display_bit_set(display, B_E);
		delay_500ns();

		uint8_t status = (display->gpio->i_data_high & B_BUSY);
		if (status == 0) { break; }
		
		graphics_display_bit_clr(display, B_E);
		delay_500ns();
	}
	
	//delay_milli(1);
	
	graphics_display_bit_set(display, B_E);
	
	// Revert all pins to output pins
	display->gpio->mode = 0x55555555;
}

//
// Write to the display, CS1/CS2 or both.
// Assumes that RS and RW are set before calling this!
// At least one of the chips has to be set before calling this!
//
void graphics_display_write(GraphicsDisplay *display, ControllerSelect cs, uint8_t value)
{
	// Write value to all selected controllers
	display->gpio->o_data_high = value;
	graphics_display_select_controller(display, cs);
	delay_500ns();
	graphics_display_bit_set(display, B_E);
	delay_500ns();
	graphics_display_bit_clr(display, B_E);

	// Make sure controller 1 is ready (if it was written to)
	if (cs & CS1) {
		graphics_display_select_controller(display, CS1);
		graphics_display_wait_until_ready(display);
	}

	// Make sure controller 2 is ready (if it was written to)
	if (cs & CS2) {
		graphics_display_select_controller(display, CS2);
		graphics_display_wait_until_ready(display);
	}

	// Reset data value (but don't write anything, obviously...)
	display->gpio->o_data_high = 0;
	graphics_display_bit_set(display, B_E);
	graphics_display_select_controller(display, CS_NONE);
}

void graphics_display_write_command(GraphicsDisplay *display, ControllerSelect cs, uint8_t command)
{
	graphics_display_bit_clr(display, B_E);
	graphics_display_bit_clr(display, B_RS|B_RW);
	graphics_display_write(display, cs, command);
}

void graphics_display_write_data(GraphicsDisplay *display, ControllerSelect cs, uint8_t data)
{
	graphics_display_bit_clr(display, B_E);
	graphics_display_bit_set(display, B_RS);
	graphics_display_bit_clr(display, B_RW);
	graphics_display_write(display, cs, data);
}

void graphics_display_clear(GraphicsDisplay *display)
{
	const uint8_t clear_value = 0x00;
	
	for (int page = 0; page < 8; page++) {
		graphics_display_write_command(display, CS_BOTH, CMD_SET_PAGE|page);
		graphics_display_write_command(display, CS_BOTH, CMD_SET_ADDR|0); // Set to first column, will auto increment
		for (int column = 0; column < 64; column++) {
			graphics_display_write_data(display, CS_BOTH, clear_value);
		}
	}
}

GraphicsDisplay graphics_display_create(GPIO *gpio)
{
	GraphicsDisplay display;
	display.gpio = gpio;

	display.gpio->mode = 0x55555555;    // Default all pins to output pins
	display.gpio->o_type = 0x0000;      // Set all output pins to push/pull
	display.gpio->pupd = 0x55550000;    // Set input pins to pull-up
	display.gpio->o_speed = 0x55555555; // Medium speed for output pins // TODO: Can we make it faster?

	// Initialize display
	graphics_display_bit_set(&display, B_E);
	delay_micro(10);
	graphics_display_bit_clr(&display, B_CS1|B_CS2|B_RESET|B_E);
	delay_milli(30);
	graphics_display_bit_set(&display, B_RESET);
	graphics_display_write_command(&display, CS_BOTH, CMD_DISPLAY_OFF);
	graphics_display_write_command(&display, CS_BOTH, CMD_DISPLAY_ON);
	graphics_display_write_command(&display, CS_BOTH, CMD_DISPLAY_START);
	graphics_display_write_command(&display, CS_BOTH, CMD_SET_ADDR);
	graphics_display_write_command(&display, CS_BOTH, CMD_SET_PAGE);
	graphics_display_select_controller(&display, 0);

	return display;
}

void graphics_display_push_framebuffer(GraphicsDisplay *display, Framebuffer *framebuffer)
{
	// For each page/8 rows:
	for (int page = 0; page < 8; page++) {
		
		// Set page for both controllers
		graphics_display_write_command(display, CS_BOTH, CMD_SET_PAGE|page);
		
		// For each column in the display:
		for (int screen_column = 0; screen_column < 128; screen_column++) {
			
			// Calculate the current controller and controller-local column
			ControllerSelect cs = (screen_column >= 64) ? CS2 : CS1;
			
			// Set address/column to 0 before beginning a write to a new column.
			// Must be done for both controllers! Will auto increment with writes!
			if (screen_column == 0 || screen_column == 64) {
				graphics_display_write_command(display, cs, CMD_SET_ADDR|0);
			}
			
			// Find value for byte coding for 8 pixels
			const int y = page * 8;
			const int x = screen_column;
			uint8_t pixels =
				framebuffer_get_pixel(framebuffer, x, y + 0) << 0 |
				framebuffer_get_pixel(framebuffer, x, y + 1) << 1 |
				framebuffer_get_pixel(framebuffer, x, y + 2) << 2 |
				framebuffer_get_pixel(framebuffer, x, y + 3) << 3 |
				framebuffer_get_pixel(framebuffer, x, y + 4) << 4 |
				framebuffer_get_pixel(framebuffer, x, y + 5) << 5 |
				framebuffer_get_pixel(framebuffer, x, y + 6) << 6 |
				framebuffer_get_pixel(framebuffer, x, y + 7) << 7;
				
			// Obscure bug fix! If I write a 0xFF data value to the display, the next time the
			// mode is changed to 0x55555555 (after reading/checking status) the left half of
			// the screen will turn off/crash... It does set the data correctly, because if you
			// restart the screen you can see the image correctly before you clear the screen.
			// Alternatively, just re-enable the screen after doing the drawing. That works
			// every time, in comparrison to the fix below which only works sometimes... And also,
			// it doesn't fuck up the actual graphics.
			//if (pixels == 0xFF) {
			//	pixels -= 1;
			//}

			// Write to display. Will auto increment column/address with write
			graphics_display_write_data(display, cs, pixels);
		}
	}
	
	graphics_display_write_command(display, CS_BOTH, CMD_DISPLAY_ON);
}
