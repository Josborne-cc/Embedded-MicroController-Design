/*
Purpose: Functions for using the SCI module
Author:  Josh Osborne
Date:    April 6, 2015
Version: 00
*/

#include "derivative.h"      /* derivative-specific definitions */
#include "TimerModule.h"
#include "SCIModule.h"
#include "utils.h"

volatile char SCI_Rx_BUF[SCI_Rx_BUF_LENGTH] = {0};
volatile unsigned int SCI_Rx_INDEX = 0;
int CMD_FLAG;
int COMM_ERROR;
int CMD_START;
unsigned long MESSAGE_COUNT;


/*************SCIintit()*******************************************************
Purpose: Initalize the SCI module
Input:   int BAUD_RATE - a divisor to get the desired baud rate
         int SCI_DATA  - a value that determines parity and data bits
         int CONFIG    - a value that specifies what features the the module will be used
******************************************************************************/
void SCIinit(int BAUD_RATE, int SCI_DATA, int CONFIG) 
{
  SCIBD = BAUD_RATE;
  SCICR1 = LOW(SCI_DATA);
  SCICR2 = LOW(CONFIG);
}

/************putcSCI()*********************************************************
Purpose: Put a character to the Serial Port
Input:   char cx - character to be printed
******************************************************************************/
void putcSCI(char cx) 
{
  while(!(SCISR1&SCISR1_TDRE_MASK));
  SCIDRL = cx;
}

/***********putsSCI()**********************************************************
Purpose: Put a string to the serial port
Input:   char *str - pointer to the printed string
******************************************************************************/
void putsSCI(char *str) 
{
  while(*str)
    putcSCI(*str++);
}

/*
*/
void getsSCI(int num, char *str) 
{
  int count;
  
  for(count = 0; count < num; count++)
     str[count] = SCI_Rx_BUF[(SCI_Rx_INDEX - num + count)];
     
  str[num] = '\0'; 
}

/*************getcSCI()********************************************************
Purpose: Get a character from the serial port
Input:   char cx - character that is grabed
******************************************************************************/
void getcSCI(char cx) 
{
     if(CMD_FLAG == 1 && cx == '<')
     {
          COMM_ERROR = 1;
     }

     if(cx == '<')
     {
          CMD_START = SCI_Rx_INDEX;
          CMD_FLAG = 1;
     }


     if(CMD_FLAG == 1) 
     {  
          //SCI_Rx_BUF[SCI_Rx_INDEX] = cx;
         // SCI_Rx_INDEX = (SCI_Rx_INDEX + 1) % SCI_Rx_BUF_LENGTH;
           *(SCI_Rx_BUF + (SCI_Rx_INDEX++)) = cx;
           SCI_Rx_INDEX %= SCI_Rx_BUF_LENGTH;
     }
     
     if(cx == '>')
     {
          MESSAGE_COUNT++;
          CMD_FLAG = 0;
     }

}

void SCIgetc(char buf)
{
  *(SCI_Rx_BUF + (SCI_Rx_INDEX++)) = buf;
  SCI_Rx_INDEX %= SCI_Rx_BUF_LENGTH;
}

/************SCI_HANDLER()*****************************************************
Purpose: Grab a character from the serial port on every interupt
******************************************************************************/
interrupt VectorNumber_Vsci void SCI_HANDLER( void )
{
  
  
  // If data is available on port
  if(SCISR1 & SCISR1_RDRF_MASK)
  {
    getcSCI(SCIDRL);
    //SCIgetc(SCIDRL);
    // Clear indicator stating that there is data on the port
    //CLR_BITS(SCISR1, SCISR1_RDRF_MASK);
  }
}