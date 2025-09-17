/*
 * GccApplication1.c
 *
 * Created: 7/4/2021 4:14:07 PM
 * Author : mahdi
 */ 

#include "all.h"

struct Queue
{
	struct Patient queue[10];
	int maxsize;
	int front_num;
	int rear_num;
	int toq;
};

struct Queue q;

void Queue_Init(struct Queue *qptr)
{
	//toq=0;
	qptr->front_num = 0;
	qptr->rear_num = -1;
}

bool empty(struct Queue *qptr)
{
	if(qptr->front_num > qptr->rear_num)
		return true;
	else
		return false;
}

int size(struct Queue *qptr)
{
	return qptr->rear_num - qptr->front_num + 1 ;
}

void enqueue(struct Queue *qptr, struct Patient x)
{
	if(empty(&q))
	{
		
		Lcd_PrintLine(LCDNOTICE , 0 , "Patients are Wai");
		Lcd_PrintLine(LCDNOTICE , 1 , "ting.use button");
		
	}
	qptr->queue[++(qptr->rear_num)] = x;
}

struct Patient* dequeue(struct Queue *qptr)
{
	struct Patient* value;
	if(empty(qptr))
	{
		return NULL;
	}
	
	value = &qptr->queue[qptr->front_num++];

	return value;
}

struct Patient* front(struct Queue *qptr)
{
	if(!empty(qptr))
	return &qptr->queue[qptr->front_num];
	else
	return NULL;
}
struct Patient* rear(struct Queue *qptr)
{
	if(!empty(qptr))
	return &qptr->queue[qptr->rear_num];
	else
	return NULL;

}

int currentPatientSerial(struct Queue *qptr)
{
	return front(qptr)->serial;
}

int nextPatientSerial(struct Queue *qptr)
{
	return qptr->queue[qptr->front_num + 1].serial;
}

bool doctorSeeing = 0;

// Function to disable JTAG to use PORTC pins
void disable_jtag(void)
{
	// Disable JTAG interface to use PC2, PC3, PC4, PC5 as normal I/O
	// This must be done within 4 clock cycles
	MCUCSR |= (1 << JTD);
	MCUCSR |= (1 << JTD);  // Write twice within 4 clock cycles
}

// Function to update the 3rd LCD (Serial Display)
void updateSerialDisplay()
{
	// Clear the serial LCD
	Lcd_ClearScreen(LCDSERIAL);
	
	// Set position to first line, first column
	Lcd_Position(LCDSERIAL, 0, 0);
	
	if(empty(&q))
	{
		// Display "Serial:" when no patients
		Lcd_Prints(LCDSERIAL, "Serial:");
		
		// Second line - display "None"
		Lcd_Position(LCDSERIAL, 1, 0);
		Lcd_Prints(LCDSERIAL, "None");
	}
	else
	{
		// Display current patient serial
		struct Patient* now = front(&q);
		char serialStr[17];
		sprintf(serialStr, "Serial: %d", now->serial);
		Lcd_Prints(LCDSERIAL, serialStr);
		
		// Second line - display patient name
		Lcd_Position(LCDSERIAL, 1, 0);
		Lcd_Prints(LCDSERIAL, now->name);
	}
}

int main(void)
{
	// FIRST THING: Disable JTAG to use PORTC pins
	disable_jtag();
	
	// Small delay after JTAG disable
	_delay_ms(10);
	
	Queue_Init(&q);
	doctorSeeing = 0;
	
	//HC05_Init_Data_Mode();
	//HC05_Init_ATCommand_Mode();
	
	//HC05_SendString("hello there");
	
	// Initialize all LCDs
	Lcd_Init(LCDKEYPAD);
	Lcd_Init(LCDNOTICE);
	Lcd_Init(LCDSERIAL);  // Initialize the 3rd LCD
	
	// Add a small delay after LCD initialization
	_delay_ms(100);
	
	//Lcd_Position(LCDNOTICE , 0,0);
	//Lcd_Prints(LCDNOTICE , "abcdefghijklmnopqrstuvwxyz");
	//Lcd_Position(LCDNOTICE , 0,0);
	//Lcd_Prints(LCDNOTICE , "abcdefghijklmnopqrstuvwxyz");
	//Lcd_Prints(LCDNOTICE , "abcdefghijklmnopqrstuvwxyz");
	
	States_GotoState(IDLE);
	//States_GotoState(GENERATE_SERIAL);
	//States_GotoState(ENTERING_BP);
	
	Keypad_Init();
	Button_Init();
	
	Lcd_PrintLine(LCDNOTICE , 0,"No Patient in ");
	Lcd_PrintLine(LCDNOTICE , 1,"the queue");
	
	// Test the 3rd LCD with initial display
	Lcd_ClearScreen(LCDSERIAL);
	Lcd_Position(LCDSERIAL, 0, 0);
	Lcd_Prints(LCDSERIAL, "JTAG Disabled");
	Lcd_Position(LCDSERIAL, 1, 0);
	Lcd_Prints(LCDSERIAL, "LCD Working");
	
	// Wait a moment to see the test message
	_delay_ms(2000);
	
	// Initialize the 3rd LCD with proper display
	updateSerialDisplay();
	
	while (1)
	{
		if(Keypad_KeyPressed())
		{
			_delay_ms(100);
			if(Keypad_KeyPressed())
			{
				int_fast8_t x = Keypad_GetKey();
				KeyProcessor_ProcessKey(x);
				_delay_ms(100);
				Keypad_Init();
				
			}
		}
		States_Refresh();
		if(Button_ButtonPressed())
		{
			
			//Buzzer_Sound();
			Error_ErrorNum("dc",doctorSeeing);
			if(doctorSeeing)
			{
				dequeue(&q);// ager ta falailam
				if(empty(&q))
				{
					Lcd_PrintLine(LCDNOTICE , 0,"No Patient in ");
					Lcd_PrintLine(LCDNOTICE , 1,"the queue");
					doctorSeeing = 0;
				}
				else 
				{
					struct Patient* now =  front(&q);
					char s[17];
					sprintf(s,"%2d-%13s",now->serial , now->name);
					Lcd_PrintLine(LCDNOTICE , 0,s);
					sprintf(s,"%6sF%3sBPM%2sY",now->temperature,now->bp,now->age);
					Lcd_PrintLine(LCDNOTICE , 1,s);
					
				}
				
			}
			else
			{
				if(empty(&q))
				{
					Lcd_PrintLine(LCDNOTICE , 0,"No Patient in ");
					Lcd_PrintLine(LCDNOTICE , 1,"the queue");
				}
				else 
				{
					doctorSeeing = 1;
					
					struct Patient* now =  front(&q);
					char s[17];
					sprintf(s,"%2d-%13s",now->serial , now->name);
					Lcd_PrintLine(LCDNOTICE , 0,s);
					sprintf(s,"%6sF%3sBPM%2sY",now->temperature,now->bp,now->age);
					Lcd_PrintLine(LCDNOTICE , 1,s);
				}
			}
			
			// Update the 3rd LCD whenever button is pressed
			updateSerialDisplay();
			
			_delay_ms(500);
		}
		
	}
}