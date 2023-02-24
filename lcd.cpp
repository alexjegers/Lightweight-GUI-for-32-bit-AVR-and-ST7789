/*
 * lcd.cpp
 *
 * Created: 10/16/2022 6:59:16 PM
 *  Author: AJ992
 */ 
/*
Function:
Params:
Returns:
Description:
*/


#include <avr32/io.h>
#include <stdint.h>
#include <string.h>
#include "system.h"
#include "io.h"
#include "lcd.h"
#include <delay.h>
#include "fonts.h"
#include <math.h>

/*
Function: init
Params: none
Returns: none
Description: Configures the LCD control and data pins. Resets the LCD, turns it on
			loads basic settings, and fills the screen all black.
*/
void ST7789::init()
{
	/*Give GPIO control of the LCD control pins*/
	ioSetPinIO(&LCD_PORT, LCD_WR_bm 
						| LCD_RD_bm 
						| LCD_RESET_bm 
						| LCD_DC_bm 
						| LCD_CS_bm);
						
	/*Set all the LCD control pins as outputs*/
	ioSetPinOutput(&LCD_PORT, LCD_WR_bm
							| LCD_RD_bm
							| LCD_RESET_bm
							| LCD_DC_bm
							| LCD_CS_bm);
	
	/*Drive all pins high to start*/
	ioPinHigh(&LCD_PORT, LCD_WR_bm
						| LCD_RD_bm
						| LCD_RESET_bm	
						| LCD_DC_bm
						| LCD_CS_bm);
						
	/*Allow GPIO to control the data lines, and set them as outputs*/					
	ioSetPinIO(&LCD_DATA_PORT, LCD_DATA_PINS_bm);
	ioSetPinOutput(&LCD_DATA_PORT, LCD_DATA_PINS_bm);
	
	hwReset();							//Hardware reset.
	swReset();							//Software reset.
	delay_us(100000);					//Required after reset.
	exitSleep();						//Screen is in sleep mode after reset, take it out of sleep.
	delay_us(100000);					//Required after exiting sleep mode.
	setColorMode();						//Change the color settings.
	displayOn();						//Turn the display on.
	
	/*Fills the screen all black*/
	setDisplayArea(X_MIN, Y_MIN, X_MAX, Y_MAX);
	begin();
	writeAddr(ST7789_RAMWR);
	for (int i = 0; i < MAX_PIXELS; i++)
	{
		writeData(0x00);
		writeData(0x00);
	}
	end();
}

/*
Function: hwReset
Params: none
Returns: none
Description: Hardware reset, see datasheet for default HW reset values.
*/
void ST7789::hwReset()
{
	ioPinLow(&LCD_PORT, LCD_RESET_bm);		//Drive reset low.
	delay_ms(200);							//Hold for 200ms for good measure (10us minimum is required).
	ioPinHigh(&LCD_PORT, LCD_RESET_bm);		//Release the reset.
	delay_ms(250);							//Wait for 250ms before doing anything (120ms is required).
}

/*
Function: begin
Params: none
Returns: none
Description: Pulls chip select low. This signals the start of a transmission.
*/
void ST7789::begin()
{
	ioPinLow(&LCD_PORT, LCD_CS_bm);				//Drive CS low starts transmission.
}

/*
Function: end
Params: none
Returns: none
Description: Pulls chip select high, this signals the end of a transmission.
*/
void ST7789::end()
{
	ioPinHigh(&LCD_PORT, LCD_CS_bm);			//Drive CS high ends transmission.
}

/*
Function: writeAddr
Params: addr: one byte address.
Returns: none
Description: writes an address to the LCD, controls the DC/X pin automatically,
			does not toggle chip select.
*/
void ST7789::writeAddr(uint8_t addr)
{
	/*DC pin has to get set before driving WR low*/
	ioPinLow(&LCD_PORT, LCD_DC_bm);								//DC pin low when sending an address.
	ioPinLow(&LCD_PORT, LCD_WR_bm);								//WR pin low before data gets written to bus.
	
	/*Write the address to the data bus on Port B*/
	ioPinHigh(&LCD_DATA_PORT, (uint32_t)addr & 0x000000FF);		 
	ioPinLow(&LCD_DATA_PORT, ~((uint32_t)addr) & 0x000000FF);
	
	ioPinHigh(&LCD_PORT, LCD_WR_bm);							//WR high latches in the data (has to stay high minimum 15ns).	
}

/*
Function: writeData
Params: data: one byte.
Returns: none
Description: writes one byte data or parameter to the LCD. Function controls 
			the DC/X pin but not the chip select pin. Must repeatedly call function
			to write more than one byte.
*/
void ST7789::writeData(uint8_t data)
{
	/*DC pin has to get set before driving WR low*/
	ioPinHigh(&LCD_PORT, LCD_DC_bm);							//DC pin high when sending a data.
	ioPinLow(&LCD_PORT, LCD_WR_bm);								//WR pin low before data gets written to bus.
	
	/*Write the address to the data bus on Port B*/
	ioPinHigh(&LCD_DATA_PORT, (uint32_t)data & 0x000000FF);
	ioPinLow(&LCD_DATA_PORT, ~((uint32_t)data) & 0x000000FF);

	ioPinHigh(&LCD_PORT, LCD_WR_bm);							//WR high latches in the data (has to stay high minimum 15ns).
}

/*
Function: displayOn
Params: none
Returns: none
Description: Turns the display on. Turning the display on causes the contents
			of the LCD RAM to be visible.
*/
void ST7789::displayOn()
{
	begin();
	writeAddr(ST7789_DISPON);
	delay_us(3);
	end();
}

/*
Function: exitSleep
Params: none
Returns: none
Description: The LCD is in sleep mode after a reset, some registers are not accessible
			when in sleep. This function takes the LCD out of sleep mode. Delay is
			required after exiting sleep before anything else is written.
*/
void ST7789::exitSleep()
{
	begin();
	writeAddr(ST7789_SLPOUT);
	end();
	delay_ms(1000);				//Must wait at least 120ms before sending other commands.
}

/*
Function: readData
Params: none
Returns: LCD_DATA_PORT: one byte received from the LCD.
Description: This is called after writing an address to the screen. Function
			handles DC/X, RD, and WR pins. Returns one byte received from the
			LCD.
*/
uint8_t ST7789::readData()
{
	/*Set the data pins to inputs*/
	ioSetPinInput(&LCD_DATA_PORT, LCD_DATA_PINS_bm);
	
	ioPinHigh(&LCD_PORT, LCD_DC_bm);					//This has to be set before RD moves low.
	delay_us(5);										//Minimum time 0ns.
	ioPinLow(&LCD_PORT, LCD_RD_bm);						
	delay_us(5);										//RD has to be low for 15ns before going high.
	ioPinHigh(&LCD_PORT, LCD_RD_bm);					//This will strobe the data.
	delay_us(5);										//Has to be held hig for 15ns.
	
	/*Return the first byte of port B*/
	return (uint8_t)(ioReadPort(&LCD_DATA_PORT) & 0x000000FF);
}

/*
Function: readStatus
Params: none
Returns: none
Description: reads the LCD status register (RDDST, 0x09) into the read buffer array.
*/
void ST7789::readStatus()
{
	begin();
	writeAddr(ST7789_RDDST);
	for (uint8_t i = 0; i < 4; i++)
	{
		readBuffer[i] = readData();
	}
	end();
	delay_ms(1000);
}

/*
Function: setColorMode
Params: none
Returns: none
Description: Sets the color mode settings to 16 bit and 65k colors.
*/
void ST7789::setColorMode()
{
	begin();
	writeAddr(ST7789_COLMOD);
	writeData(ST7789_COLMOD_65K | ST7789_COLMOD_16BIT);
	end();
}

/*
Function: swReset
Params: none
Returns: none
Description: Performs a software reset of the LCD driver.
*/
void ST7789::swReset()
{
	begin();
	writeAddr(ST7789_SWRESET);
	end();
	delay_ms(250);						//Minimum of 120ms required.
}

/*
Function: setDisplayArea
Params: x1: the starting x coordinate.
		y1: the starting y coordinate.
		x2: the ending x coordinate.
		y2: the ending y coordinate.
Returns: none
Description: Sets the area of the screen that the next write to the LCD
			RAM will be displayed.
*/
void ST7789::setDisplayArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	begin();
	writeAddr(ST7789_CASET);
	writeData(x1 >> 8);
	writeData(x1);
	writeData(x2 >> 8);
	writeData(x2);
	end();
	
	begin();
	writeAddr(ST7789_RASET);
	writeData(y1 >> 8);
	writeData(y1);
	writeData(y2 >> 8);
	writeData(y2);
	end();
}

/*
Function: clearDisplay
Params: none
Returns: none
Description: Fills the contents of the LCD RAM with black.
*/
void ST7789::clearDisplay()
{
	setDisplayArea(X_MIN, Y_MIN, X_MAX, Y_MAX);
	begin();
	writeAddr(ST7789_RAMWR);
	for (int i = 0; i < MAX_PIXELS; i++)
	{
		writeData(0x00);
		writeData(0x00);
	}
	end();
}

/*
Function: drawRectangle
Params: x1: the left most edge of the rectangle.
		y1: the top most edge of the rectangle.
		x2: the right most edge of the rectangle.
		y2: the bottom edge of the rectangle.
Returns: none
Description: Writes a rectangle to the screen constrained by the coordinates
			(x1,y1) and (x2,y2), top left and bottom right corners respectively.
*/
void ST7789::drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	setDisplayArea(x1, y1, x2, y2);
	begin();
	writeAddr(ST7789_RAMWR);
	for (uint16_t y = y1; y <= y2; y++)
	{
		for (uint16_t x = x1; x <= x2; x++)
		{
			writeData(color >> 8);
			writeData(color);
		}
	}
	end();
}

/*
Function: drawCircle
Params: x: x location of the center of the circle.
		y: the y location of the center of the circle.
		radius: radius of the circle in pixels.
		color: use color macro, in lcd.h defines.
Returns:
Description:
*/
void ST7789::drawCircle(int16_t x, int16_t y, int16_t radius, int16_t color)
{
	int16_t radiusSquared = pow(radius, 2);
	int16_t xStart = x - radius;
	int16_t xEnd = x + radius;
	int16_t yStart = y - radius;
	int16_t yEnd = y + radius;
	
	setDisplayArea(xStart, yStart, xEnd, yEnd);
	begin();
	writeAddr(ST7789_RAMWR);
	
	for (int16_t _y = yStart; _y <= yEnd; _y++)
	{
		for (int16_t _x = xStart; _x <= xEnd; _x++)
		{
			
			if ((pow(_y - y, 2)) + (pow(_x - x, 2)) < radiusSquared)
			{
				writeData(color >> 8);
				writeData(color);
			}
			else
			{
				writeData(0x00);
				writeData(0x00);
			}
		}
	}
	end();
}

int16_t _x1;
int16_t _x2;
int16_t _y1;
int16_t _y2;
int16_t y;
int16_t x;
//http://benice-equation.blogspot.com/2016/10/equation-of-rounded-rectangle.html
void ST7789::drawRoundedRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t radius, uint16_t color, uint16_t backgroundColor)
{

	setDisplayArea(x1, y1, x2, y2);
	_x1 = x1 - ((x1 + x2) / 2);
	_x2 = x2 - ((x1 + x2) / 2);
	_y1 = y1 - ((y1 + y2) / 2);
	_y2 = y2 - ((y2 + y1) / 2);	
	int16_t a = _x2 - radius;
	int16_t b = _y2 - radius;
	radius++;	
	int16_t radiusSquared = pow(radius, 2);

	begin();
	writeAddr(ST7789_RAMWR);
	for (y = _y1; y <= _y2; y++)
	{
		for (x = _x1; x <= _x2; x++)
		{
			/*Area 1*/
			if ((abs(x) >= a) && (abs(y) >= b))
			{
				if ((pow(abs(y) - b, 2)) + (pow(abs(x) - a, 2)) <= radiusSquared)
				{
					writeData(color >> 8);
					writeData(color);		
				}
				else
				{
					writeData(backgroundColor >> 8);
					writeData(backgroundColor);
				}
			}
			/*Area 2*/
			if ((abs(x) >= a) && (abs(y) < b))
			{
				writeData(color >> 8);
				writeData(color);				
			}
			/*Area 3*/
			if ((abs(x) < a) && (abs(y) >= b))
			{
				writeData(color >> 8);
				writeData(color);				
			}
			/*Area 4*/
			if ((abs(x) < a) && (abs(y) < b))
			{
				writeData(color >> 8);
				writeData(color);		
			}

		}
	}
	end();		
}

void ST7789::drawBorderedRoundedRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t radius, uint8_t borderWidth, uint16_t borderColor, uint16_t insideColor, uint16_t backgroundColor)
{
	drawRoundedRectangle(x1, y1, x2, y2, radius, borderColor, backgroundColor);
	drawRoundedRectangle(x1 + borderWidth, y1 + borderWidth, x2 - borderWidth, y2 - borderWidth, radius, insideColor, borderColor);
}
/*
Function: drawBorderedRect
Params: x1: the left most edge of the rectangle.
		y1: the top most edge of the rectangle.
		x2: the right most edge of the rectangle.
		y2: the bottom edge of the rectangle.
		borderWidth: width of the border in pixels.
		borderColor: color of the border.
		insideColor: color of the rectangle inside the border.
Returns: none
Description: Writes a rectangle with a border to the screen constrained by the coordinates
			(x1,y1) and (x2,y2), top left and bottom right corners respectively.
*/
void ST7789::drawBorderedRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t borderWidth, uint16_t borderColor, uint16_t insideColor)
{
	drawRectangle(x1, y1, x2, y2, borderColor);						//Rectangle border.
	drawRectangle(x1 + borderWidth, y1 + borderWidth, 
				x2 - borderWidth, y2 -borderWidth, insideColor);	//Inner rectangle.	
}

/*
Function: drawText
Params: let[]: Array of character to write to the screen.
		x1: the position of the top left corner of the first character on the x axis.
		y1: the position of the top left corner of the first character on the y axis.
		*font: pointer to the array of the desired font.
Returns: none
Description: writes a string to the screen in a certain location in the selected font.
*/
 void ST7789::drawText(char let[], uint16_t x1, uint16_t y1, const uint8_t font[], uint16_t charColor, uint16_t backColor)
{
	uint8_t stringLength = 0;
	uint16_t charWidth = font[0];
	uint16_t charHeight = font[1];
	uint16_t charPadding = font[2];
	
	//stringLength = strlen(let);
	for (uint8_t i = 0; i <= 9; i++)
	{
		if (i == 9)
		{
			stringLength = 9;
			break;
		}
		if (let[i] == 0)
		{
			stringLength = i;
			break;
		}
	}
	
	for (unsigned int j = 0; j < stringLength; j++) //This loop iterates the following for each character in the word.
	{
		char currentChar = let[j];
		
		//Defines the range of the screen that a single character will take up and
		//adjusts over to the right for each character when the loop iterates.
		//////////////////////////////////////////////////////////////////////////
 		setDisplayArea(x1 + (j*(charWidth - charPadding)), y1, x1 + charWidth + ((j*(charWidth-charPadding))), y1+charHeight);
		//////////////////////////////////////////////////////////////////////////
		
		begin();
		writeAddr(0x2C); //Starts the write to frame memory
		uint16_t totalBytes = ((charWidth + 1) / 8) * (charHeight);		//Determines total number of bytes in the whole character,
																//this determines how many writes will be made to frame memory.
		uint8_t currentBit = 0;

		for (unsigned int byteCounter = 0; byteCounter < totalBytes; byteCounter++)
		{
			//For loop below sets val to one byte based on byteCounter and parses through
			//each bit in the byte and writes 0x0000 (white) for each 1 bit and 0xffff
			//(black) for each 0 bit. This happens for each byte in the array until
			//byteCounter == totalBytes.
			//////////////////////////////////////////////////////////////////////////
			for (int i = 0; i < 8; i++)
			{
				currentBit = (font[(((currentChar - 31) * totalBytes)) + byteCounter]);	//[(currentChar - 31) * 105] converts the char ASCII value to the correct index in the array
				currentBit = (currentBit >> i) & 0x01;
				if (currentBit == 1)
				{
					writeData(charColor >> 8);
					writeData(charColor);
				}
				else
				{
					writeData(backColor >> 8);
					writeData(backColor);
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}
	end();
}