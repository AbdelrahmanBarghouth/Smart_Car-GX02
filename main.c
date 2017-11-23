/*
 * main.c
 *
 *  Created on: 9 November 2017
 *      Author: Abdelrahman Barghouth, Wendong Hong, and Lawal Hassan
 */

/*
 *  ======== main.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/PWM.h>

/* Board Header files */
#include "Sharp96x96.h"
#include "Board.h"
#include "distanceTask.h"
#include "buttons.h"
#include "Robot.h"
#include <stdio.h>

/*
 *  ======== global system state variables ========
 */
uint8_t state = 1;
int encoderLeft = 0;
int encoderRight = 0;
void RHMotorChannelAFallingFxn(unsigned int index);
void LHMotorChannelAFallingFxn(unsigned int index);
int taskNumber = 0;
/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initPWM();
    Board_initI2C();

    GPIO_setCallback(Board_BUTTON0, gpioButtonFxn0);
    GPIO_enableInt(Board_BUTTON0);

    GPIO_setCallback(Board_BUTTON1, gpioButtonFxn1);
    GPIO_enableInt(Board_BUTTON1);

    // Switch on the LEDs
    //
//    GPIO_write(Board_LED0, Board_LED_ON);   // LH LED
//    GPIO_write(Board_LED1, Board_LED_ON);   // RH LED, Red
//    GPIO_write(Board_LED2, Board_LED_ON);   // RH LED, Green
//    GPIO_write(Board_LED3, Board_LED_ON);   // RH LED, Blue

    GPIO_setCallback(MSP_EXP432P401R_P6_1, RHMotorChannelAFallingFxn);//Pin6.1 is set to trigger an interrupt when there is a falling edge in the input
    GPIO_enableInt(MSP_EXP432P401R_P6_1);
    GPIO_setCallback(MSP_EXP432P401R_P4_5, LHMotorChannelAFallingFxn);
    GPIO_enableInt(MSP_EXP432P401R_P4_5);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}

void RHMotorChannelAFallingFxn(unsigned int index)
{
    if (GPIO_read(MSP_EXP432P401R_P4_2))
    {
        encoderRight--;
    } else {
        encoderRight++;
    }
}

void LHMotorChannelAFallingFxn(unsigned int index)
{
    if (GPIO_read(MSP_EXP432P401R_P5_4))
    {
        encoderLeft++;
    } else {
        encoderLeft--;
    }
}
