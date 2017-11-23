/*
 * motorTask.c
 *
 *  Created on: 20 November 2017
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
#include <ti/drivers/PWM.h>

/* Board Header files */
#include "distanceTask.h"
#include "Sharp96x96.h"
#include "Board.h"
#include "Robot.h"
#include "buttons.h"
#include <stdio.h>
#include <math.h>

#define pi 3.1415926
#define diameter 6 //This is the diameter of the wheels


// Define pwm variables for the 2 motors
PWM_Handle pwmRightForward;
PWM_Handle pwmRightBackward;
PWM_Handle pwmLeftForward;
PWM_Handle pwmLeftBackward;
PWM_Params params;

uint16_t   pwmPeriod = 3000;      // Period and duty in microseconds
uint16_t   dutyLeft = 0;
uint16_t   dutyRight = 0;
uint16_t   dutyTurn = 350;
uint16_t   dutyZero = 0;

extern int distanceLeft[4];
extern int distanceRight[4];
extern int encoderLeft;
extern int encoderRight;
extern UInt32 time;
extern int taskNumber;
UInt32 timeStart;

// Choosing PID parameters for each motor
// The parameters are tweaked using trial and error approach
// We tuned the numbers till reaching the optimal constants for each motor
float KpLeft = 0.80;
float TiLeft = 0.0127;
float TdLeft = 0.007;

float KpRight = 2.18;
float TiRight = 0.0222;
float TdRight = 0.0105;

// Initialising error variables with zeros
float pidLeft = 0;
float pidRight = 0;
float errorLeftP = 0;
float errorLeftI = 0;
float errorLeftD = 0;
float errorLeftPre = 0;
float errorRightP = 0;
float errorRightI = 0;
float errorRightD = 0;
float errorRightPre = 0;

// Choosing Target Speeds for each task
float DeadReckonX = 200.0; // Distance 2 meters for x-axis between start and end points
float DeadReckonY = 500.0; // Distance 5 meters for y-axis between start and end points
float angle;
float distance;
float deviationPerSecond;
float DeadReckonLeftSpeed;
float DeadReckonRightSpeed;


//Our average speed chosen for this task is 10 cm/s
float wallFollSpdR = 10.0;
float wallFollSpdL = 10.0;
int wallFront;
int wallLeft;
int wallRight;
int wallDistance = 8;
bool isThereWallFront;
float constantSpeed = 8.0;

// Initialise the motor speeds and encoder readings
float speedLeft = 0;
float speedRight = 0;
int encoderLeftStart;
int encoderRightStart;

// Identifying functions used for the 3 tasks
void PIDControl(float, float);
void moveConstantSpeed();
void deadReckoning();
void wallFollowing();
void stop();
void turnLeft();
void turnRight();

void motorTaskFxn (UArg arg0, UArg arg1){
    GPIO_write(Robot_MotorEN, 1);
    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;
    pwmRightForward = PWM_open(Board_PWM0, &params);//right motor input 1
    pwmRightBackward = PWM_open(Board_PWM1, &params);//right motor input 2
    pwmLeftForward = PWM_open(Board_PWM2, &params);//left motor input 1
    pwmLeftBackward = PWM_open(Board_PWM3, &params);//left motor input 2

    PWM_start(pwmRightForward);
    PWM_start(pwmRightBackward);
    PWM_start(pwmLeftForward);
    PWM_start(pwmLeftBackward);

    while(1)
    {
        printf("task is %d\n",taskNumber);
        if(taskNumber == 1)
        {
            if(time<=2000){
                takeYourSpeed();
            }else{
                moveConstantSpeed();
            }
        }
        else if(taskNumber == 3)
        {
            if(time<=2000){
                takeYourSpeed();
            }else{
                wallFollowing();
            }
        }
        else if(taskNumber == 4)
        {
            if(time <= 2000){
                takeYourSpeed();
            }

            else if(time<=58000){
                deadReckoning();
            }else{
                stop();
            }
        }
    }
}
void takeYourSpeed (){
    // Start with zero speed
    PWM_setDuty(pwmRightForward,dutyZero);
    PWM_setDuty(pwmRightBackward,dutyRight);
    PWM_setDuty(pwmLeftForward,dutyZero);
    PWM_setDuty(pwmLeftBackward,dutyLeft);

    // Call PID to update speed with same speed for both motors to move straight
    PIDControl(10.0, 10.0);

    // Read the new speed
    timeStart = time;
    encoderLeftStart = encoderLeft;
    encoderRightStart = encoderRight;
    Task_sleep(10);
    speedLeft = (encoderLeft-encoderLeftStart)*pi*diameter*1000/(750*(time-timeStart));
    speedRight = (encoderRight-encoderRightStart)*pi*diameter*1000/(750*(time-timeStart));
}

void moveConstantSpeed (){
    // Start with zero speed
    PWM_setDuty(pwmRightForward,dutyZero);
    PWM_setDuty(pwmRightBackward,dutyRight);
    PWM_setDuty(pwmLeftForward,dutyZero);
    PWM_setDuty(pwmLeftBackward,dutyLeft);

    // Call PID to update speed with same speed for both motors to move straight
    PIDControl(constantSpeed, constantSpeed);

    // Read the new speed
    timeStart = time;
    encoderLeftStart = encoderLeft;
    encoderRightStart = encoderRight;
    Task_sleep(10);
    speedLeft = (encoderLeft-encoderLeftStart)*pi*diameter*1000/(750*(time-timeStart));
    speedRight = (encoderRight-encoderRightStart)*pi*diameter*1000/(750*(time-timeStart));
}

void wallFollowing(){

    // Start with zero speed
    PWM_setDuty(pwmRightForward,dutyZero);
    PWM_setDuty(pwmRightBackward,dutyRight);
    PWM_setDuty(pwmLeftForward,dutyZero);
    PWM_setDuty(pwmLeftBackward,dutyLeft);

    // Get Readings from all 4 front sensors
    wallFront = distanceLeft [3] + distanceRight[3] + distanceLeft[2] + distanceRight[2];

    // Get Readings of each side sensors, We gave the sensor with angle more weight
    // as we see that it is more sensitive to how much we need to rotate
    wallRight = 2 * distanceRight[2] + distanceRight[1] + distanceRight[0];
    wallLeft  = 2 * distanceLeft [2] + distanceLeft [1] + distanceLeft [0];

    // Instead of making 4 if conditions we can now check if their average is < 10
    isThereWallFront = wallFront/4 < wallDistance ? 1 : 0;

    // If there is a wall in front of us we turn other wise we try to keep stick to wall
    // We assume that the other side is free or the wall will be at the same distance with both sensors
    wallFollSpdR += isThereWallFront == 1 ? ( 0.001 * ( wallLeft - wallRight)) : 0.001 * (distanceRight[0] - distanceRight[1] + distanceLeft[1] - distanceLeft[0]);
    wallFollSpdL += isThereWallFront == 1 ? ( 0.001 * ( wallRight - wallLeft)) : 0.001 * (distanceRight[1] - distanceRight[0] + distanceLeft[0] - distanceLeft[1]);

    if(wallFollSpdR > 18 || wallFollSpdR > 18 )
    {
        wallFollSpdR -= 2;
        wallFollSpdL -= 2;
    }
    // Call PID to update speed with differential speed to obtain a curve orientation
    PIDControl(wallFollSpdL, wallFollSpdR);

    // Read the new speed
    timeStart = time;
    encoderLeftStart = encoderLeft;
    encoderRightStart = encoderRight;
    Task_sleep(10);
    speedLeft = (encoderLeft-encoderLeftStart)*pi*diameter*1000/(750*(time-timeStart));
    speedRight = (encoderRight-encoderRightStart)*pi*diameter*1000/(750*(time-timeStart));
}

void deadReckoning(){
    angle = atan2f(DeadReckonX,DeadReckonY);
    distance = sqrt(pow(DeadReckonX,2)+pow(DeadReckonY,2));
    deviationPerSecond = angle/(distance/10);
    DeadReckonLeftSpeed  = 10 * cos(deviationPerSecond) - 10 * sin(deviationPerSecond);
    DeadReckonRightSpeed = 10 * cos(deviationPerSecond) + 10 * sin(deviationPerSecond);

    // Start with zero speed
    PWM_setDuty(pwmRightForward,dutyZero);
    PWM_setDuty(pwmRightBackward,dutyRight);
    PWM_setDuty(pwmLeftForward,dutyZero);
    PWM_setDuty(pwmLeftBackward,dutyLeft);

    // Call PID to update speed with differential speed to obtain a curve orientation
    PIDControl(DeadReckonLeftSpeed, DeadReckonRightSpeed);

    // Read the new speed
    timeStart = time;
    encoderLeftStart = encoderLeft;
    encoderRightStart = encoderRight;
    Task_sleep(10);
    speedLeft = (encoderLeft-encoderLeftStart)*pi*diameter*1000/(750*(time-timeStart));
    speedRight = (encoderRight-encoderRightStart)*pi*diameter*1000/(750*(time-timeStart));
}


void stop (){
    PWM_setDuty(pwmRightForward,dutyZero);
    PWM_setDuty(pwmRightBackward,dutyZero);
    PWM_setDuty(pwmLeftForward,dutyZero);
    PWM_setDuty(pwmLeftBackward,dutyZero);
}

void turnLeft (){
    PWM_setDuty(pwmRightForward,dutyZero);
    PWM_setDuty(pwmRightBackward,dutyRight);
    PWM_setDuty(pwmLeftForward,dutyZero);
    PWM_setDuty(pwmLeftBackward,dutyZero);
    Task_sleep(10);
}

void turnRight (){
    PWM_setDuty(pwmRightForward,dutyZero);
    PWM_setDuty(pwmRightBackward,dutyZero);
    PWM_setDuty(pwmLeftForward,dutyZero);
    PWM_setDuty(pwmLeftBackward,dutyLeft);
}

void PIDControl (float targetLeft, float targetRight){
    // Calculating errors in speed with respect to target speed for each motor
    printf("l = %f  r = %f\n",targetLeft,targetRight);

    errorLeftP = targetLeft - speedLeft;
    errorRightP = targetRight - speedRight;

    errorLeftD = errorLeftP - errorLeftPre;
    errorLeftPre = errorLeftD;

    errorRightD = errorRightP - errorRightPre;
    errorRightPre = errorRightD;

    // Updating the motor speed with the PID system
    pidLeft=KpLeft*errorLeftP+TiLeft*errorLeftI+TdLeft*errorLeftD;
    dutyLeft=dutyLeft+pidLeft;

    pidRight=KpRight*errorRightP+TiRight*errorRightI+TdRight*errorRightD;
    dutyRight=dutyRight+pidRight;

    // Integrating Error which is summation as we are discrete
    errorLeftI=errorLeftI+errorLeftP;
    errorRightI=errorRightI+errorRightP;
}

