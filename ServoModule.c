/*
Purpose: Functions for using the servo motor
Author:  Josh Osborne
Date:    April 13, 2015
Version: 00
*/

#include "derivative.h"      /* derivative-specific definitions */
#include "utils.h"
#include "TimerModule.h"
#include <stdlib.h>
#include "ServoModule.h"

// Flag for using the sweep                                                                                                                                                                                                                                                       0
int SWEEP = 0;
// Value of final on time
long DESTINATION;
// State of output waveform
volatile int STATE;
// Time till move complete
unsigned long TIME;
// On time for output waveform
long long PULSE_WIDTH;

/*************************servo_sweep()****************************************
Purpose: Move the servo motor with a sweep function
Input:   int angle - angle to move to
         unsigned ling msecs - time to take to get there
******************************************************************************/
int servo_sweep(int angle, unsigned long msecs)
{

  if(angle > MAXANGLE || angle < MINANGLE)
    return 0;
  DisableInterrupts;
  SWEEP = 1;
  TIME = msecs;
  DESTINATION = get_pulse(angle);
  EnableInterrupts;
  
  while(PULSE_WIDTH != DESTINATION);
  
  SWEEP = 0;
  return 0;
}

/*************************servo_postion()**************************************
Purpose: Move the servo motor to a specified position
Input:   int angle - angle to move to
******************************************************************************/
int servo_position(int angle) 
{
  if(angle > MAXANGLE || angle < MINANGLE)
    return 0;

  DisableInterrupts;
  PULSE_WIDTH = get_pulse(angle);
  EnableInterrupts;
  return 0;
}

void SERVOinit(void) 
{
    // Set channel 2 to output capture
  MAKE_CHNL_OC( 2 );
  
  SET_OC_ACTION(2, OC_GO_LO);
  STATE = LOW_;      
  
    // preset TC0 for first OC event
  TIMER_CHNL( 2 ) = TCNT + (unsigned int)PULSE_WIDTH;
  
  ENABLE_CHNL_INTERRUPT( 2 );
    //Clear any pending interrupts on channel 0
  TFLG1 = (TFLG1_C2F_MASK);
  
  // Zero the stepper motor on start
  PULSE_WIDTH = 1590;
}
/**************************Servo Handler()************************************
Purpose: Interrupt Handler for the servo motor
 - outputs waveform with specified pulse width

******************************************************************************/
interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 2 ) ) void TC2_OC_handler( void )
{

  static long pos;
  static int count;
  signed long temp;
  static long steps;
  
  //TFLG1 = TIMER_CHNL_FLAG( 0 );
  TFLG1 = TFLG1_C2F_MASK;
  
  if(SWEEP == 1) 
  {
    
    steps = TIME/20; 
    pos = PULSE_WIDTH;        
    (signed long)temp = (DESTINATION - pos);
    asm("nop");
    
    PULSE_WIDTH += temp / steps;
    
    count ++;
    TIME -= 20;
  }
  else if(SWEEP == 0)
    count = 0;
    
  switch(STATE) 
  {
     case LOW_:
     SET_OC_ACTION(2, OC_GO_HI);    // Change the state of the waveform
     STATE = HIGH;                  // Set the State indicator
     TC2 += (20000 - (unsigned int)PULSE_WIDTH);  // Remainder of period is low time
     break;
     
     case HIGH:
     SET_OC_ACTION(2, OC_GO_LO);    // Change the state of the waveform
     STATE = LOW_;                  // Set the State indicator
     TC2 += (unsigned int)PULSE_WIDTH;            // Set High time of period
     break;
  } 

      
} // end of TC0_OC_handler()


/*************************get_pulse()******************************************
Purpose: Get the pulse width for a specified angle
Input:   int angle - angle to move to
Output:  unsigned long temp - pulse width
******************************************************************************/
unsigned long get_pulse(int angle) 
{
  unsigned long temp;
  
  (unsigned long)temp = (GAIN * angle);
  temp += OFFSET;
  
  return temp;
}