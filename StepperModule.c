/******************************************************************************
Purpose: Functions to use the steppper motor
Author:  Josh Osborne
Date:    Arpil 9, 2015
Version: 00
******************************************************************************/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "StepperModule.h"
#include "TimerModule.h"
#include "utils.h"

// GLOBALS
int init_complete;
// Step type
int stepper_step;
// Stepper Speed
int stepper_speed;
// Left Side tick count
int stepper_left = 0;
// Right Side tick count
int stepper_right = 0;
// Position in step pattern
unsigned int step_num;
// Step type full or half
volatile int step_type;
// Tick count for stepper position in rotation
volatile int stepper_count;
// Direction stepper is turning
volatile int stepper_rotation;
// Flag which means stepper is being initialized
volatile int stepper_init_flag;
// Direction stepper is turning
volatile int stepper_direction;
// Pattern that moves the stepper motor
int pattern[] = {0b10000000, 0b10100000, 0b00100000, 0b01100000, 0b01000000, 0b01010000, 0b00010000, 0b10010000};

/********************STEPPER_RELATIVE()****************************************
Purpose: Move the motor relative to the current position
Input:	 int degrees - number of degrees to move
******************************************************************************/
int STEPPER_RELATIVE(int degrees) 
{
  int temp;
  int ticks;
  
  // Convert the degrees to number of ticks
  ticks = degrees * 2;
  // Compute the new stepper motor position
  temp = stepper_count - ticks;
  
  // Make sure the stepper motor does not pass the barriers
  if(temp > 370)
    temp = 370;
  if(temp < 10)
    temp = 10;
  
  // Determine the direction that the stepper motor will turn
  if(temp < stepper_count) {
    stepper_rotation = COUNTERCLOCKWISE;
    stepper_direction = (-1 * step_type);
  }
  else if(temp > stepper_count) {
    stepper_rotation = CLOCKWISE;
    stepper_direction = (1 * step_type);
  } 
  else if(temp == stepper_count)
    return 0;
  
  // Move stepper motor to new spot
  ENABLE_CHNL_INTERRUPT( 7 );
    while(stepper_count != temp && stepper_init_flag != HIGH);
  stepper_init_flag == LOW_;
  DISABLE_CHNL_INTERRUPT( 7 );
  
  return 0;
}

/***************************STEPPER_MOVE()*************************************
Purpose: Move the stepper motor to a specified angle
Input:   int angle - angle to move the stepper motor to
******************************************************************************/
int STEPPER_MOVE(int angle) 
{
  int position;
  signed int temp_count;
  int temp;
  
  // Dont allow the motor to move to an angle outside the range
  if(angle > 90 || angle < -90)
    return 0;
  // Get the tick number for the new angle
  (signed)temp_count = get_stepper_ticks(angle);
  // Get current position of motor
  position = get_stepper_position();
  // determine what direction the motor needs to move
  (int)temp = angle - position;
  
  // Set the direction that the motor will be turning
  if(temp > 0) {
    stepper_rotation = COUNTERCLOCKWISE;
    stepper_direction = -1 * step_type;
  }
  else if(temp < 0) {
    stepper_rotation = CLOCKWISE;
    stepper_direction = 1 * step_type;
  } 
  else if(temp == 0) // Motor not turning
    return 0;

  // Move the motor
  ENABLE_CHNL_INTERRUPT( 7 );
  while(stepper_count != temp_count && stepper_init_flag != HIGH);
  stepper_init_flag == LOW_;
  DISABLE_CHNL_INTERRUPT( 7 );
  
  return 0;
}

/****************TC7_OC_handler()**********************************************
Purpose: Process events on interrupt
BUG - when switchs are hit will move two ticks
******************************************************************************/
interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 7 ) ) void TC7_OC_handler( void ) 
{
  // Clear the interrupt mask for pending interrupts
  TFLG1 = TFLG1_C7F_MASK;
  
  // Counts the number of positions during homing
  if(stepper_init_flag == HIGH) 
  {
    if(stepper_rotation == COUNTERCLOCKWISE)
      stepper_right++;
    
    // Change direction when limit switch is hit
    if((SWITCH_PORT & PTP_PTP2_MASK) && (stepper_rotation == COUNTERCLOCKWISE)) 
    {
      stepper_direction *= (-1  * step_type);
      stepper_rotation = CLOCKWISE;
      if(stepper_init_flag == HIGH) 
      {
        stepper_init_flag = LOW_;
      }
        
    }

    if((SWITCH_PORT & PTP_PTP3_MASK) && (stepper_rotation == CLOCKWISE)) 
    {
      stepper_direction *= (-1  * step_type);
      stepper_rotation = COUNTERCLOCKWISE;
    }
  } 
  else if(stepper_init_flag == LOW_ && init_complete == HIGH)
  {
    // Should not hit switches therefore home motor to re-initalize 
    if(SWITCH_PORT & PTP_PTP2_MASK) 
    {
      stepper_init_flag = HIGH;
      DISABLE_CHNL_INTERRUPT( 7 );
    }
    if(SWITCH_PORT & PTP_PTP3_MASK)
    {
      stepper_init_flag = HIGH;
      DISABLE_CHNL_INTERRUPT( 7 );
    }
  }
  // Count the tick placement for precise location of stepper motor  
  if(stepper_rotation == CLOCKWISE)
    stepper_count++;
  else if(stepper_rotation == COUNTERCLOCKWISE)
    stepper_count--;
  
  // Select next output pattern
  stepper_step = pattern[step_num];
  
  // Mod the counter
  step_num = (step_num + stepper_direction) % STEPPER_MOTOR_COUNT_MASK;
  
  // Set the output pattern
  FORCE_BITS(STEPPER_MOTOR_PORT, STEPPER_MOTOR_MASK, stepper_step);
  
  // Set time for next interrupt
  TIMER_CHNL( 7 ) = TCNT + stepper_speed;
}

/**************************STEPPERhome()***************************************
Purpose: Home the stepper motor
############# DONE ONLY ONCE IN MAIN ##############
******************************************************************************/
int STEPPERhome(void) 
{
  // Set parametes
  init_complete = LOW_;
  stepper_init_flag = HIGH;
  stepper_direction = 1;
  stepper_rotation = CLOCKWISE;
  stepper_right = 0;
  ENABLE_CHNL_INTERRUPT( 7 );
  
  // Move the motor to home position
  for(;;) 
  {  
    if(stepper_init_flag == LOW_) 
    {
      stepper_count = 0;
      while(stepper_count != (stepper_right/2));
      DISABLE_CHNL_INTERRUPT( 7 );
      init_complete = HIGH;
      return 0;
    }
  }
}

/***************************STEPPERinit()**************************************
Purpose: Initalize the stepper motor
******************************************************************************/
void STEPPERinit(void)
{

  // Clear the interrupt mask for pending interrupts
  TFLG1 = TFLG1_C7F_MASK;
  
  // Preset channel for first interrupt
  TIMER_CHNL( 7 )= TCNT + 10000;
  
  // Make the channel output compare
  MAKE_CHNL_OC( 7 );
  
  // Set the channel to off so the pin can be used for GPIO
  SET_OC_ACTION(7, OC_OFF);
  
  // Enable the channels interrupt
  ENABLE_CHNL_INTERRUPT( 7 );

  // Initalize the port
  //STEPPER_MOTOR_PORT = STEPPER_MOTOR_PORT_INIT;
  CLR_BITS(STEPPER_MOTOR_PORT, STEPPER_MOTOR_PORT_INIT);
  //STEPPER_MOTOR_DDR = STEPPER_MOTOR_DDR_INIT;
  SET_BITS(STEPPER_MOTOR_DDR, STEPPER_MOTOR_DDR_INIT);
  
  // Initalize the DDR
  //SWITCH_DDR = SWITCH_PORT_INIT;
  CLR_BITS(SWITCH_PORT, SWITCH_PORT_INIT);
  //SWITCH_DDR = LOW(SWITCH_DDR_INIT);
  SET_BITS(SWITCH_DDR, SWITCH_DDR_INIT);
  
  // Set the parameters
  stepper_direction = 1; 
  stepper_speed = 5000;
  step_num = 0;
  step_type = HALF;
  stepper_step = pattern[step_num];
  stepper_rotation = CLOCKWISE;
  stepper_count = 0;
}

/***********get_stepper_position()*********************************************
Purpose:  Get the stepper motors current position
******************************************************************************/
int get_stepper_position(void) 
{
  signed long pos;
  
  (signed long)pos = ((signed long)stepper_count) / -2;
  pos += 92;
  return (int)pos;
}

/****************get_stepper_state()*******************************************
Purpose: Get the stepper motors current position
Output:  1 stepper motor is moving
         0 stepper motor is stopped
******************************************************************************/
int get_stepper_state(void) 
{
  if(TIE & 0x07)
   return 1;
  else
    return 0;
}

/*****************get_stepper_ticks()******************************************
Purpose: Convert an angle to its tick number
Output:  Number of ticks
******************************************************************************/
int get_stepper_ticks(int angle) 
{
  signed long i;
  
  (signed long)i = (angle - 92) * -2;
  asm("nop");
  return (int)i; 
}

/*****************change_step()***********************************************
Purpose: Change the step type
******************************************************************************/
void change_step(int step) 
{
  step_type = step;
}

/*****************change_speed()**********************************************
Purpose: Change the stepper motor speed
******************************************************************************/
void change_speed(int speed) 
{
  stepper_speed = speed;
}