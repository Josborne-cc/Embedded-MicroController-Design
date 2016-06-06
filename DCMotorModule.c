/*
Purpose: Functions for using the DC motor
Author:  Josh Osborne
Date:    April 15, 2015
Version: 00
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "DCMotorModule.h"
#include "TimerModule.h"
#include "utils.h"


// Overflow counters
unsigned int OVER_FLOW_A;
unsigned int OVER_FLOW_B;

// Encoder states
volatile int ZERO_STATE_A;
volatile int ZERO_STATE_B;

// Encoder periods
volatile signed long PERIOD_A;
volatile signed long PERIOD_B;

// Speed measurements
volatile signed long SPEED_CM_A;
volatile signed long SPEED_CM_B;

// Input set points
volatile signed long SETPOINT_A;
volatile signed long SETPOINT_B;

// Encoder Error
volatile signed long SPEED_ERROR_A;
volatile signed long SPEED_ERROR_B;

// Pulse width drive value
volatile signed long DRIVE_VALUE_A;
volatile signed long DRIVE_VALUE_B;

// Encoder Error integral
volatile signed long SPEED_ERROR_INTEGRAL_A;
volatile signed long SPEED_ERROR_INTEGRAL_B;

/*****************DCMOTORAinit()***********************************************
Purpose: Initalize one DC motor
******************************************************************************/
void DCMOTORAinit(void) 
{
  // Set up the motor port
  CLR_BITS(DC_PORT, (DC_A1_MASK | DC_A2_MASK));
  SET_BITS(DC_DDR, (DC_A1_MASK | DC_A2_MASK));
  
  // Set up the enable port
  CLR_BITS(DC_E_PORT, PTP_PTP4_MASK);
  SET_BITS(DC_E_DDR, DC_A_E_MASK);
  
  // Use 8 bit
  PWMCTL = BIT_8;
  // Divide prescaler by 32
  SET_BITS(PWMPRCLK, DIV_32);
  
  // Use clock A
  CLR_BITS(PWMCLK, DC_A_E_MASK);
  
  // Set 1 KHz period
  DC_A_PER = PERIOD_1K;
  
  // Start with motor off
  DC_A_DTY = START;
  
  // Start counter at zero
  DC_A_CNT = START; 
  
  // Select center aligned
  SET_BITS(PWMCAE, DC_A_E_MASK);
  
  // Set the polarity
  SET_BITS(PWMPOL, DC_A_E_MASK);
  
  // Set inital direction at forward
  FORCE_BITS(DC_PORT, MOTOR_A_MASK, (DC_A1_MASK | DC_A2_MASK));
  
  // Enable the PWM channel
  ENABLE_PWM_CHNL(4);
}

/*****************DCMOTORBinit()***********************************************
Purpose: Initalize on DC motor
******************************************************************************/
void DCMOTORBinit(void) 
{
  // Set up the motor port
  CLR_BITS(DC_PORT, (DC_B1_MASK | DC_B2_MASK));
  SET_BITS(DC_DDR, (DC_B1_MASK | DC_B2_MASK));
  
  // Set up the enable port
  CLR_BITS(DC_E_PORT, PTP_PTP5_MASK);
  SET_BITS(DC_E_DDR, DC_B_E_MASK);
  
  // Use 8 bit
  PWMCTL = BIT_8;
  // Divide prescaler by 32
  SET_BITS(PWMPRCLK, DIV_32);
  
  // Use clock A
  CLR_BITS(PWMCLK, DC_B_E_MASK);
  
  // Set 1 KHz period
  DC_B_PER = PERIOD_1K;
  
  // Start with motor off
  DC_B_DTY = START;
  
  // Start counter at zero
  DC_B_CNT = START; 
  
  // Select center aligned
  SET_BITS(PWMCAE, DC_B_E_MASK);
  
  // Set the polarity
  SET_BITS(PWMPOL, DC_B_E_MASK);
  
  // Set inital direction at forward
  FORCE_BITS(DC_PORT, MOTOR_B_MASK, (DC_B1_MASK | DC_B2_MASK));
  
  // Enable the PWM channel
  ENABLE_PWM_CHNL(5);
}

/*****************ENCODERinit()************************************************
Purpose: Initalize the optical encoders
******************************************************************************/
void ENCODERinit(void)
{

     // Make channel input capture
     MAKE_CHNL_IC( 0 );
     MAKE_CHNL_IC( 1 );
     MAKE_CHNL_OC( 5 );

     // Prepare for first edge
     SET_IC_EDGE(0, IC_EDGE_RISING);
     SET_IC_EDGE(1, IC_EDGE_RISING);
     ZERO_STATE_A = FIRST_RISING;
     ZERO_STATE_B = FIRST_RISING;

     // Enable the channel interrupt
     ENABLE_CHNL_INTERRUPT( 0 );
     ENABLE_CHNL_INTERRUPT( 1 );
     ENABLE_CHNL_INTERRUPT( 5 );

     // Clear any pending interrupts
     TFLG1 = (TFLG1_C0F_MASK | TFLG1_C1F_MASK | TFLG1_C5F_MASK);
     
     // Prepare watch dog for first interrupt
     TIMER_CHNL( 5 ) = TCNT + WatchDogTime;

}

/************DC_SPEED_A()******************************************************
Purpose: Calculates the speed of the motor
******************************************************************************/
void DC_SPEED_A (void)
{
     SPEED_CM_A =  FEEDBACK_SCALE_FACTOR / PERIOD_A;
     SPEED_CM_A /= SENSOR_GAIN;
     
     if(DC_PORT & DC_A2_MASK)
          SPEED_CM_A *= -1;
}

/************DC_SPEED_B()******************************************************
Purpose: Calculates the speed of the motor
******************************************************************************/
void DC_SPEED_B (void)
{
     SPEED_CM_B =  FEEDBACK_SCALE_FACTOR / PERIOD_B;
     SPEED_CM_B /= SENSOR_GAIN;
     
     if(DC_PORT & DC_B2_MASK)
          SPEED_CM_B *= -1;
}

/*********************TC0 interrupt handler************************************
Purpose: Encoder 1 interrupt
         -watches for the rising and falling edges and calculates the period
******************************************************************************/
interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 0 ) ) void TC0_IC_handler( void )
{

  static unsigned long EDGE_1;
  static unsigned long EDGE_2;

  // Clear pending interrupts
  TFLG1 = TFLG1_C0F_MASK;
  
  // done do no error on first rising edge
  if(ZERO_STATE_A == FIRST_RISING)
  {                                
    ZERO_STATE_A = FALLING;               
    SET_IC_EDGE(0, IC_EDGE_FALLING);            
    EDGE_1 = TC0 + (OVER_FLOW_A * 65536ul); 
  }
  
  // look for a rising edge
  if(ZERO_STATE_A == RISING)
  { 
    PERIOD_A = (TC0 + (OVER_FLOW_A * 65536ul)) - EDGE_1;
    //ONTIME = EDGE_2 - EDGE_1;      
    ZERO_STATE_A = FALLING;
    SET_IC_EDGE(0, IC_EDGE_FALLING);
    EDGE_1 = TC0 + (OVER_FLOW_A * 65536ul);   
  }
  
  // look for a falling edge
  else if(ZERO_STATE_A == FALLING) 
  {
    ZERO_STATE_A = RISING;
    SET_IC_EDGE(0, IC_EDGE_RISING);
    EDGE_2 = TC0 + (OVER_FLOW_A * 65536ul);            
  }
  
  // reset the overflow
  OVER_FLOW_A = 0;

}

/*********************TC1 interrupt handler************************************
Purpose: Encoder 1 interrupt
         -watches for the rising and falling edges and calculates the period
******************************************************************************/
interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 1 ) ) void TC1_IC_handler( void )
{

  static unsigned long EDGE_1;
  static unsigned long EDGE_2;

  // Clear pending interrupts
  TFLG1 = TFLG1_C1F_MASK;
  
  // done do no error on first rising edge
  if(ZERO_STATE_B == FIRST_RISING)
  {                                
    ZERO_STATE_B = FALLING;               
    SET_IC_EDGE(1, IC_EDGE_FALLING);            
    EDGE_1 = TC1 + (OVER_FLOW_B * 65536ul); 
  }
  
  // look for a rising edge
  if(ZERO_STATE_B == RISING)
  { 
    PERIOD_B = (TC1 + (OVER_FLOW_B * 65536ul)) - EDGE_1;
    //ONTIME = EDGE_2 - EDGE_1;      
    ZERO_STATE_B = FALLING;
    SET_IC_EDGE(1, IC_EDGE_FALLING);
    EDGE_1 = TC1 + (OVER_FLOW_B * 65536ul);   
  }
  
  // look for a falling edge
  else if(ZERO_STATE_B == FALLING) 
  {
    ZERO_STATE_B = RISING;
    SET_IC_EDGE(1, IC_EDGE_RISING);
    EDGE_2 = TC1 + (OVER_FLOW_B * 65536ul);            
  }
  
  // reset the overflow
  OVER_FLOW_B = 0;

}

/********************Overflow Handler******************************************
Purpose: Increase a counter each time an overflow occurs
******************************************************************************/
interrupt VectorNumber_Vtimovf void OVER_FLOW_HANDLER( void )
{
     OVER_FLOW_A++;
     OVER_FLOW_B++;    
}

/*******************motorA()***************************************************
Purpose: Set the speed and direction of motor A
Input:   int direction - direction the motor is gunna go
         int speed - speed of the motor
******************************************************************************/
void motorA(int direction, int speed) 
{
  DisableInterrupts;
  
  //set the direction port output
  if(direction == FORWARD)
    FORCE_BITS(DC_PORT, MOTOR_A_MASK, DC_A1_MASK);
  else if(direction == REVERSE)
    FORCE_BITS(DC_PORT, MOTOR_A_MASK, DC_A2_MASK);
  else if(direction == BRAKE)
    FORCE_BITS(DC_PORT, MOTOR_A_MASK, (DC_A1_MASK | DC_A2_MASK)); 
  
    SETPOINT_A = speed * SENSOR_GAIN;
  
  // set an output so that the motor will turn and trigger and isr and then correct itself
  DC_A_DTY = 250;
  
  EnableInterrupts;
}

/*******************motorB()***************************************************
Purpose: Set the speed and direction of motor A
Input:   int direction - direction the motor is gunna go
         int speed - speed of the motor
******************************************************************************/

void motorB(int direction, int speed) 
{
  DisableInterrupts;
  
  //set the direction port output
  if(direction == FORWARD)
    FORCE_BITS(DC_PORT, MOTOR_B_MASK, DC_B1_MASK);
  else if(direction == REVERSE)
    FORCE_BITS(DC_PORT, MOTOR_B_MASK, DC_B2_MASK);
  else if(direction == BRAKE)
    FORCE_BITS(DC_PORT, MOTOR_B_MASK, (DC_B1_MASK | DC_B2_MASK)); 
  
    SETPOINT_B = speed * SENSOR_GAIN;
  
  // set an output so that the motor will turn and trigger and isr and then correct itself
  DC_B_DTY = 250;
  
  EnableInterrupts;
}

/******************************Watchdog Handler()*************************
Purpose: Calculates the error and determine new pulsewidth output
*************************************************************************/
interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 5 ) ) void TC5_OC_handler( void ) 
{
     // Clear the interrupt flag 
     TFLG1 = TFLG1_C5F_MASK;

     // Motor A
     // calculate current error
     SPEED_ERROR_A = (signed long)SETPOINT_A - (signed long)(FEEDBACK_SCALE_FACTOR/PERIOD_A);

     if((SPEED_ERROR_A < STUPID_SPEED_ERROR) && (SPEED_ERROR_A > -STUPID_SPEED_ERROR))
     {
          // Do the integration
          if(((SPEED_ERROR_A > 0) && (DC_A_DTY < MAX_DRIVE_VALUE)) || ((SPEED_ERROR_A < 0) && (DC_A_DTY > MIN_DRIVE_VALUE)))
               SPEED_ERROR_INTEGRAL_A += SPEED_ERROR_A;
          
          // Calculate new output
          DRIVE_VALUE_A = ((SPEED_ERROR_A * P_GAIN_A) + (SPEED_ERROR_INTEGRAL_A * I_GAIN_A)) / GAIN_DIVISOR_A;

          // Make sure new pulsewidth is within the limits
          if(DRIVE_VALUE_A > MAX_DRIVE_VALUE)
               DRIVE_VALUE_A = MAX_DRIVE_VALUE;
          if(DRIVE_VALUE_A < MIN_DRIVE_VALUE)
               DRIVE_VALUE_A = MIN_DRIVE_VALUE;
          
          // Send pulsewidth to pwm register
          DC_A_DTY = (byte)DRIVE_VALUE_A;
     }
     
     // Motor B
     // calculate current error
     SPEED_ERROR_B = (signed long)SETPOINT_B - (signed long)(FEEDBACK_SCALE_FACTOR/PERIOD_B);

     if((SPEED_ERROR_B < STUPID_SPEED_ERROR) && (SPEED_ERROR_B > -STUPID_SPEED_ERROR))
     {
          // Do the integration
          if(((SPEED_ERROR_B > 0) && (DC_B_DTY < MAX_DRIVE_VALUE)) || ((SPEED_ERROR_B < 0) && (DC_B_DTY > MIN_DRIVE_VALUE)))
               SPEED_ERROR_INTEGRAL_B += SPEED_ERROR_B;
          
          // Calculate new output
          DRIVE_VALUE_B = ((SPEED_ERROR_B * P_GAIN_A) + (SPEED_ERROR_INTEGRAL_B * I_GAIN_A)) / GAIN_DIVISOR_A;

          // Make sure new pulsewidth is within the limits
          if(DRIVE_VALUE_B > MAX_DRIVE_VALUE)
               DRIVE_VALUE_B = MAX_DRIVE_VALUE;
          if(DRIVE_VALUE_B < MIN_DRIVE_VALUE)
               DRIVE_VALUE_B = MIN_DRIVE_VALUE;
          
          // Send pulsewidth to pwm register
          DC_B_DTY = (byte)DRIVE_VALUE_B;
     }


     TIMER_CHNL( 5 ) = TCNT + WatchDogTime;
}