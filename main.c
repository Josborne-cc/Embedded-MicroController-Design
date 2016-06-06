/*
Purpose: Start of project
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "SCIModule.h"
#include "TimerModule.h"
#include "ServoModule.h"
#include "StepperModule.h"
#include "LCDModule.h"
#include "DCMotorModule.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char cmd;
int arg1;
int arg2;
unsigned long CMD_COUNT;
char cpy[SCI_Rx_BUF_LENGTH];
char command[SCI_Rx_BUF_LENGTH];
char send[SCI_Rx_BUF_LENGTH];

void parse_cmd(void);
void select_cmd(void);
void copy_string(char *dest, char *source, int num);
void clear_string(char *str, int num);
void return_cmd(void);



void main(void) 
{
  
     SET_TCNT_PRESCALE( TCNT_PRESCALE_8);
     TSCR1 = TSCR1_INIT;

     SCIinit(BAUD_RATE_DIVISOR_9600, N81_DATA, (SCICR2_RE_MASK|SCICR2_TE_MASK|SCICR2_RIE_MASK));
     SERVOinit();
     STEPPERinit();
     LCDinit();
     ENCODERinit();
     DCMOTORAinit();
     DCMOTORBinit();


     EnableInterrupts;
     (void)STEPPERhome();
     

     for(;;) 
     {

          asm("nop");
          
          DisableInterrupts;
          DC_SPEED_A();
          DC_SPEED_B();
          EnableInterrupts;

          // Are there any unprocessed commands
          if(MESSAGE_COUNT > CMD_COUNT)
          {
               parse_cmd();
               select_cmd();

          }
          
          LCDprintf("Right Speed: %ld \n", SPEED_CM_A);
          LCDprintf("Left Speed: %ld \r", SPEED_CM_B);


     } /* loop forever */
     /* please make sure that you never leave main */
}

void select_cmd(void)
{
     switch(cmd)
     {
          case 'P':
               return_cmd();
               break;
          case 'E':
               if(arg1 == 0)
                    servo_position(arg2);
               else if(arg1 == 1)
                    servo_sweep(arg2, 5000);
               return_cmd();
               break;
          case 'S':
               if(arg1 == 0)
                    (void)STEPPERhome();
               else if(arg1 == 1)
                    (void)STEPPER_MOVE(arg2);
               else if(arg1 == 2)
                    (void)STEPPER_RELATIVE(arg2);
               return_cmd();
               break;
          case 'G':
               motorA(arg1, arg2);
               return_cmd();
               break;
          case 'H':
               motorB(arg1, arg2);
               return_cmd();
               break;
          default:
               return;
     }
}

void return_cmd(void)
{
     char temp;
     
     temp = tolower(cmd);
     
     (void)sprintf(send, "<%c %d %d>", temp, arg1, arg2);
     asm("nop");
     putsSCI(send);
}

void parse_cmd(void)
{
     int i;
     int neg_arg = 0;
     
     cmd = '\0';
     arg1 = 0;
     arg2 = 0;
     
     
     
     // Copy the serial buffer
     DisableInterrupts;
     // this is a problem
     //(void)strncpy(cpy, (signed char *)SCI_Rx_BUF, SCI_Rx_BUF_LENGTH);
     copy_string(cpy, (signed char*)SCI_Rx_BUF, SCI_Rx_BUF_LENGTH);
     EnableInterrupts;
     
     // Get the command from buffer
     for(i = 0; i < SCI_Rx_BUF_LENGTH; i++)
     {
          //i %= SCI_Rx_BUF_LENGTH;
          CMD_START = (CMD_START+1) % SCI_Rx_BUF_LENGTH;
          if(cpy[CMD_START] == '>')
          {
               command[i] = ' '; // add space at end of string
               break;
          }
          command[i] = cpy[CMD_START];
                 
     }
     
     
     //command[strlen(command) + 1] = '\0';
     
     i = 0;
     asm("nop");
     // Get the command
     while(command[i] != ' ')
     {
          
          cmd = command[i];
          i++;
     }
     i++;
     // Get the first argument
     while(command[i] != ' ')
     {
          arg1 *= 10;
          arg1 += (command[i] - 0x30);
          i++;
     }
     i++;
     // Get the second argument
     while(command[i] != ' ')
     {
          if(command[i] == '-')
               neg_arg = 1;
          else
          {   
               arg2 *= 10;
               arg2 += (command[i] - 0x30);
          }
          i++;
     }
     
     if (neg_arg == 1)
          arg2 *= -1;
     neg_arg = 0;

     // indicate that this command has been processed
     CMD_COUNT++;
     
     //clear_string(command, 16);     
}

/********************************copy_string***********************************
Purpose: Copy two strings that do not have a terminating null character
Input:   char *dest - pointer to the destination string
         char *source - the source string to copy
         int num -  the number of characters that will be copied
         
***NOTE: strings have no terminating characters therefore num is essential
         and function will not work without correct string length
         Do Not pass strlen(string) as the length         
******************************************************************************/
void copy_string(char *dest, char *source, int num)
{
     int i;
     
     for(i = 0; i < num; i++)     
     {
          dest[i] = source[i];
     }

}

void clear_string(char *str, int num)
{
     int i;
     
     for(i = 0; i < num; i++)
     {
          str[i] = '\0';
     }
}