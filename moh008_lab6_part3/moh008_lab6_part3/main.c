/* Partner Name & email : Stephanie scabr006@ucr.edu
 * Lab Section: 021
 * Assignment: Lab6 Exercise 3
 * Created: 2019-10-25 오후 2:24:34
 * Author : Lincoln
 *
 * I acknoledge all content contained herein, excluding template or example code, is my own original work.
 */ 
#include <avr/io.h>
#include <avr/wdt.h>
#include "scheduler.h"
#include "timer.h"
#include "lcd.h"
#include "incDecSM.h"
#define buttons (~PINA & 0x07)
const unsigned char wakeDisplay = (1 << PA0); // Which pin wakesup LCD

static task task1, task2;
task *tasks[] = {&task1, &task2};
const unsigned short numTasks = sizeof (tasks)/ sizeof(task*);

unsigned char wakecount = 0;
enum Buttons_States {inactive, press, hold, released};
int Button_tick(int state)
{
	switch (state)	//state transition
	{
		case inactive:
		PORTB = 0x00;	//turn off the backlight
		wakecount = 0;
		if(buttons == wakeDisplay)
		state = press;
		else
		state = inactive;
		break;
		
		case press:
		if(buttons == wakeDisplay)
		{
			state = hold;
		}
		break;
		
		case hold:
		if(buttons == wakeDisplay)
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
		else if (buttons == wakeDisplay)
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
};

void WDT_init(void)
{
	__disable_interrupt();		//Disable interrupts (critical timing section)
	__watchdog_reset();			//Reset watchdog (hint: use avr/wdt.h)
								//Set up WDT interrupt
								//Start watchdog with Xs prescalar
	__enable_interrupt();		//Re-enable global interrupts
}

ISR(WDT_vect)
{
								//Insert any code you need here
}
	
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned long int SMTick1_period = 1;
	unsigned long int SMTick2_period = 2;
	
	//Task 1 Counter task
	task1.state = -1; //task initial state
	task1.period = SMTick1_period;
	task1.active = 0x01;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &Button_tick;
	
	//Task 1 Counter task
	task2.state = -1; //task initial state
	task2.period = SMTick2_period;
	task2.active = 0x01;
	task2.elapsedTime = SMTick1_period;
	task2.TickFct = &IncDec_tick;
	
	LCD_init();
	TimerSet(50);
	TimerOn();
	
	unsigned short i;
	
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