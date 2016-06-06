/*
Purpose: Servo Macros
Author:  Josh Osborne
Date:    April 13, 2015
Version: 00
*/
#ifndef _ServoModule_H
#define _ServoModule_H

// Used for setting flags
#define LOW_ 0
#define HIGH 1

// Used for output calculation
#define GAIN 9
#define OFFSET 1590

// Maximum and minimum pulse width
#define HIGHLIMIT 2400
#define LOWLIMIT 1300

// Maximum and Minimum angles
#define MAXANGLE 90
#define MINANGLE -20


#define SERVO_INIT_POSITION 1590 

// Flag for using the sweep                                                                                                                                                                                                                                                       0
extern int SWEEP;
// Value of final on time
extern long DESTINATION;
// State of output waveform
extern volatile int STATE;
// Time till move complete
extern unsigned long TIME;
// On time for output waveform
extern long long PULSE_WIDTH; 

// Prototypes
void SERVOinit(void);
int servo_position(int angle);
unsigned long get_pulse(int angle);
int servo_sweep(int angle, unsigned long msecs);
interrupt VECTOR_NUM( TIMER_CHNL_VECTOR( 2 ) ) void TC2_OC_handler( void );

#endif