#ifndef TIMER_H_
#define TIMER_H_

#include <stdbool.h>
#include <stdint.h>

//
// Definitions
//

#define CPU_CLOCK_SPEED (168000000)

#define EMULATOR_MODE 1
#define CYCLES_PER_250_NANOSECONDS (1)
//#define CYCLES_PER_250_NANOSECONDS (41)

//#if EMULATOR_MODE
// #define CYCLES_PER_250_NANOSECONDS (1)
//#else
// #define CYCLES_PER_250_NANOSECONDS (((CPU_CLOCK_SPEED / 1000000) / 4) - 1)
//#endif

#define SYS_TICK_BASE_ADR (0xE000E010)

typedef volatile struct sys_tick {
	uint32_t ctrl;
	uint32_t load; // (lower 24 bit)
	uint32_t val;  // (lower 24 bit)
	uint32_t calib;
} SysTick;

//
// Global data
//

extern SysTick *g_sys_tick;

//
// Procedures
//

static inline void timer_init()
{
	g_sys_tick = (SysTick *)SYS_TICK_BASE_ADR;
}

static inline int is_timer_finished()
{
	return (g_sys_tick->ctrl & 0x00010000);
}

static inline void delay_cycles(uint32_t cycles)
{
	if (cycles <= 0) { return; } // ignore 'empty' delays

	g_sys_tick->ctrl = 0;          // reset/stop timer
	g_sys_tick->load = cycles;     // load timer count
	g_sys_tick->val = 0;           // reset current
	g_sys_tick->ctrl = 0x00000005; // start timer (busy wait mode)
	while (!is_timer_finished());  // wait...
	g_sys_tick->ctrl = 0;          // reset
}

//
// Delay program execution by 250 nanoseconds
//
static inline void delay_250ns()
{
	delay_cycles(CYCLES_PER_250_NANOSECONDS);
}

//
// Delay program execution by 250 nanoseconds
//
static inline void delay_500ns()
{
	//delay_cycles(2 * CYCLES_PER_250_NANOSECONDS);
	delay_cycles(CYCLES_PER_250_NANOSECONDS);
	delay_cycles(CYCLES_PER_250_NANOSECONDS);
}

//
// Delay program execution by 1 microsecond
//
static inline void delay_1us()
{
	//delay_cycles(4 * CYCLES_PER_250_NANOSECONDS);
	delay_cycles(CYCLES_PER_250_NANOSECONDS);
	delay_cycles(CYCLES_PER_250_NANOSECONDS);
	delay_cycles(CYCLES_PER_250_NANOSECONDS);
	delay_cycles(CYCLES_PER_250_NANOSECONDS);
}

//
// Delay program execution by us microseconds
//
static inline void delay_micro(uint32_t us)
{
	for (int i = 0; i < us; i++) {
		delay_1us();
	}
}

//
// Delay program execution by ms milliseconds
//
static inline void delay_milli(uint32_t ms)
{
#if EMULATOR_MODE
	uint32_t us = ms;
#else
	uint32_t us = 1000 * ms;
#endif

	delay_micro(us);
}

#endif // TIMER_H_