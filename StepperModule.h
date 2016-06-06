/******************************************************************************
Purpose: Macros for the stepper motor
Author:	 Josh Osborne
Date:	 April 9, 2015
Version: 00
******************************************************************************/
#ifndef _StepperModule_H
#define _StepperModule_H

// Macro to define the stepper motor port
#define STEPPER_MOTOR_PORT PTT

// Macro to define stepper motor DDR
#define STEPPER_MOTOR_DDR DDRT

// Macro to initialise stepper motor port
#define STEPPER_MOTOR_PORT_INIT 0

// Macro to initalize stepper motor DDR
#define STEPPER_MOTOR_DDR_INIT (PTT_PTT4_MASK | PTT_PTT5_MASK | PTT_PTT6_MASK | PTT_PTT7_MASK)

// Macro to define mask for writing to stepper motor
#define STEPPER_MOTOR_MASK 0xF0

// Macro for counting number of stepper motor turn positions
#define STEPPER_MOTOR_COUNT_MASK 0x08

// Macro to define switch port
#define SWITCH_PORT PTP

// Macro to define switch DDR
#define SWITCH_DDR DDRP

// Macro to initialize switch port
#define SWITCH_PORT_INIT 0

// Macro to initialize switch DDR
#define SWITCH_DDR_INIT ~(PTP_PTP2_MASK | PTP_PTP3_MASK)

// Macro to define direction that motor is turning
#define CLOCKWISE 0
#define COUNTERCLOCKWISE 1

// Macro to define states
#define LOW_ 0
#define HIGH 1

// Macro to define mode of operation
#define CONTINUOUS 0
#define FIXED 1

// Macro to define motor step pattern incrementation
#define HALF 1
#define FULL 2

// GLOBALS
// Step pattern
extern int stepper_step;
// Stepper Speed
extern int stepper_speed;
// Left Side tick count
extern int stepper_left;
// Right Side tick count
extern int stepper_right;
// Position in step pattern
extern unsigned int step_num;
// Step type full or half
extern volatile int step_type;
// Tick count for stepper position in rotation
extern volatile int stepper_count;
// Direction stepper is turning
extern volatile int stepper_rotation;
// Flag which means stepper is being initialized
extern volatile int stepper_init_flag;
// Direction stepper is turning
extern volatile int stepper_direction;
// Pattern that moves the stepper motor
extern int pattern[];

// Prototypes
int STEPPERhome(void);
void STEPPERinit(void);
void change_step(int step);
int get_stepper_state(void);
int STEPPER_MOVE(int angle);
int get_stepper_position(void);
int get_stepper_ticks(int angle);
int STEPPER_RELATIVE(int degrees);

#endif