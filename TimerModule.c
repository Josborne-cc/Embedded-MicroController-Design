/*
Purpose: Functions for using the timer module
Author:  Josh Osborne
Date:    April 4, 2015
Version: 02
*/

#include "derivative.h"      /* derivative-specific definitions */
#include "TimerModule.h"
#include "utils.h"


/**************msDELAY()*******************************************************
Purpose: ms Delay
Input:   int k - number of milliseconds
**Currently Uses a prescaler of 8**
******************************************************************************/
void msDELAY(int k) 
{
  int ix;
  
  TC6 = TCNT + msDELAY_TICKS;  // Preset TC0 for first OC event
  
  MAKE_CHNL_OC( 6 );  // Set channel as OC
  //ENABLE_CHNL_INTERRUPT( 6 );
  SET_OC_ACTION(6, OC_OFF);
  for(ix = 0; ix < k; ix++)
  {
    while(!(TFLG1 & TFLG1_C6F_MASK));
      TC6 += msDELAY_TICKS;  
  }
  
  TIOS &= LOW(~TIOS_IOS6_MASK);  // Turn off OC
}