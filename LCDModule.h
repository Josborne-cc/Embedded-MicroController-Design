/*
Purpose: Macros for using the LCD module
Author:  Josh Osborne
Date:    April 4, 2015
Version: 00
*/

#ifndef _LCDModule_H
#define _LCDModule_H

// Maximum number of characters that can be printed to the screen
#define MAX_CHARACTERS 32

// Macro to move cursor to new line
#define LCD_NEW_LINE 0xC0

// Macro to identify LCD GPIO Port
#define LCD_PORT PORTA

// Macro to identify LCD DDR register
#define LCD_PORT_DDR DDRA

// Macro to specify enable bus bits
#define LCD_E_MASK PORTA_BIT4_MASK

// Macro to specify RS bus bits
#define LCD_RS_MASK PORTA_BIT5_MASK

// Macro to specify RW bus bits
#define LCD_RW_MASK PORTA_BIT6_MASK

// Macro to specify LCD bus mask
#define LCD_BUS_MASK 0x0Fu

// Macro for clearing LCD
#define LCD_CLEAR_MASK 0x01u

// Macro to return cursor to home position
#define LCD_HOME_MASK 0x02u

// Macro to initialize the LCD
#define LCD_CMD_FUNCTION 0x20
#define LCD_FUNCTION_4BIT 0x00
#define LCD_FUNCTION_2LINES 0x08
#define LCD_FUNCTION_5X8FONT 0x00
#define LCD_DISPLAY 0x08
#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_NOCURSOR 0x00
#define LCD_NOBLINK 0x00
#define LCD_ENTRY 0x04
#define LCD_INC 0x02
#define LCD_MOVE_CURSOR 0x00

// Macro to evaluate the lower nybble of an argument
#define LO_NYBBLE(value)  ( ( value ) & 0x0F )

// Macro to evaluate the upper nybble of an argument
// -could also be LO_NYBBLE((value)>>4)
#define HI_NYBBLE(value)  ( ( ( value ) >> 4 ) & 0x0F )

// Macro to set the enable line low
#define LCD_E_LO CLR_BITS( LCD_PORT, LCD_E_MASK )

// Macro to set the enable line high
#define LCD_E_HI SET_BITS( LCD_PORT, LCD_E_MASK )

// Macro to set the register select to direction register
#define LCD_RS_DR SET_BITS( LCD_PORT, LCD_RS_MASK )

// Macro to set register select to instruction register
#define LCD_RS_IR CLR_BITS( LCD_PORT, LCD_RS_MASK )

// Macro to set RW register to write
#define LCD_RW_WRITE  CLR_BITS(LCD_PORT, LCD_RW_MASK) 

/* Macro to force the 4 bits of the port connected to the 
LCD data pins to the value of the lower nybble of its argument*/
#define LCD_BUS(value) FORCE_BITS( LCD_PORT, LCD_BUS_MASK, ( value ) )

// Prototypes for LCDModule.c
static void LCDdata(unsigned char value); 
static void LCDcmd(unsigned char value);
void LCDprintf(char *format, ...);
void LCDclear(void);
void LCDputc(char cx);
void LCDputs(char *sx);
void LCDinit(void);

#endif