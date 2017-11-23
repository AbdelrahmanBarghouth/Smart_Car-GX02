/*
 * distanceTask.c
 *
 *  Created on: 9 November 2017
 *      Author: Abdelrahman Barghouth, Wendong Hong, and Lawal Hassan
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
#include "distanceTask.h"
#include "Sharp96x96.h"
#include "Board.h"
#include "Robot.h"
#include "buttons.h"
#include <stdio.h>

int distanceLeft[4];
int distanceRight[4];

void distanceTaskFxn (UArg arg0, UArg arg1){
    unsigned int    i;
    uint8_t         txBufferSwitch[1];
    uint8_t         txBufferSensor[3];
    uint8_t         rxBufferLeft[4];
    uint8_t         rxBufferRight[4];
    I2C_Handle      i2cRight;
    I2C_Handle      i2cLeft;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;

    GPIO_write(Robot_Right_Sensor_PE, 1);
    GPIO_write(Robot_Left_Sensor_PE, 1);
    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cRight = I2C_open(Board_I2C1, &i2cParams);
    i2cLeft = I2C_open(Board_I2C2, &i2cParams);
    if (i2cRight == NULL || i2cLeft == NULL) {
        System_abort("Error Initializing I2C\n");
    }
    else {
        System_printf("I2C Initialized!\n");
    }

    while (1) {
        txBufferSwitch[0] = 0x01;
        for(i = 0; i < 4; i++){

            //Talking to the switch
            i2cTransaction.slaveAddress = 0x70;
            i2cTransaction.writeBuf = txBufferSwitch;
            i2cTransaction.writeCount = 1;
            i2cTransaction.readBuf = NULL;
            i2cTransaction.readCount = 0;
            I2C_transfer(i2cRight, &i2cTransaction);
            I2C_transfer(i2cLeft, &i2cTransaction);

            txBufferSensor[0] = 0x00;
            txBufferSensor[1] = 0x18;
            txBufferSensor[2] = 0x01;
            i2cTransaction.slaveAddress = 0x29;
            i2cTransaction.writeBuf = txBufferSensor;
            i2cTransaction.writeCount = 3;
            I2C_transfer(i2cRight, &i2cTransaction);
            I2C_transfer(i2cLeft, &i2cTransaction);

            txBufferSensor[0] = 0x00;
            txBufferSensor[1] = 0x62;
            i2cTransaction.slaveAddress = 0x29;
            i2cTransaction.writeBuf = txBufferSensor;
            i2cTransaction.writeCount = 2;
            i2cTransaction.readBuf = rxBufferRight;
            i2cTransaction.readCount = 1;
            I2C_transfer(i2cRight, &i2cTransaction);
            i2cTransaction.readBuf = rxBufferLeft;
            i2cTransaction.readCount = 1;
            I2C_transfer(i2cLeft, &i2cTransaction);

            distanceRight[i] = rxBufferRight[0];
            distanceLeft[i] = rxBufferLeft[0];
            txBufferSwitch[0] *=2;
        }
        System_flush();
        Task_sleep((UInt)arg0);
    }
    /* Deinitialized I2C */
    I2C_close(i2cRight);
    I2C_close(i2cLeft);
    System_printf("I2C closed!\n");

    System_flush();
}



