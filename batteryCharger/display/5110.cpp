/*
 * Nokia 3110 / 5110 unbuffered driver
 * (C)2013 Radu Motisan
 * www.pocketmagic.net
 * Based on a work by Tony Myatt - 2007
 */

#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "5110.h"


// Alphabet lookup font
// we are actually using 6x8 pixels, but the last are for border
// this means in 84x48 we have 14x6 characters
// the list is ordonated by the ASCII codes 
const unsigned char PROGMEM font5x7 [][CHAR_WIDTH - 1] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
	{ 0x32, 0x34, 0x08, 0x16, 0x26 },   // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
    { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
    { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
    { 0x00, 0x7F, 0x3E, 0x1C, 0x08 },   // > Filled
	{ 0x08, 0x1C, 0x3E, 0x7F, 0x00 }, 	 // < Filled
	{ 0x08, 0x7C, 0x7E, 0x7C, 0x08 },   // Arrow up
	{ 0x10, 0x3E, 0x7E, 0x3E, 0x10 },   // Arrow down	
	{ 0x3E, 0x3E, 0x3E, 0x3E, 0x3E },   // Stop
	{ 0x00, 0x7F, 0x3E, 0x1C, 0x08 }    // Play
};

void Set(volatile uint8_t  *port, uint8_t  dq, int state) {
	if (state)
		*port |= (1<<dq);
	else
		*port &= ~(1<<dq);
}


/* Performs IO & LCD controller initialization */
void LCD_5110::lcd_init(
	volatile uint8_t  *port_SCE, uint8_t  dq_SCE,
	volatile uint8_t  *port_RST, uint8_t  dq_RST,
	volatile uint8_t  *port_DC, uint8_t  dq_DC,
	volatile uint8_t  *port_DATA, uint8_t  dq_DATA,
	volatile uint8_t  *port_CLK, uint8_t  dq_CLK
	)
{
	// save configuration
	m_port_SCE = port_SCE; m_dq_SCE = dq_SCE; 
	m_port_RST = port_RST; m_dq_RST = dq_RST;
	m_port_DC  = port_DC;  m_dq_DC = dq_DC;
	m_port_DATA= port_DATA;m_dq_DATA = dq_DATA;
	m_port_CLK = port_CLK; m_dq_CLK = dq_CLK;
	
	
    // Pull-up on reset pin
	Set(m_port_RST, m_dq_RST, 1); //LCD_PORT |= LCD_RST_PIN;
	
	// Set output bits on lcd port
	Set(Port2DDR(m_port_RST), m_dq_RST, 1);
	Set(Port2DDR(m_port_SCE), m_dq_SCE, 1);
	Set(Port2DDR(m_port_DC), m_dq_DC, 1);
	Set(Port2DDR(m_port_DATA), m_dq_DATA, 1);
	Set(Port2DDR(m_port_CLK), m_dq_CLK, 1);
	//LCD_DDR |= LCD_RST_PIN | LCD_SCE_PIN | LCD_DC_PIN | LCD_DATA_PIN | LCD_CLK_PIN;
    
	// Wait after VCC high for reset (max 30ms)
    _delay_ms(15);
    
    // Toggle display reset pin
    Set(m_port_RST, m_dq_RST, 0); //LCD_PORT &= ~LCD_RST_PIN;
    //lcd_delay(); /// delay macro function #define lcd_delay() for(int i=-32000;i<32000;i++)
	_delay_ms(64);
    Set(m_port_RST, m_dq_RST, 1);//LCD_PORT |= LCD_RST_PIN;

    // Disable LCD controller
    Set(m_port_SCE, m_dq_SCE, 0);//LCD_PORT |= LCD_SCE_PIN;

    lcd_send(0x21, LCD_CMD);  // LCD Extended Commands
    lcd_send(0xC8, LCD_CMD);  // Set LCD Vop(Contrast)
    lcd_send(0x06, LCD_CMD);  // Set Temp coefficent
    lcd_send(0x13, LCD_CMD);  // LCD bias mode 1:48
    lcd_send(0x20, LCD_CMD);  // Standard Commands, Horizontal addressing
    lcd_send(0x0C, LCD_CMD);  // LCD in normal mode
    
    // Clear lcd
    lcd_clear();
	

	// Set display contrast. Note: No change is visible at ambient temperature 
	int contrast = 0x40;
	lcd_send(0x21, LCD_CMD);				// LCD Extended Commands
	lcd_send(0x80 | contrast, LCD_CMD);		// Set LCD Vop(Contrast)
	lcd_send(0x20, LCD_CMD);				// LCD std cmds, hori addr mode
}



// Clears the display 
void LCD_5110::lcd_clear(void)
{
	lcdCacheIdx = 0;
	lcd_base_addr(lcdCacheIdx);
	// Set the entire cache to zero and write 0s to lcd
	
	int lcd_size =  ((LCD_WIDTH * LCD_HEIGHT) / CHAR_HEIGHT);
	
    for(int i=0;i<lcd_size;i++)  lcd_send(0, LCD_DATA);
    
}

// Clears an area on a line 
void LCD_5110::lcd_clear_area(unsigned char line, unsigned char startX, unsigned char endX)
{  
    // Start and end positions of line
    int start = line * LCD_WIDTH + startX;
    int end = line * LCD_WIDTH + endX;
	
	lcd_base_addr(start);
    
    // Clear all data in range from cache
    for(unsigned int i = start; i < end; i++) lcd_send(0, LCD_DATA);
}

// Clears an entire text block. (rows of 8 pixels on the lcd) 
void LCD_5110::lcd_clear_line(unsigned char line)
{
    lcd_clear_area(line, 0, LCD_WIDTH);
}

// Sets cursor location to xy location corresponding to basic font size 
void LCD_5110::lcd_goto_xy(unsigned char x, unsigned char y)
{
    lcdCacheIdx = x*CHAR_WIDTH + y * LCD_WIDTH;
}

// Sets cursor location to exact xy pixel location on the lcd 
void LCD_5110::lcd_goto_xy_exact(unsigned char x, unsigned char y)
{
    lcdCacheIdx = x + y * LCD_WIDTH;
}

// Displays a character at current cursor location or moves to new line if enter char is given
void LCD_5110::lcd_chr(char chr)
{
	if (chr == '\n') {
		// move to next line and first column
		lcdCacheIdx = lcdCacheIdx + LCD_WIDTH - lcdCacheIdx % LCD_WIDTH; //next line and x=0
	} else {
		lcd_base_addr(lcdCacheIdx);
	
		// 5 pixel wide characters and add space
		for(unsigned char i=0; i < CHAR_WIDTH - 1; i++) 
			lcd_send(pgm_read_byte(&font5x7[chr-32][i]) << 1, LCD_DATA);
		
		lcd_send(0, LCD_DATA); // right empty separator line of 8 vertical pixels
	
		lcdCacheIdx += CHAR_WIDTH;
	}		
}

// Displays null terminated string at current cursor location and increment cursor location 
void LCD_5110::lcd_str(char *str)
{
    while(*str)  lcd_chr(*str++);
}

// displays a formated string, similar to printf
void LCD_5110::lcd_string_format(char *szFormat, ...)
{
	char szBuffer[96]; //in this buffer we form the message
	int NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	va_list pArgs;
	va_start(pArgs, szFormat);
	vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);
	
	lcd_str(szBuffer);
}

// Set the base address of the lcd
void LCD_5110::lcd_base_addr(unsigned int addr) {
	lcd_send(0x80 |(addr % LCD_WIDTH), LCD_CMD);
	lcd_send(0x40 |(addr / LCD_WIDTH), LCD_CMD);
}

// Sends data to display controller : a line of 8 pixels, as defined by data
void LCD_5110::lcd_send(unsigned char data, LcdCmdData cd)
{
	// Data/DC are outputs for the lcd (all low)
	//LCD_DDR |= LCD_DATA_PIN | LCD_DC_PIN;
	Set(Port2DDR(m_port_DATA), m_dq_DATA, 1);
	Set(Port2DDR(m_port_DC), m_dq_DC, 1);
	
    // Enable display controller (active low)
    Set(m_port_SCE, m_dq_SCE, 0);//LCD_PORT &= ~LCD_SCE_PIN;

    // Either command or data
    /*if(cd == LCD_DATA) {
		
        LCD_PORT |= LCD_DC_PIN;
    } else {
        LCD_PORT &= ~LCD_DC_PIN;
    }*/
	Set(m_port_DC, m_dq_DC, (cd == LCD_DATA));
	
	for(unsigned char i=0;i<8;i++) {
		// Set the DATA pin value:8 bits in a row
		/*if((data>>(7-i)) & 0x01) {
			LCD_PORT |= LCD_DATA_PIN; //lcd pin high
		} else {
			LCD_PORT &= ~LCD_DATA_PIN; //lcd pin low
		}*/
		Set(m_port_DATA, m_dq_DATA, ((data>>(7-i)) & 0x01));
		// Toggle the clock
		
		Set(m_port_CLK, m_dq_CLK, 1);//LCD_PORT |= LCD_CLK_PIN;
		Set(m_port_CLK, m_dq_CLK, 0);//LCD_PORT &= ~LCD_CLK_PIN;
	}

	// Disable display controller
    Set(m_port_SCE, m_dq_SCE, 1);//LCD_PORT |= LCD_SCE_PIN;
	
	// Data/DC can be used as button inputs when not sending to LCD (/w pullups)
	//LCD_DDR &= ~(LCD_DATA_PIN | LCD_DC_PIN);
	Set(Port2DDR(m_port_DATA), m_dq_DATA, 0);
	Set(Port2DDR(m_port_DC), m_dq_DC, 0);
	//LCD_PORT |= LCD_DATA_PIN | LCD_DC_PIN;
	Set(m_port_DATA, m_dq_DATA, 1);
	Set(m_port_DC, m_dq_DC, 1);
}

void LCD_5110::lcd_col(char chr)
{
	lcd_base_addr(lcdCacheIdx);
    
	lcd_send(chr, LCD_DATA);
	
	lcdCacheIdx++;
}

// It goes back the cursor on LCD for a single step
void LCD_5110::lcd_pixelBack(void) 
{
	lcdCacheIdx--;
}

// Prints on LCD a hex based picture, A hex picture can be produced from the "LCDAssistant.exe" windows based software.  
void LCD_5110::printPictureOnLCD ( const unsigned char *data)
{
	int pixel_cols = LCD_WIDTH * (LCD_HEIGHT / CHAR_HEIGHT); //<6> means 6 lines on LCD.
 	lcd_goto_xy(0, 0);
	 
	for(int i=0;i<pixel_cols;i++)
		lcd_col(pgm_read_byte(data++));
		
	//_delay_ms(100);
}

  
// the most basic function, set a single pixel
void LCD_5110::drawPixel(unsigned char  x, unsigned char  y, int color) {
	int pixel_addr = x + y/CHAR_HEIGHT * LCD_WIDTH;
	
	lcd_send(0x80 |(pixel_addr % LCD_WIDTH), LCD_CMD);
	lcd_send(0x40 |(pixel_addr / LCD_WIDTH), LCD_CMD);
	if (color)
		lcd_send(1<<(y % CHAR_HEIGHT), LCD_DATA); //"black pixel"
	else
		lcd_send(0, LCD_DATA); //"remove black pixel"
}

