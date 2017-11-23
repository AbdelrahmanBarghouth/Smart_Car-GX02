/*
 * clockTask.c
 *
 *  Created on: 9 November 2017
 *      Author: Abdelrahman Barghouth, Wendong Hong, and Lawal Hassan
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>

/* Example/Board Header files */
#include "Board.h"

Void clk0Fxn(UArg arg0);

UInt32 time;
Clock_Struct clk0Struct;

void clockTaskFxn (UArg arg0, UArg arg1){
    /* Construct BIOS Objects */
    Clock_Params clkParams;

    /* Call board init functions */
    Board_initGeneral();

    Clock_Params_init(&clkParams);
    clkParams.period = 5000/Clock_tickPeriod;
    clkParams.startFlag = TRUE;

    /* Construct a periodic Clock Instance */
    Clock_construct(&clk0Struct, (Clock_FuncPtr)clk0Fxn,
                    5000/Clock_tickPeriod, &clkParams);

    clkParams.period = 0;
    clkParams.startFlag = FALSE;

    BIOS_start();    /* does not return */
}

/*
 *  ======== clk0Fxn =======
 */
Void clk0Fxn(UArg arg0)
{
    time = Clock_getTicks();
}



