/******************************************************************* 
 * File:   uartLib.h
 * Author: Jose Domingues (j.domingues@ua.pt)
 *         Diogo Vala Correia (dv.correia@ua.pt)
 * Created on March 2018
 * 
 * Resume:
 * This library handles UART1 Configuration and interaction functions for
 * Max32 Digilent PIC32MX795F512L microcontroller.
/******************************************************************/

#ifndef UARTLIB_H
#define	UARTLIB_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*******************************************************************
 * NAME :            void uartInit(baudrate, parity, stopbits)
 * DESCRIPTION :     Initializes UART1 with a defined baudrate.
 * INPUTS :
 *   uint16_t        baurate    Baudrate value for UART1
 *   char            parity     Choose parity bits and data bits:
 *                      'N':  8-bit data, no parity
 *                      'E':  8-bit data, even parity
 *                      'O':  8-bit data, odd parity
 *                      'M':  9-bit data, no parity
 *   uint8_t         stopbits   Choose number of stopbits:
 *                       1:   1 stop bits
 *                       2:   2 stop bits
 * NOTES :           Uses USB port in digilent max32 development board
 *******************************************************************/
void uartInit(uint16_t, char, uint8_t);

/*******************************************************************
 * NAME :            void putChar(c)
 * DESCRIPTION :     Sends char to UART.
 * INPUTS :
 *   char            c          Char to send
 *******************************************************************/
void putChar(char);

/*******************************************************************
 * NAME :            void getChar()
 * DESCRIPTION :     Gets char from UART.
 * OUTPUTS :
 *   char            c          Char from UART RX
 *******************************************************************/
char getChar();

/*******************************************************************
 * NAME :            void putString(char *s)
 * DESCRIPTION :     Sends a string to UART.
 * INPUTS :
 *   char *          String first char pointer        
 *******************************************************************/
void putString(char *);

/*******************************************************************
 * NAME :            void printInt10(uint32_t value)
 * DESCRIPTION :     Sends a Interiger to UART.
 * INPUTS :
 *   int32_t         Intereger value        
 *******************************************************************/
void printInt10(int32_t);

int getCoordinatesXX();
int getCoordinatesYY();

#ifdef	__cplusplus
}
#endif
#endif

