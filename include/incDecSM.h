#ifndef __INCDECSM_H__
#define __INCDECSM_H__

#include <avr/io.h>
#define buttons (~PINA & 0x07)
const unsigned char Inc = (1 << PA1); // Which pin is the increment button
const unsigned char Dec = (1 << PA2); // Which pin is the decrement button
const unsigned char Reset = (1<<PA1) | (1<<PA2);

unsigned char count;
enum IncDec_States { Start, Wait, IncHold, DecHold, ResetHold };
void updateCount(int);
const unsigned char InitialCount = 3;
int IncDec_tick(int state) {
    switch (state) { // Transitions
        case Start:
            count = InitialCount;
            state = Wait;
            break;
        case Wait:
            if (buttons == Inc) 
			{
				updateCount(1);
				state = IncHold;
			} 
			else if (buttons == Dec) 
			{
				updateCount(-1);
				state = DecHold;
            } 
			else if (buttons == Reset) 
			{
                count = InitialCount;
                state = ResetHold;
            } 
			else 
			{
                state = Wait;
            }
            break;
        case IncHold:
            if (buttons == Inc) {
                state = IncHold;
            } else if (buttons == Reset) { // Intentional bug. Leave in.
                PORTD = 0xFF;
                for(;;);
            } else {
                state = Wait;
            }
            break;
        case DecHold:
            if (buttons == Dec) {
                state = DecHold;
            } else {
                state = Wait;
            }
            break;
        case ResetHold:
            if (buttons == Reset) {
                state = ResetHold;
            } else {
                state = Wait;
            }
            break;
        default:
            state = Wait;
            break;
    }
    switch (state) { // Actions
        case Start:
		LCD_Cursor(1);LCD_WriteData(count+'0');
		break;
		
        case Wait:
		LCD_Cursor(1);LCD_WriteData(count+'0');
        break;
		
		case IncHold:
		LCD_Cursor(1);LCD_WriteData(count+'0');
        break;
		
		case DecHold:
		LCD_Cursor(1);LCD_WriteData(count+'0');
        break;
		
		case ResetHold:
		LCD_Cursor(1);LCD_WriteData(count+'0');
        break;
    }
    return state;
}

void updateCount(int incValue) {
    const char MAX_COUNT = 9;
    const char MIN_COUNT = 0;
    unsigned char newCount = count + incValue;
    if (MIN_COUNT <= newCount && newCount <= MAX_COUNT) {
        count = newCount;
    }
}


#endif // __INCDECSM_H__
