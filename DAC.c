/*
Purpose: Get the DAC working
Author:  Josh Osborne
Date:    April 28, 2015
Version: 00
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "LCDModule.h"
#include "TimerModule.h"
#include "utils.h"

#define SPICR1_INIT 0b01010000
#define SPI_DIV 0x02;

#define SPI_CS_PORT PTJ
#define SPI_CS_PORT_DDR DDRJ
#define SPI_CS1_MASK PTJ_PTJ6_MASK

#define SPI_PORT PTM
#define SPI_PORT_DDR DDRM
#define SPI_SCK_MASK PTM_PTM5_MASK
#define SPI_MOSI_MASK PTM_PTM4_MASK
#define SPI_NSS_MASK PTM_PTM3_MASK
#define SPI_MISO_MASK PTM_PTM2_MASK

#define ON  0b1101110000000000
#define OFF 0b1110110000000000


void main(void) {

  
     SPICR1 = SPICR1_INIT;

	EnableInterrupts;


  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}

/**************putcSPI*********************************************************
Purpose: Write a character to the SPI port
Input:   char cx - character to write
******************************************************************************/
void putcSPI(char cx)
{
     while(!(SPISR & SPISR_SPTEF_MASK));
     
     SPIDR = cx;
     
     while(!(SPISR&SPISR_SPIF_MASK));
     
     (void)SPIDR;
}

/*******************SPInit()***************************************************
Purpose: Initalize the SPI PORT
*******************************************************************************/
void SPIinit(void)
{
  // Slave select
  SET_BITS(SPI_CS_PORT_DDR, SPI_CS1_MASK);
  // Init from class
  SPICR1 = SPICR1_INIT;
  // 1 MBPS
  SPIBR = SPI_DIV;
  // Initialize to on
  SET_BITS(SPI_CS_PORT, SPI_CS1_MASK);
}

/********************DAC_FINISH()**********************************************
Purpose: Turn the DAC OFF
         - Select Slave
         - Send upper byte
         - Send lower byte
         - Unselect slave
******************************************************************************/
void DAC_START(void)
{
   CLR_BITS(SPI_CS_PORT, SPI_CS1_MASK);
   putcSPI(ON >> 8);            
   putcSPI(ON & 0xFF);          
   SET_BITS(SPI_CS_PORT, SPI_CS1_MASK);
}

/********************DAC_FINISH()**********************************************
Purpose: Turn the DAC OFF
         - Select Slave
         - Send upper byte
         - Send lower byte
         - Unselect slave
******************************************************************************/
void DAC_FINISH(void)
{
  CLR_BITS(SPI_CS_PORT, SPI_CS1_MASK);
  putcSPI(OFF >> 8);
  putcSPI(OFF & 0xFF);   
  SET_BITS(SPI_CS_PORT, SPI_CS1_MASK);
}



