/*! \file pwmLib.h
 * Authors: Diogo Vala Correia (dv.correia@ua.pt)
 *          Jose Domingues (j.domingues@ua.pt)

 * Created on March 2018
 * 
 * Resume:
 * This library handles PWM Configuration and Parameters for Parallax Standard 
 * Servo BS1 in PIC32MX795F512L microcontroller.
 * 
 * Servo characteristics:
 *   Period:          20 ms       50 Hz
 *       - minimum:   0.75 ms     PWM(0.0375)
 *       - center:    1.5 ms      PWM(0.075)
 *       - maximum:   2.25 ms     PWM(0.1125)
 * 
 */
#ifndef PWMLIB_H
#define	PWMLIB_H

#ifdef	__cplusplus
extern "C" {
#endif
/*!
 * DESCRIPTION :     Initialize PWM/Timer Configuration Bits and 
 *                   Starts PWM for both Servos.

 * NOTES :           Uses Timer 2 and Pins 3 and 5 of PIC32MX795F512L.
 *                   PWM for Servo x on Pin 3 and Servo y on Pin 5
*/
void pwmInit();

/*!

 * DESCRIPTION :     Sets selected servo position. 

 * INPUTS :
 *   char            servoXY    Servo identification axis 'x' or 'y'

 *   uint16_t        pwm        Servo angular position between 0% and 100%        
*/
void setPWM(char, uint16_t);

/*!

 * DESCRIPTION :     Returns selected servo actual setpoint position. 

 * INPUTS :
 *   char            servoXY    Servo identification axis 'x' or 'y'

 * OUTPUTS :
 *   uint16_t        pwm        Servo angular position between 0% and 100%
*/
uint16_t getPWM(char);

#ifdef	__cplusplus
}
#endif
#endif
