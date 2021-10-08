// define the LCD operation
#include <reg51.h>
#include "system.h"
#include "lcm.h"

#define LCM_LINE_NUM	2	
#define LCM_LINE_CHARS	16		//number of characters in each line
#define LCM_LINE1_START	0x80	//start position of first row
#define LCM_LINE2_START	0xc0	//start position of second row
#define LCM_LINE3_START 0x90	//start position of third row
#define LCM_LINE4_START 0xD0	//start position of 4'th row

// define LCD operation
#define CMD_ClearDisplay	1
#define CMD_EntryMode		4
	#define bCursorShiftR	2
	#define bEntireShift	1
#define CMD_DisplayControl	0x8
	#define bDISPLAY_ON		4
	#define bCURSOR_ON		2
	#define bCURSOR_BLINK	1
#define CMD_LocateCursor	0x80
#define CMD_FunctionSet		0x20

	// the width of DATA in the interface
	// 0 = 4 bits (DB7-DB4)
	// 1 = 8 bits (DB7-DB0)
	#define bBus8Bit		0x10
	// number of rows to diaplay
	// 0 = 1 row only
	// 1 = 2 row
	#define bRow2			8
	#define bSetPattern		4

// LCD DATA pins
sbit LCD_B7 = P0^5;	//pin 7
sbit LCD_B6 = P0^4;	//pin 6
sbit LCD_B5 = P0^3;	//pin 5
sbit LCD_B4 = P0^2;	//pin 4
// LCD control pins
sbit LCD_RS	= P0^0;	//RS
sbit LCD_EN	= P0^1;	//EN

void InitLcdCommand(void);
void LcdWriteCommand(unsigned char);
void LcdWriteData(unsigned char);
void LcdEnPulse(void);
void Delayms(int);
void DspHexNibble(unsigned char);
void OutLcdPort(unsigned char);

unsigned char DspCtrlCommand;

// initialize
void LCD_Startup(void)
{
	DspCtrlCommand = CMD_DisplayControl+bDISPLAY_ON+bCURSOR_ON+bCURSOR_BLINK;
	LCD_EN = 0;
	LCD_RS = 0;
	
	OutLcdPort(0x30);	
	LcdEnPulse();	
	OutLcdPort(0x30);
	LcdEnPulse();	
	OutLcdPort(0x30);	
	LcdEnPulse();
	OutLcdPort(0x20);	
	LcdEnPulse();	
	
	InitLcdCommand();	
	
	Delayms(100);
}

// set the start position of LCD displayer
void LocateCursor(unsigned char loc_x, unsigned char loc_y)
{
	// if is in first row, second row, third row or 4'th row
	if(loc_y == 0) loc_x += LCM_LINE1_START;
	else if(loc_y == 1) loc_x += LCM_LINE2_START;
	else if(loc_y == 2)	loc_x += LCM_LINE3_START;
	else loc_x += LCM_LINE4_START;

	LcdWriteCommand(loc_x);
}

// display or disable the cursor
void CursorDisplay(bit cursw)
{
	DspCtrlCommand &= ~(bCURSOR_ON+bCURSOR_BLINK);
	if (cursw) DspCtrlCommand |= (bCURSOR_ON+bCURSOR_BLINK);
	LcdWriteCommand(DspCtrlCommand);
}

// display the hex number to start position
void DspLocHexByte(unsigned char loc_x, unsigned char loc_y, unsigned char lcm_hex)
{
	// set the start position
	LocateCursor(loc_x,loc_y);	
	// display the first 4 bits
	DspHexNibble(lcm_hex >> 4);	
	// display the low 4 bits
	DspHexNibble(lcm_hex & 0xf);
}

// display the decimal number to start position
void DspLocDecWord(unsigned char loc_x, unsigned char loc_y, unsigned int dec_word, unsigned char digi, bit fhdig)
{
	LocateCursor(loc_x,loc_y);
	DspDecWord(dec_word, digi, fhdig);
}


// display the decimal number to start position
// note this will display the number in byte DATA
// digi: how many bits to display
// fhdig: invisible the leading 0
void DspLocDecByte(unsigned char loc_x,unsigned char loc_y,unsigned char dec_byte,unsigned char digi,bit fhdig)
{
	LocateCursor(loc_x,loc_y);	
	DspDecByte(dec_byte,digi,fhdig);	
}

// display the character to start position
void DspLocChar(unsigned char loc_x, unsigned char loc_y, unsigned char c)
{
	LocateCursor(loc_x,loc_y);	
	DisplayChar(c);	
}

// display the string to start position
void DspLocString(unsigned char loc_x, unsigned char loc_y, unsigned char *str)
{
	LocateCursor(loc_x,loc_y);
	DisplayString(str);
}

// display the string to current position
void DisplayString(unsigned char *str)
{
	while (*str != 0)	DisplayChar(*str++);                       
}

// display the decimal number to current position
void DspHexWord(unsigned int lcm_hex)
{
	DspHexByte(lcm_hex >> 8);	
	DspHexByte(lcm_hex & 0xff);	
}

// display the hex number to current position
void DspHexByte(unsigned char lcm_hex)
{
	DspHexNibble(lcm_hex >> 4);
	DspHexNibble(lcm_hex & 0xf);
}

// display the hex number in bytes to current position
void DspDecWord(unsigned int dec_word, unsigned char digi, bit fhdig)
{
	unsigned char hdig;
	unsigned int eten;
	unsigned char tcnt;
	
	while(digi != 0)
	{
		eten = 1;
		for(tcnt = 0; tcnt < digi-1; tcnt++) eten *= 10;
			
		hdig = (unsigned int)dec_word/eten;
		if (!fhdig && (hdig == 0) && (digi != 1)) DisplayChar(' ');	
		else
		{
			DspHexNibble(hdig);	
			fhdig = 1;
		}
		dec_word -= (hdig*eten);
		digi--;
	}
}

// display the decimal number in bytes to current position
void DspDecByte(unsigned char dec_byte, unsigned char digi, bit fhdig)
{
	unsigned char hdig;
	unsigned char eten;
	unsigned char tcnt;
	
	while(digi != 0)	
	{
		eten = 1;	
		for(tcnt = 0; tcnt < digi-1; tcnt++) eten *= 10;
			
		hdig = dec_byte/eten;
		if (!fhdig && (hdig == 0) && (digi != 1)) DisplayChar(' ');
		else
		{
			DspHexNibble(hdig);
			fhdig = 1;
		}
		dec_byte -= (hdig*eten);
		digi--;
	}
}


void DspHexNibble(unsigned char nibb_hex)
{
	nibb_hex += '0';
	if ( nibb_hex > '9' ) nibb_hex += 7;
	DisplayChar(nibb_hex);
}

// display character in current position
void DisplayChar(unsigned char chr)
{
	LcdWriteData(chr);
}

// clear the content on the LCD
void ClearDisplay(void)
{
#if 0
	LcdWriteCommand(0x01);
#else
	unsigned char i,j;
	
	for(j = 0; j < LCM_LINE_NUM; j++)	
	{
		LocateCursor(0,j);
		for (i = 0; i < LCM_LINE_CHARS; i++) DisplayChar(' ');
	}
#endif
}

// clear one row on the LCD
void ClearRowDisplay(unsigned char row)
{
	unsigned char i;
	
	LocateCursor(0,row);
	for (i = 0; i < LCM_LINE_CHARS; i++) DisplayChar(' ');
}


void CursorShift(bit dir)
{
	LcdWriteCommand((dir)? 0x14:0x10);
}

// initialize
void InitLcdCommand(void)
{
	// set to 4-bits control mode
	LcdWriteCommand(CMD_FunctionSet+bRow2);
	LcdWriteCommand(DspCtrlCommand);
	LcdWriteCommand(0x01);
}

// write command to LCD
void LcdWriteCommand(unsigned char cmd)
{
	// wrtie to 4 LSB bits to IO pin
	OutLcdPort(cmd);
	LCD_RS = 0;	
	LcdEnPulse();
	
	// write to 4 MSB bits to IO pin
	cmd <<= 4;
	OutLcdPort(cmd);
	LCD_RS = 0;
	LcdEnPulse();
}

// Write DATA to LCD
void LcdWriteData(unsigned char dat)
{
	// wrtie to 4 LSB bits to IO pin
	OutLcdPort(dat);
	LCD_RS = 1;	
	LcdEnPulse();
	
	// write to 4 MSB bits to IO pin
	dat <<= 4;
	OutLcdPort(dat);
	LCD_RS = 1;
	LcdEnPulse();	
}

// set LCD signals
void LcdEnPulse(void)
{
	unsigned char i;
	
	// note : need to wait signals become stable
	LCD_EN = 1;
	for (i = 0; i<8; i++);
	LCD_EN = 0;
	for (i = 0; i<8; i++);
}

// output to IO pins
void OutLcdPort(unsigned char dat)
{
	LCD_B7 = (dat & 0x80) ? 1:0;	// output to pin 7
	LCD_B6 = (dat & 0x40) ? 1:0;	// output to pin 6
	LCD_B5 = (dat & 0x20) ? 1:0;	// output to pin 5
	LCD_B4 = (dat & 0x10) ? 1:0;	// output to pin 4
}

// delay method
#define DELAY_1MS	160	//使用 12MHz 振盪器時
void Delayms(int ms)
{
	unsigned int i,j;
	// delay for 1 ms
	for (i = 0; i < ms; i++)
	    for (j = 0; j < DELAY_1MS; j++);
}
