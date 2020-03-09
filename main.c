/* Project: "Bola no Plano" (MAIN)
 *
 * Authors: Diogo Vala Correia (dv.correia@ua.pt)
 *          Jose Domingues (j.domingues@ua.pt)
 *
 * Created on May 2018
 * 
 * Modules in use:
 * - Timer 2: PWM Servo Control
 * - Timer 4 + Interrupt: PID Control Loop
 * - Timer 3 + Interrupt: Analog Joystick Control Loop
 * - Timer 5 + Interrupt: Secondary Sreeen Setpoint Input
 * - Ext Interrupt 1: For SW Switching Mode
 * 
 * TODO:
 * - UART Interrupt for Setpoint (See ext_SW - setPoint = (0,0), to remove after)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/attribs.h>
#include "xc32.h"
#include "uartLib.h"
#include "adcLib.h"

#define SYSCLK  40000000L   // System clock frequency (Hz)
#define PBCLOCK 20000000L   // Peripheral Bus Clock frequency (Hz)

#define DP 100              // Decimal point multiplier
#define SAT 10              // Saturation (anti-windup)
#define plvledx 26          // Leveled Plane Servo X Value
#define plvledy 70          // Leveled Plane Servo Y Value
#define xmaxpwm 100         // Maximum PWM value for X servo
#define ymaxpwm 100         // Maximum PWM value for Y servo

// System Variables
uint16_t setPointx = 0, setPointy = 0;      // Setpoint Position
uint8_t state = 0, mode = 1;                // Aux running variables
uint8_t isTouch = 0, isTouch2 = 0;          // Is Touching Screen variables  

// Controll Variables
const uint16_t f = 10;     // Sampling Frequency (Hz)

// PID Parameters (x100)
const uint16_t Kpx = 100, Tdx = 10, Tix = 10;
const uint16_t Kpy = 100, Tdy = 10, Tiy = 10;

uint32_t a0x, a1x, a2x;
uint32_t a0y, a1y, a2y;

int32_t ex[3] = {0}, ey[3] = {0};
int32_t ux[2] = {50}, uy[2] = {50};

uint32_t posx = 0, posy = 0;
uint32_t posx2 = 0;

void initWorld();                   // Initializes all program configurations
void isr_PID(void);                 // PID Interruption Loop (uses Timer 4)
void isr_Analog(void);              // Analog Joystick Control Interrupt (uses Timer 3)
void isr_SecondTouchScreen(void);   // Second Touchscreen readings Loop
void ext_SW(void);                  // Switch External Interrupt Block
void PIDY();                        // PID X axis algorithm calculation and aplication
void PIDX();                        // PID Y axis algorithm calculation and aplication
void enableInterrupts();            // Enables board interrupts
void disableInterrupts();           // Disables board interrupts
void initPIDInterrupt();            // Initializes PID Interruption (Interrupt + Timer4)
void disablePIDInterrupt();         // Disables PID Loop Interruption
void enablePIDInterrupt();          // Enables PID Loop Interruption
void printyInfo();                  // Prints to UART1 Y axis status information
void printxInfo();                  // Prints to UART1 X axis status information
void initSWInterrupt();             // Initializes SW external interrupt
void initAnalogInterrupt();         // Initializes Analog Interruption (Interrupt + Timer3)
void disableAnalogInterrupt();      // Disables Analog Joystick Control Interrupt
void enableAnalogInterrupt();       // Enables Analog Joystick Control Interrupt
void initSecondTouchScreenInterrupt();      // Initializes Second Touchscreen readings 
void disableSecondTouchScreenInterrupt();   // Disables Second Touchscreen readings
void enableSecondTouchScreenInterrupt();    // Enables Second Touchscreen readings
void isTouching();                  // Checks if Touchscreen is being touched
void isTouching2();                 // Checks if Secondary Touchscreen is being touched

int main(int argc, char** argv) {
    initWorld();
    
    while(1);
    return (EXIT_SUCCESS);
}

void initWorld(){
    uartInit(9600, 'N', 1);
    adcInit();
    pwmInit();
    
    putString("\r\nE4 - Bola no Plano Project");
    
    // PID Coeficients Calculation (x100)
    a0x = Kpx * (DP + DP * DP/(2 * Tix * f) + Tdx * f) / DP;
    a1x = Kpx * (DP * DP/(2 * Tix * f) - 2 * Tdx * f - DP) / DP;
    a2x = Kpx * Tdx * f / DP;
    
    printf("\r\na0x = %d | a1x = %d | a2x = %d", a0x, a1x, a2x);

    a0y = Kpy * (DP + DP * DP/(2 * Tiy * f) + Tdy * f) / DP;
    a1y = Kpy * (DP * DP/(2 * Tiy * f) - 2 * Tdy * f - DP) / DP;
    a2y = Kpy * Tdy * f / DP;
    
    printf("\r\na0y = %d | a1y = %d | a2y = %d", a0y, a1y, a2y);

    // Outputs and Inputs Pins
    TRISEbits.TRISE0 = 0;   // Drive Screen 1 (output)
    TRISEbits.TRISE1 = 0;   //
    
    //initSWInterrupt();
    initPIDInterrupt();
    //initAnalogInterrupt();
    //initSecondTouchScreenInterrupt();
    
    enableInterrupts();

    //disablePIDInterrupt();
    //disableAnalogInterrupt();
    //disableSecondTouchScreenInterrupt();
    
    putString("\r\nPress Analog Switch to Start ...");
}

void __ISR(16, IPL2AUTO)  isr_PID(void){ 
    switch(state){
        case 0:
            putString("\r\nPID_FASE0");
            printxInfo();
            // Prepare to read Y axis
            PORTEbits.RE1 = 0;    
            PORTEbits.RE0 = 1;
            
            ey[2] = ey[1];
            ey[1] = ey[0];
            uy[1] = uy[0];
            state++;
            break;
        case 1:
            // Read and Controll Y axis
            putString("\r\nPID_FASE1");
            posy = readYY();
            
            if(!isTouch){
                state++;
                break;
            }
            
            PIDY();
            state++;
            break;
        case 2:
            putString("\r\nPID_FASE2");
            printyInfo();
            // Prepare to read X axis
            PORTEbits.RE0 = 0;    
            PORTEbits.RE1 = 1;
            
            ex[2] = ex[1];
            ex[1] = ex[0];
            ux[1] = ux[0]; 
            state++;
            break;
        case 3:
            // Read and Controll X axis
            putString("\r\nPID_FASE3");
            posx = readXX();
            
            isTouching();
            if(!isTouch){
                state = 0;
                break;
            }
            
            PIDX();
            state = 0;
            break;
        default: break;
    }
    IFS0bits.T4IF = 0;      // Reset interruption flag
}

void __ISR(12, IPL2AUTO) isr_Analog(void){
    if(mode == 2){
        setPointx += readAnalogX();
        if(setPointx < -105)setPointx = -105;
        if(setPointx > 105)setPointx = 105;


        setPointy += readAnalogY();
        if(setPointy < -82)setPointy = -82;
        if(setPointy > 82)setPointy = 82;
    }
    
    if(mode == 4){
        uint16_t pwmx, pwmy;
        pwmx = getPWM('x') + 5*readAnalogX();
        pwmy = getPWM('y') + 5*readAnalogY();
        setPWM('x',pwmx);
        setPWM('y',pwmy);
    }
    IFS0bits.T3IF = 0;      // Reset interruption flag
}

void __ISR(20, IPL3AUTO) isr_SecondTouchScreen(void){
    posx2 = readXX_secondary();
    isTouching2();
    
    if(isTouch2){
        setPointx = posx2; 
        setPointy = readYY_secondary();
    }else{
        setPointx = 0;
        setPointy = 0;
    }
    IFS0bits.T5IF = 0;      // Reset interruption flag
}

void __ISR(7, IPL4AUTO)  ext_SW(void){
    uint32_t j;
    for(j = 0; j < 110000;  j++) asm("nop");     // Debounce Delay
    
    switch(mode){
        case 1: 
            enablePIDInterrupt();
            disableAnalogInterrupt();
            
            putString("\r\nSet coordinate for X axis ... ");
            //setPointx = getCoordinatesXX();
            setPointx = 0;
            putString("\r\nSet coordinate for X axis ... ");
            //setPointy = getCoordinatesYY();
            setPointy = 0;
            
            putString("\r\nMODE: PID Controller Active Setpoint");
            break;
        case 2: 
            enableAnalogInterrupt();
            putString("\r\nMODE: Analog Joystick Setpoint Control PID Active");
            break;
        case 3:
            disableAnalogInterrupt();
            enableSecondTouchScreenInterrupt();
            putString("\r\nMODE: Second Touch Screen Setpoint Active");
            break;
        case 4:
            disablePIDInterrupt();
            disableSecondTouchScreenInterrupt();
            enableAnalogInterrupt();
            putString("\r\nMODE: Analog Joystick Plane Control Active");
            mode = 0;
            break;
        default: 
            mode = 0;
            break;
    }
    ++mode;
    IFS0bits.INT1IF = 0;    // Reset Interruption Flag
}

void PIDY(){
    ey[0] = setPointy - posy;
    uy[0] = uy[1] + (a0y * ey[0] + a1y * ey[1] + a2y * ey[2]) / DP;
    
    if(uy[0] > ymaxpwm) uy[0] = ymaxpwm;
    if(uy[0] < 0) uy[0] = 0;

    setPWM('y', uy[0]);
}

void PIDX(){
    ex[0] = setPointx - posx;
    ux[0] = ux[1] + (a0x * ex[0] + a1x * ex[1] + a2x * ex[2]) / DP;
    
    if(ux[0] > xmaxpwm) ux[0] = xmaxpwm;
    if(ux[0] < 0) ux[0] = 0;
   
    setPWM('x', ux[0]);
}

void enableInterrupts(){
    // INTCONbits.MVEC = 1;
    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts(); 
}

void disableInterrupts(){
    __builtin_disable_interrupts(); 
}

void initPIDInterrupt(){  
    // Uses Timer 4 as interruption trigger
    T4CONbits.ON = 0;       // Timer disable
    IPC4bits.T4IP = 2;      // Priority level 2
    IFS0bits.T4IF = 0;      // Reset Flag
    IEC0bits.T4IE = 1;      // Enable Interrupts
    T4CONbits.TCKPS = 7;    // prescaler 256
    PR4 = 1953;             // f = 40Hz
    TMR4 = 0;               // Reset timer count register
    T4CONbits.ON = 1;       // Enable Timer
}

void disablePIDInterrupt(){
    IEC0bits.T4IE = 0;      // Disable Interrupts
    T4CONbits.ON = 0;       // Disable Timer
}

void enablePIDInterrupt(){
    IEC0bits.T4IE = 1;      // Enable Interrupts
    T4CONbits.ON = 1;       // Enable Timer
}

void printyInfo(){
    putString("\r\n   y = ");
    printInt10(posy);
    putString(" | ey = ");
    printInt10(ey[0]);
    putString(" | uy = ");
    printInt10(uy[0]);
}

void printxInfo(){
    putString("\r\n   x = ");
    printInt10(posx);
    putString(" | ex = ");
    printInt10(ex[0]);
    putString(" | ux = ");
    printInt10(ux[0]);
}

void initSWInterrupt(){
    IEC0bits.INT1IE = 0;    // Disable
    INTCONbits.INT1EP = 1;  // External interrupt 3, Pin X, Rising edge = 1
    IPC1bits.INT1IP = 4;    // Priority level 4
    IEC0bits.INT1IE = 1;    // Enable
    IFS0bits.INT1IF = 0;    // Reset Flag
}

void initAnalogInterrupt(){
    // Uses Timer 3 as interruption trigger
    T3CONbits.ON = 0;       // Timer disable
    IPC3bits.T3IP = 3;      // Priority level 3
    IFS0bits.T3IF = 0;      // Reset Flag
    IEC0bits.T3IE = 1;      // Enable Interrupts
    T3CONbits.TCKPS = 7;    // prescaler 256
    PR3 = 15625;            // f = 5Hz
    TMR3 = 0;               // Reset timer count register
    T3CONbits.ON = 1;       // Enable Timer
}

void disableAnalogInterrupt(){
    IEC0bits.T3IE = 0;      // Disable Interrupts
    T3CONbits.ON = 0;       // Disable Timer
}

void enableAnalogInterrupt(){
    IEC0bits.T3IE = 1;      // Enable Interrupts
    T3CONbits.ON = 1;       // Enable Timer
}

void initSecondTouchScreenInterrupt(){
    // Uses Timer 5 as interruption trigger
    T5CONbits.ON = 0;       // Timer disable
    IPC5bits.T5IP = 3;      // Priority level 3
    IFS0bits.T5IF = 0;      // Reset Flag
    IEC0bits.T5IE = 1;      // Enable Interrupts
    T5CONbits.TCKPS = 7;    // prescaler 256
    PR5 = 39063;            // f = 2Hz
    TMR5 = 0;               // Reset timer count register
    T5CONbits.ON = 1;       // Enable Timer
}

void disableSecondTouchScreenInterrupt(){
    IEC0bits.T5IE = 0;      // Disable Interrupts
    T5CONbits.ON = 0;       // Disable Timer
}

void enableSecondTouchScreenInterrupt(){
    IEC0bits.T5IE = 1;      // Enable Interrupts
    T5CONbits.ON = 1;       // Enable Timer
}

void isTouching(){
    if(posx < -104){
        uy[0] = plvledy;
        setPWM('y', uy[0]);
        ux[0] = plvledx;
        setPWM('x', ux[0]);
        isTouch = 0;
    }
    else isTouch = 1;
    
    isTouch = 1;    // Remove, this is a production debug line 
}

void isTouching2(){
    if(posx2 < -104) isTouch2 = 0;
    else isTouch2 = 1;
}