#define LCMCMD_CursorShiftLeft	0
#define LCMCMD_CursorShiftRight	1

#define ZERO_OFF	0	
#define ZERO_ON		1	
#define SYM_DEGREE	0xdf	

void LCD_Startup(void);
void DspLocHexByte(unsigned char, unsigned char, unsigned char);
void DspLocDecWord(unsigned char, unsigned char, unsigned int, unsigned char,bit);
void DspLocDecByte(unsigned char,unsigned char,unsigned char,unsigned char,bit);
void DspLocChar(unsigned char, unsigned char, unsigned char);
void DisplayChar(unsigned char);
void DspLocString(unsigned char, unsigned char, unsigned char *);
void DisplayString(unsigned char *);
void DspHexWord(unsigned int);
void DspHexByte(unsigned char);
void DspDecWord(unsigned int, unsigned char,bit);
void DspDecByte(unsigned char,unsigned char,bit);
void LocateCursor(unsigned char, unsigned char);
void ClearDisplay(void);
void ClearRowDisplay(unsigned char);
void CursorDisplay(bit);
void CursorShift(bit);
void DisplayChar(unsigned char);
