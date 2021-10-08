#include <reg51.h>
#include <INTRINS.H>
#include "ds18xx.h"

// the pin position for DS18B20
sbit DS18XX_DQ = P3^7;	

void ResetPresence(void);
void DelayUs(unsigned char);
unsigned char SendReceiveByte(unsigned char);
bit SendReceiveBit(bit);

// initialize DS18B20: set to high volatage
void InitDS18XX(void)
{
	DS18XX_DQ = 1;	
}
	
// convert the digital temperature data to standard type
void StartConvertTemperature(void)
{
	ResetPresence();
	SendReceiveByte(0xcc);
	// active the temperature convert operation
	SendReceiveByte(0x44);	
}

// read temperature
unsigned char ReadTemperature(void)
{
	unsigned char tempL=0; 
	unsigned char tempH=0; 
	// reset bus
	ResetPresence();
	SendReceiveByte(0xcc);
	// read temperature registor
	SendReceiveByte(0xbe);	
	
	// read the low position of the temperature byte
	tempL = SendReceiveByte(0xff);
	// read the high position of the temperature byte
	tempH = SendReceiveByte(0xff);
	
	tempL >>= 4;
	tempH <<= 4;
	tempL += tempH;
	// 
	if(tempL >= 80) tempL = 0;
		
	return(tempL);
}


// send a 1 byte and receive 1 byte 
unsigned char SendReceiveByte(unsigned char dat)
{
	unsigned char cnt;
	bit rbit;

	for(cnt = 0; cnt < 8; cnt++)
	{
		// start the transmission from LSB(bit0)
		rbit = SendReceiveBit(dat & 1);	
		dat >>= 1;
		
		// put the receive dat in MSB (bit7)
		if(rbit) dat |= 0x80;
	}
	
	return(dat);
}
	
// send one BIT and receive one BIT
bit SendReceiveBit(bit bdat)
{
	unsigned char dly;
	
	DS18XX_DQ = 0;
	// send nop to wait instead of use system clock for performance
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	// send dat
	DS18XX_DQ = bdat;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	// receive dat
	bdat = DS18XX_DQ;	
	
	for(dly = 0; dly < 12; dly++);
	
	DS18XX_DQ = 1;	
	
	return(bdat);
}


// start the transmission function of 1-wire bus
// method: make DQ at low voltage
void ResetPresence(void)
{
	// send reset signal
	DS18XX_DQ = 0;
	// wait for signal become stable
	DelayUs(70);
	
	// start DS18B20 again
	DS18XX_DQ = 1;
	// wait for signal become stable
	DelayUs(3);
	// wait for the response from DS18B20
	while(DS18XX_DQ);	
	while(!DS18XX_DQ);
}
	
// simple delay
void DelayUs(unsigned char us)
{
	for(;us != 0; us--)	
	{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}
