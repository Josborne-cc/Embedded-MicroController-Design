#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "LCDModule.h"
#include "TimerModule.h"
#include "utils.h"


// Power on ATD, set fast clear flag, halt conversions while frozen
#define ATDCTL2_INIT (ATDCTL2_ADPU_MASK | ATDCTL2_AFFC_MASK | ATDCTL2_AWAI_MASK); 


// 3 conversions, no FIFO, finish current conversion and halt on freeze
#define ATDCTL3_INIT (ATDCTL3_S2C_MASK | ATDCTL3_S1C_MASK | ATDCTL3_FRZ1_MASK);


// 8-bit resolution, 8-ATD clock periods, prescaler of 10 (5-20MHz)
#define ATDCTL4_INIT (ATDCTL4_SRES8_MASK | ATDCTL4_SMP1_MASK | ATDCTL4_PRS2_MASK);


// Axis selection:
#define AXIS_MASK (ATDCTL5_CC_MASK | ATDCTL5_CB_MASK | ATDCTL5_CA_MASK)
#define X_AXIS_MASK 0
#define Y_AXIS_MASK ATDCTL5_CA_MASK
#define Z_AXIS_MASK ATDCTL5_CB_MASK

// Left justified, signed
#define ATDCTL5_INIT ATDCTL5_DSGN_MASK

// Axis registers
#define X_AXIS_REG ATDDR0
#define Y_AXIS_REG ATDDR1
#define Z_AXIS_REG ATDDR2

// Accelerometer axis indices
#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

char getX(void);
char getY(void);
char getZ(void);
void Accelinit(void);
void Accelread(float *readings);

void main(void) 
{
     float x;
     float y;
     float z;
     
     float read[3];
     SET_TCNT_PRESCALE( TCNT_PRESCALE_8 );
     TSCR1 = TSCR1_INIT;
  
     
     Accelinit();
     LCDinit();
      


     EnableInterrupts;


  for(;;) 
  {

   // x = ((float)getX() * 0.2) + 9;
    //y = ((float)getY() * 0.2) + 9;
    //z = ((float)getZ() * 0.2) + 9;
    

    Accelread(read); 
    LCDprintf("X: %1.3f Y: %1.3f \n", read[X_AXIS], read[Y_AXIS]);
    LCDprintf("Z: %1.3f \r", read[Z_AXIS]);
    
    msDELAY(200);
   // _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}


char getX(void)
{
  FORCE_BITS(ATDCTL5, AXIS_MASK, X_AXIS_MASK);              
  while(!(ATDSTAT1 & ATDSTAT1_CCF0_MASK));
  return (ATDDR0 >> 8);
}

/**********getY()**************************************************************
Pupose: - Read the Y axis
        - Wait for read
        - Return the value as char
*******************************************************************************/
char getY(void)
{
  FORCE_BITS(ATDCTL5, AXIS_MASK, Y_AXIS_MASK);           
  while(!(ATDSTAT1 & ATDSTAT1_CCF1_MASK));
  return (ATDDR1 >> 8);
}


char getZ(void)
{
  FORCE_BITS(ATDCTL5, AXIS_MASK, Z_AXIS_MASK);               
  while(!(ATDSTAT1 & ATDSTAT1_CCF2_MASK));
  return (ATDDR2 >> 8);
}


void Accelinit(void)
{

  ATDCTL2 = ATDCTL2_INIT;  
  ATDCTL3 = ATDCTL3_INIT;
  ATDCTL4 = ATDCTL4_INIT;
  ATDCTL5 = ATDCTL5_INIT;
  
  SET_BITS(ATDCTL2, ATDCTL2_ADPU_MASK);  
}


void Accelread(float *read)
{ 
   // out = gain * in + offset
   // gain = 0.2
   // ofset = 9 
   // except 1.8 for z 
   read[X_AXIS] = ((float)getX() *0.2) +9;
   read[Y_AXIS] = ((float)getY() *0.2) +9;
   read[Z_AXIS] = ((float)getZ() *0.2) +1.8;
}

