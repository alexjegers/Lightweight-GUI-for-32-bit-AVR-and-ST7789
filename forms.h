/*
 * forms.h
 *
 * Created: 11/30/2022 11:57:11 AM
 *  Author: AJ992
 */ 


#ifndef FORMS_H_
#define FORMS_H_

#define BUTTON_FOCUSED_COLOR			COLOR_WHITE
#define BUTTON_SELECTED_COLOR			COLOR_WHITE
#define BUTTON_COLOR					COLOR_GRAY
#define TEXT_COLOR						COLOR_GRAY

#define CENTER_SWITCH					SW1_bm
#define UP_SWITCH						SW2_bm
#define RIGHT_SWITCH					SW3_bm 
#define LEFT_SWITCH						SW4_bm
#define DOWN_SWITCH						SW5_bm
#define SWITCHES_bm						(SW1_bm | SW2_bm | SW3_bm | SW4_bm | SW5_bm)
	
	class form;
	class button;
	class label;
	
	class label : protected ST7789
	{
		public:
		/*Identifiers*/
		char* name;
		label* nextLabel;

		
		label(char* name, uint16_t x, uint16_t y, char text[], const uint8_t* font, form* thisForm);
		label(char* name, char text[], uint16_t x, uint16_t y, const uint8_t* font);
		label();
		void load();
		void setText(char* newText);
		char text[10];					//Text to display in the button.
		
		private:
		const uint8_t* font;		//Pointer to font array.
		uint8_t y;					//Position of the vertical center of the button.
		uint8_t x;					//Position of the horizontal center of the button.
		uint8_t numChars;			//Number of characters in the text.
		uint8_t charWidth;			//Width of one character of the font being used.
		uint8_t charHeight;			//Height of one character of the font being used.
		uint8_t charPadding;		//Padding of the font being used.
		uint16_t stringWidth;		//Width of the whole string.
		uint16_t x0;				//Starting point of the string.
		uint16_t y0;				//Starting point of the string.
	};

	class button : protected ST7789
	{
		public:
		/*Identifiers*/
		char* name;
		button* nextBtn;
		button* previousBtn;
		
		
		button(char* name, uint16_t x, uint16_t y, char* text, const uint8_t* font, form* thisForm);
		button(char* name, char* text, uint16_t x, uint16_t y, const uint8_t* font);
		button();
		
		void load();													//Write the contents of the button to the LCD driver.
		void clearAll();												//Makes ST7789 function "clearDisplay" accessible to class::form, probably a better way to do this.
		static button* focusedBtn;
		static button* selectedBtn;

		
		void (*upSwitchSelect)(char* btnName);
		void (*downSwitchSelect)(char* btnName);
		void (*centerSwitchSelect)(char* btnName);
		void (*leftSwitchSelect)(char* btnName);
		void (*rightSwitchSelect)(char* btnName);
		
		void (*upSwitchFocus)(char* btnName);
		void (*downSwitchFocus)(char* btnName);
		void (*centerSwitchFocus)(char* btnName);
		void (*leftSwitchFocus)(char* btnName);
		void (*rightSwitchFocus)(char* btnName);
		
		bool roundedCorners;		//True for rounded corners, false for sharp corners
		uint8_t cornerRadius;		//Radius of rounded corners.		
		bool show;
		
		private:
		uint8_t y;					//Position of the vertical center of the button.
		uint8_t x;					//Position of the horizontal center of the button.
		char* text;					//Text to display in the button.
		uint8_t numChars;			//Number of characters in the text.
		uint8_t charWidth;			//Width of one character of the font being used.
		uint8_t charHeight;			//Height of one character of the font being used.
		uint8_t charPadding;		//Padding of the font being used.
		uint16_t stringWidth;		//Width of the whole string.
		uint16_t x0;				//Starting point of the string.
		uint16_t y0;				//Starting point of the string.
		uint16_t x0r;				//Starting point of rectangle.
		uint16_t y0r;				//Starting point of rectangle.
		uint16_t x1r;				//Ending point of rectangle.
		uint16_t y1r;				//Ending point of rectangle.
		const uint8_t* font;		//Pointer to font array.

	};

	class form : private button, private label
	{
		public:
		form();
		void load();						//Call the load function off all the elements in the form.
		void addNewBtn(char* name, char* text, uint16_t x, uint16_t y, const uint8_t* font);
		void addNewBtn(button* btn);
		void setBtnFocus(char* name);
		void addNewLabel(char* name, char* text, uint16_t x, uint16_t y, const uint8_t* font);
		void addNewLabel(label* lbl);
		void clear();
		
		void lblText(char* name, char* text);
		button* pButton(char* name);
		label* pLabel(char* name);
		
		static void switchInterruptHanlder();	
		static form* activeForm;
		button* firstBtn;				
		label* firstLabel;
		
		static void toggleSelectedBtn();
		
		private:

		static void focusNextBtn();
		static void focusPrevBtn();

	};
	

#endif /* FORMS_H_ */