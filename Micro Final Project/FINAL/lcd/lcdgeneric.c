#include "lcdgeneric.h"

// Function to disable JTAG (can be called multiple times safely)
void disable_jtag_for_portc(void)
{
	// Disable JTAG interface to use PC2, PC3, PC4, PC5 as normal I/O
	// This must be done within 4 clock cycles
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);  // Write twice within 4 clock cycles
}

// Functions for 3rd LCD (Serial Display)
void Lcd_Serial_Port(char a)
{
	if(a & 1)
	pinChange(D4_SERIAL,1);
	else
	pinChange(D4_SERIAL,0);
	
	if(a & 2)
	pinChange(D5_SERIAL,1);
	else
	pinChange(D5_SERIAL,0);
	
	if(a & 4)
	pinChange(D6_SERIAL,1);
	else
	pinChange(D6_SERIAL,0);
	
	if(a & 8)
	pinChange(D7_SERIAL,1);
	else
	pinChange(D7_SERIAL,0);
}

void Lcd_Serial_Cmd(char a)
{
	pinChange(RS_SERIAL,0);             // => RS = 0
	Lcd_Serial_Port(a);
	pinChange(EN_SERIAL,1);            // => E = 1
	_delay_ms(1);
	pinChange(EN_SERIAL,0);             // => E = 0
	_delay_ms(1);
}

void Lcd_Serial_Clear()
{
	Lcd_Serial_Cmd(0);
	Lcd_Serial_Cmd(1);
}

void Lcd_Serial_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a == 0)
	{
		temp = 0x80 + b;
		z = temp>>4;
		y = (0x80+b) & 0x0F;
		Lcd_Serial_Cmd(z);
		Lcd_Serial_Cmd(y);
	}
	else if(a == 1)
	{
		temp = 0xC0 + b;
		z = temp>>4;
		y = (0xC0+b) & 0x0F;
		Lcd_Serial_Cmd(z);
		Lcd_Serial_Cmd(y);
	}
}

void Lcd_Serial_Init()
{
	// Disable JTAG first to use PORTC pins
	disable_jtag_for_portc();
	_delay_ms(1);
	
	// Set PORTC pins as output for the 3rd LCD
	DDRC |= (1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7);
	
	// Initialize all pins to LOW
	PORTC &= ~((1<<PC2) | (1<<PC3) | (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7));
	
	Lcd_Serial_Port(0x00);
	_delay_ms(20);
	///////////// Reset process from datasheet /////////
	Lcd_Serial_Cmd(0x03);
	_delay_ms(5);
	Lcd_Serial_Cmd(0x03);
	_delay_ms(11);
	Lcd_Serial_Cmd(0x03);
	/////////////////////////////////////////////////////
	Lcd_Serial_Cmd(0x02);
	Lcd_Serial_Cmd(0x02);
	Lcd_Serial_Cmd(0x08);
	Lcd_Serial_Cmd(0x00);
	Lcd_Serial_Cmd(0x0C);
	Lcd_Serial_Cmd(0x00);
	Lcd_Serial_Cmd(0x06);
	Lcd_Serial_Clear();
}

void Lcd_Serial_Write_Char(char a)
{
	char temp,y;
	temp = a&0x0F;
	y = a&0xF0;
	pinChange(RS_SERIAL,1);             // => RS = 1
	Lcd_Serial_Port(y>>4);             //Data transfer
	pinChange(EN_SERIAL,1);
	_delay_ms(1);
	pinChange(EN_SERIAL,0);
	_delay_ms(1);
	Lcd_Serial_Port(temp);
	pinChange(EN_SERIAL,1);
	_delay_ms(1);
	pinChange(EN_SERIAL,0);
	_delay_ms(1);
}

void Lcd_Serial_Write_String(const char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	Lcd_Serial_Write_Char(a[i]);
}

// Original functions with added support for 3rd LCD
void Lcd_Init(int_fast8_t lcdType)
{
	switch(lcdType)
	{
		case LCDKEYPAD:
		DDRD = DDRD | 0b11111100;
		Lcd4_Init();
		break;
		case LCDNOTICE:
		//////////////////////////////////////////////////////////////////////////
		//lcd with i2c
		lcd_init(LCD_ON_DISPLAY);
		lcd_backlight(0);
		_delay_ms(500);
		lcd_backlight(1);
		_delay_ms(500);
		lcd_clrscr();
		//////////////////////////////////////////////////////////////////////////
		break;
		case LCDSERIAL:
		Lcd_Serial_Init();
		break;
		default:
		Error_Error("Lcd_Init: LcdType Notsupported");
	}
}

void Lcd_ClearScreen(int_fast8_t lcdType)
{
	switch(lcdType)
	{
		case LCDKEYPAD:
		Lcd4_Clear();
		break;
		case LCDNOTICE:
		lcd_clrscr();
		break;
		case LCDSERIAL:
		Lcd_Serial_Clear();
		break;
		default:
		Error_Error("Lcd_ClearScreen: LcdType Not supported");
	}
}

void Lcd_Prints(int_fast8_t lcdType ,const char * s)
{
	switch(lcdType)
	{
		case LCDKEYPAD:
		Lcd4_Write_String(s);
		break;
		case LCDNOTICE:
		lcd_puts(s);
		break;
		case LCDSERIAL:
		Lcd_Serial_Write_String(s);
		break;
		default:
		Error_Error("Lcd_Prints: LcdType Not supported");
	}
}

void Lcd_Position(int_fast8_t lcdType,int x,int y)
{
	switch(lcdType)
	{
		case LCDKEYPAD:
		Lcd4_Set_Cursor(x,y);
		break;
		case LCDNOTICE:
		lcd_gotoxy(y,x);
		break;
		case LCDSERIAL:
		Lcd_Serial_Set_Cursor(x,y);
		break;
		default:
		Error_Error("Lcd_Position: LcdType Not supported");
	}
}

void Lcd_Printc(int_fast8_t lcdType ,const char c)
{
	char s[] = {c,0};
	Lcd_Prints(lcdType , s);
}

void Lcd_PrintLine(int_fast8_t lcdType , int row , char * str)
{
	Lcd_Position(lcdType , row , 0);
	int i =0;
	while (str[i])
	{
		Lcd_Printc(lcdType , str[i++]);
	}
	while(i<16)
	Lcd_Printc(lcdType , ' '),i++;
}