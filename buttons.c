/*
 * buttons.c
 *
 *  Created on: 9 Oct 2016
 *      Author: shailes
 *      Edited by: Abdelrahman Barghouth, Wendong Hong, and Lawal Hassan
 *
 */
#include "stdint.h"

extern uint8_t state;
extern int taskNumber;

/*
 *  Callback function for the GPIO interrupt on Board_BUTTON0 and Board_BUTTON1.
 */
void gpioButtonFxn0(unsigned int index)
{
	state = 1;
	taskNumber++;
}

void gpioButtonFxn1(unsigned int index)
{
	state = 0;
	taskNumber+=2;
}

