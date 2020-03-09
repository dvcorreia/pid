/*! \file adcLib.h

 * Author: Jose Domingues (j.domingues@ua.pt)
 *         Diogo Vala Correia (dv.correia@ua.pt)

 * Created on April 2018
 * 
 * Resume:
 * This library handles ADC Configuration and interaction functions for
 * Max32 Digilent PIC32MX795F512L microcontroller.
 */


#ifndef ADCCONFIG_H
#define	ADCCONFIG_H


#include <stdint.h>
#include <stdbool.h>




#ifdef	__cplusplus
extern "C" {
#endif
    
    
int delayADC = 33;

/*!
 * NAME :            void adcInit(void)

 * DESCRIPTION :     Initializes ADC1 module with a defined sample time, with two channels, AN0 and AN1

 * NOTES :           10bit ADC
 */
void adcInit();

/*!
 * NAME :            void readVoltageXX()

 * DESCRIPTION :     Returns the voltage read by the Analog pin of the XX axis (AN0)

 */
int readVoltageXX();

/*!
 * NAME :            void readVoltageYY()

 * DESCRIPTION :     Returns the voltage read by the Analog pin of the YY axis (AN1)

 */
int readVoltageYY();

/*!
 * NAME :            void readXX()

 * DESCRIPTION :     Returns the XX coordinates of the object on the touch screen

 * NOTES :           Coordinates are measured from the center and are returned in milimiters 
 */
int readXX();

/*!
 * NAME :            void readYY()

 * DESCRIPTION :     Returns the YY coordinates of the object on the touch screen

 * NOTES :           Coordinates are measured from the center and are given in milimiters 

 */
int readYY();


    
    
    

#ifdef	__cplusplus
}
#endif

#endif	/* ADCCONFIG_H */

