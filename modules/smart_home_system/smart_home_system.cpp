//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "display.h"
#include "matrix_keypad.h"
#include "smart_home_system.h"

#include "siren.h"
#include "user_interface.h"
#include "fire_alarm.h"
#include "pc_serial_com.h"
#include "event_log.h"
#include "motor.h"
#include "gate.h"

//=====[Declaration of private defines]========================================
extern DigitalIn motorSensor;
static motorDirection_t previousMotorState = STOPPED;
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================
void smartHomeSystemInit()
{
    userInterfaceInit();
    fireAlarmInit();
    pcSerialComInit();
    motorControlInit();
    gateInit();
    motorSensor.mode(PullUp); 
    displayInit(DISPLAY_TYPE_LCD_HD44780, DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER);
    displayClear();
    
    displayCharPositionWrite (0,0);
    displayStringWrite( "Temperature:" );

    displayCharPositionWrite (0,1);
    displayStringWrite( "Gas:" );
    
    displayCharPositionWrite (0,2);
    displayStringWrite( "Alarm:" );
}

void smartHomeSystemUpdate()
{
    userInterfaceUpdate();
    fireAlarmUpdate();    
    pcSerialComUpdate();
    eventLogUpdate();
    motorControlUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
     char key = matrixKeypadUpdate();

     if (motorSensor == 0){
         motorDirectionWrite(STOPPED);
         pcSerialComStringWrite("Motor stopped by sensor\r\n");
     }
//use the matrix keypad to change the directon of motor
    if (key != '\0'){
        switch(key){
//when 1 is pressed
            case '1':
            motorDirectionWrite(DIRECTION_1);
            pcSerialComStringWrite("Motor drection 1");
         break;
//when 2 is pressed
        case '2':
            motorDirectionWrite(DIRECTION_2);
            pcSerialComStringWrite("Motor drection 2");
         break;
//when 0 is pressed
         case '0':
            motorDirectionWrite(STOPPED);
            pcSerialComStringWrite("Motor stopped");
         break;
        }
    }
    motorControlUpdate();
//read and update the current motor state.
static motorDirection_t lastMotorState = STOPPED;
motorDirection_t currentState = motorDirectionRead();

if (currentState != lastMotorState){
    switch (currentState){
        //for each motor state, show it on the pc output
        case STOPPED:
        displayCharPositionWrite (0,3);
        displayStringWrite("Motor stopped\r\n");
        break;

        case DIRECTION_1:
        pcSerialComStringWrite("State: Direction 1\r\n");
        break;

        case DIRECTION_2: 
        pcSerialComStringWrite("State: Direction 2\r\n");
    }
    lastMotorState = currentState;
}
motorDirection_t currentMotorState = motorDirectionRead();
if (currentMotorState != previousMotorState){
    //display current motor state on LCD
    displayCharPositionWrite(0,3);
    if (currentMotorState == STOPPED){
        displayStringWrite("Motor stopped");
    }
      if (currentMotorState == DIRECTION_1){
        displayStringWrite("Direction 1     ");
    }
      if (currentMotorState == DIRECTION_2){
        displayStringWrite("Direction 2      ");
    }
    //update the LCD and motorstate
    previousMotorState = currentMotorState;
}
}

//=====[Implementations of private functions]==================================
