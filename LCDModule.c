/*
Purpose: Functions for using the LCD module
Author:  Josh Osborne
Date:    April 4, 2015
Version: 01
*/

#include "derivative.h"      /* derivative-specific definitions */
#include "LCDModule.h"
#include "TimerModule.h"
#include "utils.h"
#include <stdarg.h>

/********************LCDdata()*************************************************
Purpose: Write character to the data register of the LCD
******************************************************************************/ 
static void LCDdata(unsigned char value) 
{
  msDELAY(2);                 // Set 2ms Delay
  LCD_E_LO;                   // Set enable low
  LCD_RS_DR;                  // Use direction register
  LCD_RW_WRITE;               // Use write line
  LCD_E_HI;                   // Set enable high
  LCD_BUS( HI_NYBBLE(value)); // Send the upper nybble
  LCD_E_LO;                   // Set enable low
  LCD_E_HI;                   // Set enable high
  LCD_BUS( LO_NYBBLE(value)); // Send the lower nybble
  LCD_E_LO;                   // Set enable low
}

/******************LCDcmd()****************************************************
Purpose: Write character to the command register of the LCD
*******************************************************************************/
static void LCDcmd(unsigned char value) 
{
  msDELAY(2);
  LCD_E_LO;
  LCD_RS_IR;
  LCD_RW_WRITE;
  LCD_E_HI;
  LCD_BUS( HI_NYBBLE(value));
  LCD_E_LO;
  LCD_E_HI;
  LCD_BUS( LO_NYBBLE(value));
  LCD_E_LO;
}

/**************LCDprintf()*****************************************************
Purpose: Diplicates printf() for the LCD screen
Input:   Arguments are the same as printf()
******************************************************************************/
void LCDprintf(char *format, ...) 
{
  va_list myArgs;
  char Buffer[MAX_CHARACTERS];
  va_start(myArgs, format);
  (void)vsprintf(Buffer,format,myArgs);
  va_end(myArgs);
  LCDputs(Buffer); 
}

/****************LCDinit()*****************************************************
Purpose: Initalize the LCD
******************************************************************************/
void LCDinit(void) 
{
  SET_BITS(LCD_PORT_DDR, 0x7Fu);
  // run through sync sequence from datasheet to start 4-bit interface    
  LCD_E_HI;
  LCD_BUS( 0x03 );      // wake up display & sync
  LCD_E_LO;

  msDELAY(5);

  LCD_E_HI;
  LCD_BUS( 0x03 );      // wake up display & sync
  LCD_E_LO;

  msDELAY(1);   

  LCD_E_HI;
  LCD_BUS( 0x03 );      // wake up display & sync
  LCD_E_LO;

  LCD_E_HI;
  LCD_BUS( 0x02 );      // wake up display & sync - go to 4-bit mode
  LCD_E_LO;

  msDELAY(2);

  // now that we're sync'd and in 4-bit mode, issue commands to configure the display
  LCDcmd( LCD_CMD_FUNCTION | LCD_FUNCTION_4BIT | LCD_FUNCTION_2LINES | LCD_FUNCTION_5X8FONT );
  LCDcmd( LCD_DISPLAY | LCD_DISPLAY_OFF );
  LCDcmd(LCD_CLEAR_MASK);
  LCDcmd( LCD_ENTRY | LCD_MOVE_CURSOR | LCD_INC );
  LCDcmd( LCD_DISPLAY | LCD_DISPLAY_ON | LCD_NOCURSOR | LCD_NOBLINK );

}

/*************LCDputc()********************************************************
Purpose: Puts a character to the screen
Input:   char cx - character to print
******************************************************************************/ 
void LCDputc(char cx) 
{
  switch(cx)
  {
    case 13:        //carriage return
      LCDcmd(LCD_HOME_MASK);
      break;
    case 10:        // line feed
      LCDcmd(LCD_NEW_LINE);
      break;
    default:
      LCDdata(cx);
  }
}

/*************LCDclear()*******************************************************
Purpose: Clear the LCD screen
******************************************************************************/
void LCDclear(void) 
{
  LCDcmd(LCD_CLEAR_MASK);
  msDELAY(16);
}



/***************LCDputs()******************************************************
Purpose: Print String to the LCD
Input:   char *cx - pointer to string of characters
******************************************************************************/
void LCDputs(char *sx)
{
  int count = 0;
  
  while(*sx)
  {
    if(count == 16)
      LCDcmd(LCD_NEW_LINE);
    if(count == 32) 
      break;
    LCDputc(*sx); 
    sx++;
    count++;
  }
}