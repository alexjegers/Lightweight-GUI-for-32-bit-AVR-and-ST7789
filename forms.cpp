/*
 * forms.cpp
 *
 * Created: 11/30/2022 11:57:23 AM
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <intc.h>
#include "io.h"
#include "iic.h"
#include "lcd.h"
#include "forms.h"

/*
Function: switchInterruptHanlder
Params: none 
Returns: none
Description: Interrupt handler for when a switch gets pressed.
*/
__attribute__((__interrupt__))void form::switchInterruptHanlder()
{
	button* btn = button::focusedBtn;
	switch (ioIntFlags(&SW_PORT) & SWITCHES_bm)
	{
		case CENTER_SWITCH:
		/*If theres no event handlers for selected or focused, toggle selected.*/
			if (btn->centerSwitchSelect == NULL && btn->centerSwitchFocus == NULL)
			{
				form::toggleSelectedBtn();
				break;
			}
			
			if (button::selectedBtn == btn)
			{
				if (btn->centerSwitchSelect != NULL)
				{
					btn->centerSwitchSelect(btn->name);
				}
			}
			else
			{
				if (btn->centerSwitchFocus != NULL)
				{
					btn->centerSwitchFocus(btn->name);
				}
				else
				{
					form::toggleSelectedBtn();
				}
			}
			
		break;
	
		case UP_SWITCH:
			/*If there's no function to run for click on focus...*/
			if (btn->upSwitchFocus == NULL)
			{
				/*Check to see if the button is selected, if it is not focus previous button.*/
				if (button::selectedBtn == NULL)
				{
					form::focusPrevBtn();
				}
				/*If it is selected, check the function pointer, then run it's click on selected function.*/
				else
				{
					if (btn->upSwitchSelect != NULL)
					{
						btn->upSwitchSelect(btn->name);
					}
				}

			}
			/*If there is a function to run for click on focus, run that function.*/
			else
			{
				btn->upSwitchFocus(btn->name);					
			}			
		break;
		
		case DOWN_SWITCH:
			/*If there's no function to run for click on focus...*/
			if (btn->downSwitchFocus == NULL)
			{
				/*Check to see if the button is selected, if it is not focus previous button.*/
				if (button::selectedBtn == NULL)
				{
					form::focusNextBtn();
				}
				/*If it is selected, check the function pointer, then run it's click on selected function.*/
				else
				{

					if (btn->downSwitchSelect != NULL)
					{
						btn->downSwitchSelect(btn->name);
					}
				}

			}
			/*If there is a function to run for click on focus, run that function.*/
			else
			{
				btn->downSwitchFocus(btn->name);					
			}
		break;
		
		case LEFT_SWITCH:

		break;
		
		case RIGHT_SWITCH:

		break;
		
		default:

		break;
	}	
	/*Clear all switch interrupts.*/
	ioClearIntFlag(&SW_PORT, SWITCHES_bm);
	
	/*Load the form with the changes.*/
	form::activeForm->load();
}

/********Button Functions*********/

/*
Constructor: button::button((char* name, uint16_t x, uint16_t y, char* text, const uint8_t font[], form* thisForm)
Params: name: name of the button, this is used to reference a specific button within the list.
		x: x position of the center button on the screen.
		y: y position of the center of the button on the screen.
		text: the text on the button (this determines the width of the button.
		font: the font of the text on the button, this determines the height of the button.
		form: which form to add the button to.
Returns: none
Description: Used to create a new button linked to a form (add new button during runtime).
*/
button::button(char* name, uint16_t x, uint16_t y, char* text, const uint8_t font[], form* thisForm)
{
	this->name = name;
	this->y = y;
	this->x = x;
	this->text = text;
	this->font = font;
	this->nextBtn = NULL;
	this->previousBtn = NULL;
	this->roundedCorners = true;
	this->cornerRadius = 8;
	this->show = true;
	
	/*Initialize function pointers.*/
	upSwitchSelect = NULL;
	downSwitchSelect = NULL;
	centerSwitchSelect = NULL;
	leftSwitchSelect = NULL;
	rightSwitchSelect = NULL;
	
	upSwitchFocus = NULL;
	downSwitchFocus = NULL;
	centerSwitchFocus = NULL;
	leftSwitchFocus = NULL;
	rightSwitchFocus = NULL;
	
	/*If this is the first button created for this form, make it firstBtn.*/
	if (thisForm->firstBtn == NULL)
	{
		thisForm->firstBtn = this;
	}
	/*Otherwise put it at the end of all the buttons.*/
	else
	{
		button* end = thisForm->firstBtn;
		button* prev = thisForm->firstBtn;
		while (end->nextBtn != NULL)
		{
			prev = end;
			end = end->nextBtn;
		}
		end->nextBtn = this;
		this->previousBtn = end;
	}
	
	numChars = strlen(text);
	
	/*Determine the width and height of the string.*/
	charWidth = font[0];
	charHeight = font[1];
	charPadding = font[2];
	stringWidth = (charWidth * numChars) - (charPadding * (numChars));
	
	/*Determine a starting and ending point for the string.*/
	x0 = x - (stringWidth / 2);
	y0 = y - (charHeight / 2); 	
	
	/*Determine size of rectangle.*/
	x0r = x0 - 20;
	y0r = y0 - 10;
	x1r = x0 + stringWidth + 20;
	y1r = y0 + charHeight + 10;	
}

/*
Constructor: button::button((char* name, uint16_t x, uint16_t y, char* text, const uint8_t font[])
Params: name: name of the button, this is used to reference a specific button within the list.
		x: x position of the center button on the screen.
		y: y position of the center of the button on the screen.
		text: the text on the button (this determines the width of the button.
		font: the font of the text on the button, this determines the height of the button.
Returns: none
Description: Used to create a new button not linked to a form (before runtime).
*/
button::button(char* name, char* text, uint16_t x, uint16_t y, const uint8_t* font)
{
	this->name = name;
	this->y = y;
	this->x = x;
	this->text = text;
	this->font = font;
	this->nextBtn = NULL;
	this->previousBtn = NULL;
	this->roundedCorners = true;
	this->cornerRadius = 8;
	this->show = true;
	
	/*Initialize function pointers.*/
	upSwitchSelect = NULL;
	downSwitchSelect = NULL;
	centerSwitchSelect = NULL;
	leftSwitchSelect = NULL;
	rightSwitchSelect = NULL;
	
	upSwitchFocus = NULL;
	downSwitchFocus = NULL;
	centerSwitchFocus = NULL;
	leftSwitchFocus = NULL;
	rightSwitchFocus = NULL;
	
	this->numChars = strlen(text);
	
	/*Determine the width and height of the string.*/
	charWidth = font[0];
	charHeight = font[1];
	charPadding = font[2];
	stringWidth = (charWidth * numChars) - (charPadding * (numChars));
	
	/*Determine a starting and ending point for the string.*/
	x0 = x - (stringWidth / 2);
	y0 = y - (charHeight / 2);
	
	/*Determine size of rectangle.*/
	x0r = x0 - 20;
	y0r = y0 - 10;
	x1r = x0 + stringWidth + 20;
	y1r = y0 + charHeight + 10;	
}

/*
Constructor: button::button()
Params:
Returns:
Description:
*/
button::button(){}

/*
Function: button::clearAll
Params: none
Returns: none
Description: clears the display, I would've liked for this to be part of the form class but there was 
			issues with inheritance of ST7789 class I couldn't figure out at the time.
*/
void button::clearAll()
{
	clearDisplay();
}

/*
Function: load
Params: none
Returns: none
Description: loads the button on the screen.
*/
void button::load()
{

	if (button::focusedBtn == this)
	{		
		/*If the button is selected.*/
		if (button::selectedBtn == this)
		{
			if (roundedCorners == false)
			{
				drawRectangle(x0r, y0r, x1r, y1r, BUTTON_SELECTED_COLOR);									//Highlight rectangle
				drawText(text, x0, y0, font, COLOR_BLACK, COLOR_WHITE);
			}
			else
			{
				drawRoundedRectangle(x0r, y0r, x1r, y1r, cornerRadius, BUTTON_SELECTED_COLOR, COLOR_BLACK);	//Highlight rectangle
				drawText(text, x0, y0, font, COLOR_BLACK, COLOR_WHITE);
			}
		}
		
		/*If the button is focused.*/
		else
		{
			if (roundedCorners == false)
			{
				drawBorderedRect(x0r, y0r, x1r, y1r, 6, BUTTON_SELECTED_COLOR, COLOR_BLACK);
				drawText(text,x0,y0,font, BUTTON_FOCUSED_COLOR, COLOR_BLACK);			
			}
			else
			{
				drawBorderedRoundedRect(x0r, y0r, x1r, y1r, cornerRadius, 6, BUTTON_SELECTED_COLOR, COLOR_BLACK, COLOR_BLACK);
				drawText(text,x0,y0,font, BUTTON_FOCUSED_COLOR, COLOR_BLACK);			
			}
		}															
	}
	
	/*If the button is neither focused nor selected.*/
	else
	{
		if (roundedCorners == false)
		{
			drawBorderedRect(x0r, y0r, x1r, y1r, 3, BUTTON_COLOR, COLOR_BLACK);			
			drawText(text, x0, y0, font, BUTTON_COLOR, COLOR_BLACK);						
		}
		else
		{
			drawBorderedRoundedRect(x0r, y0r, x1r, y1r, cornerRadius, 2, BUTTON_SELECTED_COLOR, COLOR_BLACK, COLOR_BLACK);
			drawText(text,x0,y0,font, BUTTON_FOCUSED_COLOR, COLOR_BLACK);			
		}
	}
	
}

/*Pointer the the button that is currently focused. One button will always be focused.*/
button* button::focusedBtn;

/*Pointer to the button that is currently selected. Either points to a button or NULL. If a button is selected it has to also be focused.*/
button* button::selectedBtn = NULL;


/*********Label Functions*********/

/*
Constructor: label(char* name, uint16_t x, uint16_t y, char text[], const uint8_t* font, form* thisForm)
Params: name: name of the label, used to reference a specific label in the list.
		x: x position of the center of the label.
		y: y position of the center of the label.
		text: the text that the label displays.
		font: pointer to the font array to be used for the text.
		form: which form to add the label to.
Returns: none
Description: this constructor is used when adding a new label during run time with "form.addNewLabel"
*/

 label::label(char* name, uint16_t x, uint16_t y, char text[], const uint8_t* font, form* thisForm)
{
	this->y = y;
	this->x = x;
	memset(this->text, 0, sizeof(this->text));
	memcpy(this->text, text, sizeof(text));
	this->name = name;
	this->font = font;
	nextLabel = NULL;
	
	/*Add the new label to the end of the list.*/
	if (thisForm->firstLabel == NULL)
	{
		thisForm->firstLabel = this;
	}
	else
	{
		label* end = thisForm->firstLabel;
		while (end->nextLabel != NULL)
		{
			end = end->nextLabel;
		}
		end->nextLabel = this;
	}
	
	/*Determine how many characters are in the text.*/
	numChars = strlen(text);
	
	/*Determine the width and height of the string.*/
	charWidth = font[0];
	charHeight = font[1];
	charPadding = font[2];
	stringWidth = (charWidth * numChars) - (charPadding * (numChars));
	
	/*Determine a starting and ending point for the string.*/
	x0 = x - (stringWidth / 2);
	y0 = y - (charHeight / 2);	
}

/*
Constructor: label(char* name, uint16_t x, uint16_t y, char text[], const uint8_t* font)
Params: name: name of the label, used to reference a specific label in the list.
		x: x position of the center of the label.
		y: y position of the center of the label.
		text: the text that the label displays.
		font: pointer to the font array to be used for the text.
Returns: none
Description: this constructor is used to statically create a new label.
*/
label::label(char* name, char text[], uint16_t x, uint16_t y, const uint8_t* font)
{
	this->y = y;
	this->x = x;
	memset(this->text, 0, sizeof(this->text));
	memcpy(this->text, text, sizeof(text));
	this->name = name;
	this->font = font;
	nextLabel = NULL;
	
	/*Determine how many characters are in the text.*/
	numChars = strlen(text);
	
	/*Determine the width and height of the string.*/
	charWidth = font[0];
	charHeight = font[1];
	charPadding = font[2];
	stringWidth = (charWidth * numChars) - (charPadding * (numChars));
	
	/*Determine a starting and ending point for the string.*/
	x0 = x - (stringWidth / 2);
	y0 = y - (charHeight / 2);	
}

label::label(){}

/*
Function: setText
Params: newText
Returns: none
Description: Changes the text of a label.
*/
void label::setText(char newText[])
{
	memset(this->text, 0, sizeof(this->text));
	memcpy(text, newText, sizeof(text));
}

/*
Function: load
Params: none
Returns: none
Description: displays a label on the screen.
*/
void label::load()
{
	drawText(text, x0, y0, font, TEXT_COLOR, COLOR_BLACK);
}


/**********Form Functions***********/

/*
Constructor: form
Params: none
Returns: none
Description: Creates a new instance of a form that buttons and labels can be added to.
*/
form::form()
{
	/*intc.h built in functions. Turns on interrupts for the switches.*/
	INTC_init_interrupts();
	INTC_register_interrupt(&switchInterruptHanlder, AVR32_GPIO_IRQ_2, 2);
	INTC_register_interrupt(&switchInterruptHanlder, AVR32_GPIO_IRQ_3, 2);
	Enable_global_interrupt();
	
	/*Make the first button and label pointers null.*/
	firstBtn = NULL;
	firstLabel = NULL;
}

/*Pointer to the form that is currently displayed on the screen.*/
form* form::activeForm = NULL;

/*
Function: addNewButton
Params: name: name of the button, this is used to reference a specific button within the list.
		x: x position of the center button on the screen.
		y: y position of the center of the button on the screen.
		text: the text on the button (this determines the width of the button.
		font: the font of the text on the button, this determines the height of the button.
Returns: none
Description: dynamically adds a new button the form invoking the function.
*/
void form::addNewBtn(char* name, char text[], uint16_t x, uint16_t y, const uint8_t* font)
{
	new button(name, x, y, text, font, this);
}

/*
Function: addNewButton
Params: btn: pointer to the button to add to the form.
Returns: none
Description: adds an already statically declared button to the form.
*/
void form::addNewBtn(button* btn)
{
	/*If this is the first button created for this form, make it firstBtn.*/
	if (this->firstBtn == NULL)
	{
		this->firstBtn = btn;
	}
	/*Otherwise put it at the end of all the buttons.*/
	else
	{
		button* end = firstBtn;
		button* prev = firstBtn;
		while (end->nextBtn != NULL)
		{
			prev = end;
			end = end->nextBtn;
		}
		end->nextBtn = btn;
		btn->previousBtn = end;
	}
}

/*
Function: setBtnFocus
Params: name: Name of the button to give focus to.
Returns: none
Description: focuses a button.
*/
void form::setBtnFocus(char* name)
{
	button* search = firstBtn;
	while (search->nextBtn != NULL)
	{
		if (search->name == name)
		{
			break;
		}
		else
		{
			search = search->nextBtn;
		}
	}
	button::focusedBtn = search;
}

/*
Function: addNewLabel
Params: name: name of the label, this is used to reference a specific label within the list.
		x: x position of the center label on the screen.
		y: y position of the center of the label on the screen.
		text: the text on the label.
		font: the font of the text on the label.
Returns: none
Description: dynamically adds a new label the form that is invoking the function.
*/
void form::addNewLabel(char* name, char text[], uint16_t x, uint16_t y, const uint8_t* font)
{
	new label(name, x, y, text, font, this);
}

/*
Function: addNewLabel
Params: label: pointer to the button to add to the form.
Returns: none
Description: adds an already statically declared label to the form.
*/
void form::addNewLabel(label* lbl)
{
	/*Add the new label to the end of the list.*/
	if (firstLabel == NULL)
	{
		firstLabel = lbl;
	}
	else
	{
		label* end = firstLabel;
		while (end->nextLabel != NULL)
		{
			end = end->nextLabel;
		}
		end->nextLabel = lbl;
	}
}

/*
Function: load
Params: none
Returns: none
Description: loads the form onto the screen.
*/
void form::load()
{
	if (form::activeForm != this)
	{
		clearAll();
		button::focusedBtn = this->firstBtn;
	}

	form::activeForm = this;
	
	if (firstBtn != NULL)
	{
		button* btn = firstBtn;
		while (btn->nextBtn != NULL)
		{
			if (btn->show == true)
			{
				btn->load();
				btn = btn->nextBtn;
			}
			else
			{
				btn = btn->nextBtn;
			}
		}
		btn->load();
	}
	
	if (firstLabel != NULL)
	{
		label* lbl = firstLabel;
		while (lbl->nextLabel != NULL)
		{
			lbl->load();
			lbl = lbl->nextLabel;
		}
		lbl->load();
	}
}

/*
Function: focusNextBtn
Params: none
Returns: none
Description: focuses the next button in the list on a form.
*/
void form::focusNextBtn()
{
	button* btn = form::activeForm->firstBtn;	
	while(form::activeForm->button::focusedBtn->name != btn->name)
	{
		btn = btn->nextBtn;
	}
	if (btn->nextBtn != NULL)
	{
		button::focusedBtn = btn->nextBtn;
	}
}

/*
Function: focusPrevBtn
Params: none
Returns: none
Description: focuses the previous button in the list on a form.
*/
void form::focusPrevBtn()
{
	button* btn = form::activeForm->firstBtn;	
	while(form::activeForm->button::focusedBtn->name != btn->name)
	{
		btn = btn->nextBtn;
	}
	if (btn->previousBtn != NULL)
	{
		button::focusedBtn = btn->previousBtn;
	}	
}

/*
Function: toggleSelectedBtn
Params: none
Returns: none
Description: if the focused button is currently selected, this un-selects it.
			if the currently focused button is not selected, this selects it.
*/
void form::toggleSelectedBtn()
{
	button* btn = form::activeForm->button::focusedBtn;
	if (button::selectedBtn == NULL)
	{
		button::selectedBtn = btn;
	}
	else
	{
		button::selectedBtn = NULL;
	}
}

/*
Function: pButton
Params: name: name of a button in the form
Returns: a pointer to the button of the name passed in the params.
Description: returns a pointer to a button in a form.
*/
button* form::pButton(char* name)
{
	/*Find the button by name in the list.*/
	button* btn = firstBtn;
	while(btn->name != name)
	{
		if (btn->nextBtn == NULL)
		{
			return firstBtn;
		}
		else
		{
			btn = btn->nextBtn;		
		}
	}	
	return btn;
}

/*
Function: pLabel
Params: name: name of a label
Returns: a pointer to a label by the name passed in the params
Description: returns a pointer to a label on the form.
*/
label* form::pLabel(char* name)
{
	label* lbl = this->firstLabel;
	while (lbl->name != (char*)name)
	{
		if (lbl->nextLabel == NULL)
		{
			return firstLabel;
		}
		else
		{
			lbl = lbl->nextLabel;
		}
	}
	return lbl;	
}

/*
Function: lblText
Params: name: name of the button to be modified.
		text: the new text for the button to display
Returns: none
Description: changes the text of a button on a form.
*/
void form::lblText(char* name, char* text)
{
	label* lbl = this->firstLabel;
	while (lbl->name != (char*)name)
	{
		lbl = lbl->nextLabel;
	}
	memset(lbl->text, 0, sizeof(lbl->text));
	memcpy(lbl->text, text, sizeof(lbl->text));
}

/*
Function: clear
Params: none
Returns: none 
Description: clears the screen.
*/
void form::clear()
{
	clearAll();
}