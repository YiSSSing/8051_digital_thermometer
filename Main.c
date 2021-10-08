

/*
This is an project to learn embedded system by USING 8051 MCU as example.
The project is a digital temperature, built by 89C51 and DS18B20.

The system is USING memory-mapping IO and Crystal as system clock (fast clock).

*/



#include <reg51.h>
#include "tick.h"
#include "ds18xx.h"
#include "lcm.h"
#include "cursorpos.h"

void DisplayLCDMonitor(void);
inline unsigned char C2F(unsigned char);

typedef enum enumOperatingMode
{
	OP_StartConvertT,
	OP_ReadTemper,
	OP_End
}OperatingModeIndex;

unsigned char OpMode;
unsigned int tSystemTick;
unsigned char CurrentTemperture;

code unsigned char Str_NowTemperature[]	= "Now Temperature";

main()
{
	// initail system clock
	InitTick();
	// initial LCD monitor
	LCD_Startup();
	// initial DS18B20 temperautre detector
	InitDS18XX();
	
	// reset LCD monitor
	CursorDisplay(0);	
	ClearDisplay();
	DisplayLCDMonitor();
	
	OpMode = OP_StartConvertT;
	
	while(1)
	{
		switch(OpMode)
		{
			// convert the temperature for LCD monitor to display
			case OP_StartConvertT:
				StartConvertTemperature();	
			  // read system clock
				tSystemTick = GetSystemTick();
				OpMode = OP_ReadTemper;	
				break;
				
			// read the temperature registor in DS18B20
			case OP_ReadTemper:	
				// read the registor every 300 ms
				if((GetSystemTick() - tSystemTick) >= MS_300)	
				{	
					CurrentTemperture = ReadTemperature();
					// display the temperature on LCD
					DspLocDecByte(TEMPERATURE_CX,TEMPERATURE_CY,CurrentTemperture,2,ZERO_OFF); 
					DspLocDecByte(TEMPERATURE_FX,TEMPERATURE_FY,C2F(CurrentTemperture),3,ZERO_OFF);
					OpMode = OP_StartConvertT;	
				}
				break;
		}
	}
}


// Change *C to *F
inline unsigned char C2F(unsigned char cval)
{
	unsigned int fval = (unsigned int) cval * 9;
	fval /= 5;	
	fval += 32;	
	return((unsigned char) fval);
}


// Display the temperature on LCD monitor
void DisplayLCDMonitor(void )
{
	// define the target string to display
	DspLocString(MENU_L0_X, MENU_L0_Y, &Str_NowTemperature);
	DspLocChar(STR_C_X, STR_C_Y,SYM_DEGREE);
	DisplayChar('C');
	DspLocChar(STR_F_X, STR_F_Y,SYM_DEGREE);
	DisplayChar('F');
}
