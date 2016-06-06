/*
Purpose: Macros for use with the DC motor
Author:  Josh Osborne
Date:    April 15, 2015
Version: 00
*/

#ifndef _DCMotorModule_H
#define _DCMotorModule_H

// Macro to Define the port
#define DC_PORT PORTB
#define DC_DDR DDRB

// Macro to define the bit for each motor direction
#define DC_A1_MASK PORTB_BIT0_MASK
#define DC_B1_MASK PORTB_BIT1_MASK
#define DC_A2_MASK PORTB_BIT2_MASK
#define DC_B2_MASK PORTB_BIT3_MASK

// Macro to define PWM ports
#define DC_E_PORT PTP
#define DC_E_DDR DDRP

// Macro to define the PWM enable
#define DC_A_E_MASK PTP_PTP4_MASK
#define DC_B_E_MASK PTP_PTP5_MASK

// Macro to define the period
#define DC_A_PER PWMPER4
#define DC_B_PER PWMPER5

// Macro to define the duty cycles
#define DC_A_DTY PWMDTY4
#define DC_B_DTY PWMDTY5

// Macro to define the counters
#define DC_A_CNT PWMCNT4
#define DC_B_CNT PWMCNT5

// 8 bit
#define BIT_8 0
// Divide by 32
#define DIV_32 0x04u

// 1k period
#define PERIOD_1K 250
// 50% Duty cycle
// 250 / 2 / 2
#define DUTY_50 125
// Counter starts at 0
#define START 0

// Limits for motor speed
#define MAX_SPEED 35
#define MIN_SPEED 20

// Macros for direction motor is travelling
#define BRAKE   2
#define FORWARD 1
#define REVERSE 0

// Marco for masks when changing directions
#define MOTOR_A_MASK 0b00000101
#define MOTOR_B_MASK 0b00001010

// Encoder Edge States
#define RISING  1
#define FALLING 0
#define FIRST_RISING 2

// Parameters for integral calculation
#define I_GAIN_A 1l
#define P_GAIN_A 500l
#define GAIN_DIVISOR_A 100l

// Parameters for scaling the speed values
#define FEEDBACK_SCALE_FACTOR 16777216
#define SENSOR_GAIN 613

// Outragous error
#define STUPID_SPEED_ERROR 1000000

// Drive limits
#define MIN_DRIVE_VALUE 0
#define MAX_DRIVE_VALUE 250

// Watch dog interval time
#define WatchDogTime 5000

// Overflow counters
extern unsigned int OVER_FLOW_A;
extern unsigned int OVER_FLOW_B;

// Encoder states
extern volatile int ZERO_STATE_A;
extern volatile int ZERO_STATE_B;

// Encoder periods
extern volatile signed long PERIOD_A;
extern volatile signed long PERIOD_B;

// Speed measurements
extern volatile signed long SPEED_CM_A;
extern volatile signed long SPEED_CM_B;

// Input set points
extern volatile signed long SETPOINT_A;
extern volatile signed long SETPOINT_B;

// Encoder Error
extern volatile signed long SPEED_ERROR_A;
extern volatile signed long SPEED_ERROR_B;

// Pulse width drive value
extern volatile signed long DRIVE_VALUE_A;
extern volatile signed long DRIVE_VALUE_B;

// Encoder Error integral
extern volatile signed long SPEED_ERROR_INTEGRAL_A;
extern volatile signed long SPEED_ERROR_INTEGRAL_B;


// Prototypes
extern void DC_SPEED_A (void);
extern void DC_SPEED_B (void);
extern void ENCODERinit(void); 
extern void DCMOTORAinit(void);
extern void DCMOTORBinit(void);
extern void motorA(int direction, int speed); 
extern void motorB(int direction, int speed);
//extern interrupt VectorNumber_Vtimovf void OVER_FLOW_HANDLER( void );
//extern interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 0 ) ) void TC0_IC_handler( void );
//extern interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 5 ) ) void TC5_OC_handler( void );

#endif