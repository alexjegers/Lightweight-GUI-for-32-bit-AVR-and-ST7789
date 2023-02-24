/*
 * lcd.h
 *
 * Created: 10/11/2022 12:12:43 PM
 *  Author: AJ992
 */ 


#ifndef LCD_H_
#define LCD_H_

#define LCD_PORT					PORTA
#define LCD_BACKLIGHT_PIN_bm		1 << 11
#define LCD_CS_bm					1 << 12
#define LCD_RD_bm					1 << 13
#define LCD_DC_bm					1 << 14
#define LCD_WR_bm					1 << 15
#define LCD_RESET_bm				1 << 16

#define LCD_DATA_PORT				PORTB
#define LCD_DATA_PINS_bm			255

/*ST7789V Macros*/
#define ST7789_MADCTL_MY					1 << 7
#define ST7789_MADCTL_MX					1 << 6
#define ST7789_MADCTL_MV					1 << 5
#define ST7789_MADCTL_ML					1 << 4
#define ST7789_MADCTL_RGB					1 << 3
#define ST7789_MADCTL_MH					1 << 2
#define ST7789_DISPON						0x29
#define ST7789_SLPOUT						0x11
#define ST7789_RAMWR						0x2C
#define ST7789_RDDST						0x09
#define ST7789_COLMOD						0x3A
#define ST7789_COLMOD_65K					(0x5 << 4)
#define ST7789_COLMOD_16BIT					0x5
#define ST7789_SWRESET						0x01
#define ST7789_CASET						0x2A
#define ST7789_RASET						0x2B
#define ST7789_RDDID						0x04

/*Max screen coordinates*/
#define X_MIN						0
#define X_MAX						239
#define Y_MIN						0
#define Y_MAX						319
#define MAX_PIXELS					76800

#define COLOR_BLACK					0x0000
#define COLOR_WHITE					0xFFFF
#define COLOR_PINK					0xFCFC
#define COLOR_BLUE					0x5C1F
#define COLOR_GRAY					0xBDF7

class ST7789
{
	public:	
	void init();																			//Configures pins and initializes LCD.
	void clearDisplay();																	//Fills the screen black.
	void setDisplayArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);				//Sets the start and end points on the screen.
	void drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);	//Draws a rectangle constrained by (x1,y1) and (x2, y2).
	void drawBorderedRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,				//Draws a rectangle with a border.
						uint8_t borderWidth, uint16_t borderColor, uint16_t insideColor);
	void drawCircle(int16_t x, int16_t y, int16_t radius, int16_t color);
	void drawRoundedRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
						int16_t radius, uint16_t color, uint16_t backgroundColor);
	void drawBorderedRoundedRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
						int16_t radius, uint8_t borderWidth, uint16_t borderColor,
						uint16_t insideColor, uint16_t backgroundColor);
	void drawText(char let[], uint16_t x1, uint16_t y1, const uint8_t font[],				//Writes a string to the screen in desired font.
						uint16_t charColor, uint16_t backColor);				
	private:
	uint8_t readBuffer[5];								//Holds data when reading from the screen.
	void displayOn();									//Turns on the display.
	void exitSleep();									//Pulls lcd out of sleep mode.
	void setColorMode();								//Puts it in 16 bit, 65k mode.
	void hwReset();										//Hardware reset.
	void swReset();										//Software reset.
	void readStatus();									//Reads the LCD status register into readBuffer[] array.				
	void writeData(uint8_t data);						//Writes one byte with DC high.
	void writeAddr(uint8_t addr);						//Writes  1 byte with DC low.
	uint8_t readData();									//Returns a byte read from the screen.
	void begin();										//Pulls CS low.
	void end();											//Pulls CS high.
};



#endif /* LCD_H_ */