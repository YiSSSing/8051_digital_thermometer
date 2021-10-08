// define for interrupts
#include <reg51.h>
#include "system.h"
#include "tick.h"

#define TICK_INTERRUPT_PERIOD_CNT	(((XTAL*TICK_INTERRUPT_PERIOD_MS)/1000)/12)
#define MICRO_ADJUST	22
#define	TICK_PERIOD	((65536-TICK_INTERRUPT_PERIOD_CNT)+MICRO_ADJUST)

// system clock value
unsigned int SystemTick;

// read system clock
unsigned int GetSystemTick(void)
{
	// note : only read when the clock signal is stable
	//        -> stable when system clock time = time in the registor 
	volatile unsigned int StableTick;
	do
	{
		StableTick = SystemTick;
	}while(StableTick != SystemTick);
	return(StableTick);
}

// initialize
void InitTick(void)
{
	// clear system clock
	SystemTick = 0;
	// reset the clock mode
	TMOD &= 0xf0;
	TMOD |= 0x1;
	
	// stop clock
	TR0 = 0;
	// reset the overflow flag
	TF0 = 0;
	
	// load system clock value
	TH0 = TICK_PERIOD >> 8;
	TL0 = (unsigned char)TICK_PERIOD;
	
	// set clock interrupts has higher pirority
	PT0 = 1;
	// start system clock
	TR0 = 1;
	// set if timer = 0 then interrupts
	ET0 = 1;
  EA = 1;
}


// interrupts handling
void timer0 (void) interrupt 1 using 1
{
	// stop timer
	TR0 = 0;
	// re-load the system clock
	TH0 = TICK_PERIOD >> 8;
	TL0 = (unsigned char)TICK_PERIOD;
	// start timer again
	TR0 = 1;
	
	SystemTick++;
}
