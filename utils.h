/*
Purpose: Utility Macros
Author:  Josh Osborne
Date:    April 4, 2015
Version: 00
*/

#ifndef _UTILS_H
#define _UTILS_H

// Macro to grab only the lower byte of an argument
#define LOW( value )    ((value) & 0xFF)

// Macro to set specified bits
#define SET_BITS( port, bits )            (port) |= (bits)

// Macro to clear specified bits
#define CLR_BITS( port, bits )            (port) &= LOW(~(bits))

//Macro to flip specified bits
#define FLIP_BITS( port, bits )           (port) ^= (bits)

// Macro to force the bits identified by mask to take on the values of the matching bits in value
#define FORCE_BITS( port, mask, value)    (port) = ((port) & LOW(~(mask)) | ((value) & (mask)))
#define FORCE_WORD( port, mask, value)    (port) = ((port) & ((~(mask)) & 0xFFFF) | ((value) & (mask)))

#endif
