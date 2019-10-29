/* Partner Name & email : Stephanie scabr006@ucr.edu
 * Lab Section: 021
 * Assignment: Lab6 Exercise 2
 * Created: 2019-10-25 오후 2:24:34
 * Author : Lincoln
 *
 * I acknoledge all content contained herein, excluding template or example code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "scheduler.h"
#include "timer.h"
#include "lcd.h"
#include "incDecSM.h"
#define SWITCH_PRESSED !(PINA & (1<<PINA0))

static task task1;
task *tasks[] = {&task1};
const unsigned short numTasks = sizeof (tasks)/ sizeof(task*);

void initPCInt()
{
	SREG = 0x01;		//Enable global interrupts
	PCICR = 0x01;		//Enable Pin Change Interrupt 0 (Pins 7 ... 0)
	PCMSK0 = 0x01;		//Enable Pin Change Interrupt on PCINT0 which is PA0
}

//-------------------------------------------------------------------------------------

unsigned char wakecount = 0;
ISR(PCINT0_vect)	//NEED TO TURN ON FOR 3Seconds
{
	if(SWITCH_PRESSED)
	{
		while(wakecount != 10)
		{
			PORTB = 0x01;
			tasks[0]->active = 1;
			wakecount++;
		}
		wakecount = 0;
	}
	else
	{
		PORTB = 0x00;
		tasks[0]->active = 0;
	}
}
 //---------------------------------------------------------------------------------------
/*
enum Buttons_States {inactive, press, hold, released};
int Button_tick(int state)
{
	switch (state)	//state transition
	{
		case inactive:
		PORTB = 0x00;	//turn off the backlight
		wakecount = 0;
		if(SWITCH_PRESSED)
		state = press;
		else
		state = inactive;
		break;
		
		case press:
		if(SWITCH_PRESSED)
		{
			state = hold;
		}
		break;
		
		case hold:
		if(SWITCH_PRESSED)
		{	
			state = hold;
		}
		else
		{
			state = released;	
		}
		break;
		
		case released:
		if(wakecount == 60)
		state = inactive;
		if (SWITCH_PRESSED)
		{
			state = press;
			wakecount = 0;
		}
		else
		{
			state = released;
		}
		break;
		
		default:
		state = inactive;
		LCD_ClearScreen();
		break;
	}
	
	switch (state) //state action
	{
		case inactive:
		tasks[1]->active = 0;
		break;
		
		case press:
		PORTB = 0x01;	//turn on the backlight
		tasks[1]->active = 1;
		wakecount++;
		break;
		
		case hold:
		PORTB = 0x01;
		wakecount++;
		break;
		
		case released:
		PORTB = 0x01;
		wakecount++;
		break;
	}
	return state;
}
*/	
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned char i = 0;
	
	unsigned long int SMTick1_period = 2;
	
	//Task 1 Counter task
	task1.state = -1; //task initial state
	task1.period = SMTick1_period;
	task1.active = 0x01;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &IncDec_tick;
	
	initPCInt();
	LCD_init();
	TimerSet(50);
	TimerOn();
	
	sei();
	
	while (1)
	{
		for ( i = 0; i < numTasks; i++)
		{
			if (tasks[i]->active == 0x01)
			{
				if(tasks[i]->elapsedTime == tasks[i]->period)
				{
					tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
					tasks[i]->elapsedTime = 0;
				}
				tasks[i]->elapsedTime += 1;
			}
		}
		while (!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}