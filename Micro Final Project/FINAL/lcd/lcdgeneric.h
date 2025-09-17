#ifndef LCDGENERIC_H_ADDED
#define LCDGENERIC_H_ADDED

#include "../all.h"

#define LCDKEYPAD 0 // LcdType
#define LCDNOTICE 1 // LcdType
#define LCDSERIAL 2 // LcdType - New 3rd LCD for patient serial display

// Original LCD pins
#define D4 eS_PORTD5
#define D5 eS_PORTD4
#define D6 eS_PORTD3
#define D7 eS_PORTD2
#define RS eS_PORTD7
#define EN eS_PORTD6

// New 3rd LCD pins (using available pins)
#define D4_SERIAL eS_PORTC2
#define D5_SERIAL eS_PORTC3
#define D6_SERIAL eS_PORTC4
#define D7_SERIAL eS_PORTC5
#define RS_SERIAL eS_PORTC6
#define EN_SERIAL eS_PORTC7

#include "lcd_4bit.h"

#include "lcd.h"
extern void lcd_backlight(char on);   //not in lcd.h

void Lcd_Init(int_fast8_t lcdType);
void Lcd_ClearScreen(int_fast8_t lcdType);
void Lcd_Prints(int_fast8_t lcdType ,const char * s);
void Lcd_Position(int_fast8_t lcdType,int x,int y);
void Lcd_Printc(int_fast8_t lcdType ,const char c);
void Lcd_PrintLine(int_fast8_t lcdType , int row , char * str);

// New functions for 3rd LCD
void Lcd_Serial_Init();
void Lcd_Serial_Clear();
void Lcd_Serial_Set_Cursor(char a, char b);
void Lcd_Serial_Write_Char(char a);
void Lcd_Serial_Write_String(const char *a);

#endif