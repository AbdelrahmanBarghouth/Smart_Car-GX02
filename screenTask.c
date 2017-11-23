/*
 * screenTask.c
 *
 *  Created on: 9 Oct 2016
 *      Author: shailes
 *      Edited by: Abdelrahman Barghouth, Wendong Hong, and Lawal Hassan
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

/* Board Header files */
#include "Sharp96x96.h"
#include "Board.h"
#include "buttons.h"
#include "distanceTask.h"
#include <stdio.h>

extern int distanceLeft[4];
extern int distanceRight[4];
extern int encoderLeft;
extern int encoderRight;
extern float speedLeft;
extern float speedRight;
extern uint16_t dutyLeft;
extern uint16_t dutyRight;

extern int taskNumber;


/*
 *  screenTaskFxn
 */
void screenTaskFxn (UArg arg0, UArg arg1)
{
    Graphics_Context g_sContext;

    char test1_string[20];
    char test2_string[20];

    const uint8_t LHS =  10;
    const uint8_t RHS = 210;
    uint8_t y;
    uint8_t x;

    Sharp96x96_LCDInit();

    Graphics_initContext(&g_sContext, &g_sharp96x96LCD);
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontCm20b);
    Graphics_clearDisplay(&g_sContext);
    Graphics_flushBuffer(&g_sContext);

    while(taskNumber == 0)
    {
        Graphics_clearBuffer(&g_sContext);
        x = 100;
        y = 100;

        sprintf(test1_string,"Please select the task ") ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, x, y, GRAPHICS_TRANSPARENT_TEXT);

        y += 80;
        sprintf(test1_string,"   Constant Speed      Other Tasks") ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);

        Graphics_flushBuffer(&g_sContext);
    }

    while(taskNumber == 2)
    {
        Graphics_clearBuffer(&g_sContext);
        x = 100;
        y = 100;

        sprintf(test1_string,"Please select the task ") ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, x, y, GRAPHICS_TRANSPARENT_TEXT);

        y += 80;
        sprintf(test1_string,"   Wall Following      Dead Reckoning") ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);

        Graphics_flushBuffer(&g_sContext);
    }

    while (1) {
        Graphics_clearBuffer(&g_sContext);
        y = 0;

        sprintf(test1_string,"Sensor Outputs: ") ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 20;

        sprintf(test1_string,"leftSensor1 = %4d", (distanceLeft[0])) ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        sprintf(test2_string,"RightSensor1 = %4d", distanceRight[3]) ;
        Graphics_drawString(&g_sContext, test2_string, GRAPHICS_AUTO_STRING_LENGTH, RHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 20;

        sprintf(test1_string,"leftSensor2 = %4d", distanceLeft[3]) ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        sprintf(test2_string,"RightSensor2 = %4d", distanceRight[0]) ;
        Graphics_drawString(&g_sContext, test2_string, GRAPHICS_AUTO_STRING_LENGTH, RHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 20;

        sprintf(test1_string,"leftSensor3 = %4d", distanceLeft[2]) ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        sprintf(test2_string,"RightSensor3 = %4d", distanceRight[1]) ;
        Graphics_drawString(&g_sContext, test2_string, GRAPHICS_AUTO_STRING_LENGTH, RHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 20;

        sprintf(test1_string,"leftSensor4 = %4d", distanceLeft[1]) ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        sprintf(test2_string,"RightSensor4 = %4d", distanceRight[2]) ;
        Graphics_drawString(&g_sContext, test2_string, GRAPHICS_AUTO_STRING_LENGTH, RHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 20;

        sprintf(test1_string,"leftMotor = %4d", encoderLeft) ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        sprintf(test2_string,"RightMotor = %4d", encoderRight) ;
        Graphics_drawString(&g_sContext, test2_string, GRAPHICS_AUTO_STRING_LENGTH, RHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 20;

        sprintf(test1_string,"dutyLeft = %4d", dutyLeft) ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        sprintf(test2_string,"dutyRight = %4d", dutyRight) ;
        Graphics_drawString(&g_sContext, test2_string, GRAPHICS_AUTO_STRING_LENGTH, RHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 20;

        sprintf(test1_string,"speedLeft = %4.2f", speedLeft) ;
        Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS, y, GRAPHICS_TRANSPARENT_TEXT);
        sprintf(test2_string,"speedRight = %4.2f", speedRight) ;
        Graphics_drawString(&g_sContext, test2_string, GRAPHICS_AUTO_STRING_LENGTH, RHS, y, GRAPHICS_TRANSPARENT_TEXT);
        y += 40;

        switch (taskNumber)
        {
        case 1:
            sprintf(test1_string,"Chosen Task is Constant Speed") ;
            Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS+20, y, GRAPHICS_TRANSPARENT_TEXT);
            y += 20;
            break;
        case 3:
            sprintf(test1_string,"Chosen Task is Wall Following") ;
            Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS+20, y, GRAPHICS_TRANSPARENT_TEXT);
            y += 20;
            break;
        case 4:
            sprintf(test1_string,"Chosen Task is Dead Reckoning") ;
            Graphics_drawString(&g_sContext, test1_string, GRAPHICS_AUTO_STRING_LENGTH, LHS+20, y, GRAPHICS_TRANSPARENT_TEXT);
            y += 20;
            break;
        }


        Graphics_flushBuffer(&g_sContext);
        Task_sleep((UInt)arg0);
    }
}

