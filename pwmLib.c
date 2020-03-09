#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <xc.h>
// Init PWM values (centered)
static uint16_t pwmX = 50, pwmY = 50;
void pwmInit() {   
    // Timer Configuration
    // Timer 2 Selected
    T2CONbits.ON = 0;       // Timer disable
    T2CONbits.TCKPS = 5;    // 1:32 prescale value
    T2CONbits.T32 = 0;      // TMRx and TMRy form a 32-bit timer
    T2CONbits.TCS = 0;      // Internal peripheral clock
    
    PR2 = 12500;            // Load period register
    TMR2 = 0;               // Reset timer count register
    
    // PWM Setup Servo 1
    OC1CONbits.OCM = 6;     // PWM mode on OC1; Fault pin disabled
    OC1CONbits.OCTSEL = 0;  // Timer2 is the clock source
    
    OC1RS = (93758 * pwmX + 4690000)/10000; // Ton Constant (init centered)
    OC1CONbits.ON = 1;      // Output Compare peripheral enable
    
    // PWM Setup Servo 2
    OC2CONbits.OCM = 6;     // PWM mode on OC2 Fault pin disabled
    OC2CONbits.OCTSEL = 0;  // Timer2 is the clock source
    
    OC2RS = (93758 * pwmY + 4690000)/10000; // Ton Constant (init centered)
    OC2CONbits.ON = 1;      // Output Compare peripheral enable
    
    // Start PWM generation
    T2CONbits.TON = 1;      // Enable timer    
}
void setPWM(char servoXY, uint16_t pwm) {
    if(pwm > 100) pwm = 100;
    if(pwm < 0) pwm = 0;
    
    switch(servoXY){
        case 'x': 
            if(pwm > 70) pwm = 70;
            pwmX = pwm;
            OC1RS = (93758 * pwm + 4690000)/10000; 
            break;
        case 'y': 
            pwmY = pwm;
            OC2RS = (93758 * pwm + 4690000)/10000; 
            break;
        default: break;
    }
}
uint16_t getPWM(char servoXY) {
    switch(servoXY){
        case 'x': 
            return pwmX;
            break;
        case 'y': 
            return pwmY;
            break;
        default: return 0;
    }
}
