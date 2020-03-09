
#include <xc.h>
#include "adcLib.h"
#include "uartLib.h"
#include <stdio.h>


 
void delay_ms(uint32_t delayTime){

    int j, final;
    
    final = delayTime  * 550;   //550 was a calibrated value in order for the function to delay 1ms * delay_time
    for(j = 0; j < final; j++){
        asm("nop");
    }
}   

void adcInit(){
    
	//Input pins

    TRISBbits.TRISB0 = 1;       //RB0 as input
    TRISBbits.TRISB1 = 1;       //RB1 as input
    
    AD1PCFGbits.PCFG1 = 0;      //RB0 as analog input
    AD1PCFGbits.PCFG0 = 0;      //RB1 as analog input
    
    
    //Voltage reference                
    AD1CON2bits.VCFG = 0;       //AVDD and AVSS
    
    //Number of samples
	AD1CON2bits.SMPI = 0;		// 6 sample will be converted and store in buffer ADC1BUFx
    
    
	AD1CON1bits.SSRC = 7;		// Internal counter ends sampling and starts conversion 

	AD1CON1bits.CLRASAM = 1;	// Stop conversions when the first ADC interrupt is generated
    
    
    
    //Time
    AD1CON3bits.ADCS = 0;       // TAD = 2 * TPB(=1/20000000) = 100ns
    
    //Acquisition time
	AD1CON3bits.SAMC = 20;		// Acquisition time is 20 TAD
    //Conversion takes 12 Tad

    
	AD1CON1bits.ON = 1;			// Enable A/D converter
}


int readAnalogX(){
    
    
        
    
    // ADC conversion
    AD1CHSbits.CH0SA = 3;           // Selects AN1 as input for the A/D converter
    AD1CON1bits.SAMP = 1;           // Start conversion
    while(AD1CON1bits.DONE==0);     // Wait until Conversion is done
    AD1CON1bits.DONE=0;             // Reset DONE Flag by software
    
    
    
    
    if(ADC1BUF0 < 400)return -1;
    else if(ADC1BUF0 > 700)return 1;
    else return 0;
    
}

int readAnalogY(){
    
    
    
    
    // ADC conversion
    AD1CHSbits.CH0SA = 2;           // Selects AN1 as input for the A/D converter
    AD1CON1bits.SAMP = 1;           // Start conversion
    while(AD1CON1bits.DONE==0);     // Wait until Conversion is done
    AD1CON1bits.DONE=0;             // Reset DONE Flag by software
    
    if(ADC1BUF0 < 400)return -1;
    else if(ADC1BUF0 > 700)return 1;
    else return 0;
    
}



int readVoltageXX(){
    
    
    PORTEbits.RE1 = 0;    
    PORTEbits.RE0 = 1;              //DRIVE Pin set to XX
    
   // delay_ms(delayADC);             //Wait for the circuit to stabilize
    
    // ADC conversion
    AD1CHSbits.CH0SA = 0;           // Selects AN1 as input for the A/D converter
    AD1CON1bits.SAMP = 1;           // Start conversion
    while(AD1CON1bits.DONE==0);     // Wait until Conversion is done
    AD1CON1bits.DONE=0;             // Reset DONE Flag by software
    
    
    
    int value = (ADC1BUF0*330)/1023; // Return Buffer Value 

    
    
    return value;
}
int readVoltageYY(){
    
    PORTEbits.RE0 = 0;    
    PORTEbits.RE1 = 1;              //DRIVE Pin set to XX
    
    delay_ms(delayADC);             //Wait for the circuit to stabilize
    
    // ADC conversion
    AD1CHSbits.CH0SA = 1;       // Selects AN1 as input for the A/D converter
     AD1CON1bits.SAMP = 1;      // Start conversion
    while(AD1CON1bits.DONE==0); // Wait until Conversion is done
    AD1CON1bits.DONE=0;         // Reset DONE Flag by software
    
    
    
    
    int value = (ADC1BUF0*330)/1023; // Return Buffer Value 
    
    return value;
    
}
int readXX(){
    
    
    //PORTEbits.RE0 = 0;    
    //PORTEbits.RE1 = 1;              //DRIVE Pin set to XX
    
    delay_ms(delayADC);             //Wait for the circuit to stabilize
    
    // ADC conversion
    AD1CHSbits.CH0SA = 0;           // Selects AN1 as input for the A/D converter
    AD1CON1bits.SAMP = 1;           // Start conversion
    while(AD1CON1bits.DONE==0);     // Wait until Conversion is done
    AD1CON1bits.DONE=0;             // Reset DONE Flag by software
  
    
    
    int value = ADC1BUF0; // Buffer Value 
    
    int distance = (value * 260 - 117650)/1000; // Return distance value
    
    // Linearization to get XX cordinates
    //int distance = (value * 283 - 112730)/1000; // Return distance value
    //int distance = (value * 236 - 110810)/1000; // Return distance value
    if(distance < -105)distance = -105;
    else if(distance > 105) distance = 105;
    
    //printf("x = %d\n",distance);
    //putString("x = ");
    //printInt10(distance);
    
    return distance;
}
int readYY(){
    
    //PORTEbits.RE1 = 0;    
    //PORTEbits.RE0 = 1;              //DRIVE Pin set to YY
    
    delay_ms(delayADC);             // Wait for the circuit to stabilize
    
    // ADC conversion
    AD1CHSbits.CH0SA = 1;           // Selects AN1 as input for the A/D converter
    AD1CON1bits.SAMP = 1;           // Start conversion
    while(AD1CON1bits.DONE==0);     // Wait until Conversion is done
    AD1CON1bits.DONE=0;             // Reset DONE Flag by software
    
    
    int value = ADC1BUF0;           // Buffer Value 
    
    // Linearization to get YY cordinates
    
    int distance = (value * 227 - 105630)/1000; // Return distance value
    
    //int distance = (value * 248 - 101570)/1000; // Return distance value
    
   // int distance = (value * 193 - 93289)/1000; // Return distance value
    if(distance < -82)distance = -82;
    else if(distance > 82) distance = 82;
    
    
    
    
    return distance;
}

int readXX_secondary(){
    
    
    PORTEbits.RE2 = 0;    
    PORTEbits.RE3 = 1;              //DRIVE Pin set to XX
    
    delay_ms(delayADC);             //Wait for the circuit to stabilize
    
    // ADC conversion
    AD1CHSbits.CH0SA = 0;           // Selects AN1 as input for the A/D converter
    AD1CON1bits.SAMP = 1;           // Start conversion
    while(AD1CON1bits.DONE==0);     // Wait until Conversion is done
    AD1CON1bits.DONE=0;             // Reset DONE Flag by software
  
    
    
    int value = ADC1BUF0; // Buffer Value 
    
    
    // Linearization to get XX cordinates
    int distance = (value * 283 - 112730)/1000; // Return distance value
    if(distance < -105)distance = -105;
    else if(distance > 105) distance = 105;
    
    //printf("x = %d\n",distance);
    //putString("x = ");
    //printInt10(distance);
    
    return distance;
}
int readYY_secondary(){
    
    PORTEbits.RE3 = 0;    
    PORTEbits.RE2 = 1;              //DRIVE Pin set to YY
    
    delay_ms(delayADC);             // Wait for the circuit to stabilize
    
    // ADC conversion
    AD1CHSbits.CH0SA = 1;           // Selects AN1 as input for the A/D converter
    AD1CON1bits.SAMP = 1;           // Start conversion
    while(AD1CON1bits.DONE==0);     // Wait until Conversion is done
    AD1CON1bits.DONE=0;             // Reset DONE Flag by software
    
    
    int value = ADC1BUF0;           // Buffer Value 
    
    // Linearization to get YY cordinates
    int distance = (value * 248 - 101570)/1000; // Return distance value
    if(distance < -82)distance = -82;
    else if(distance > 82) distance = 82;
    
    
    
    
    return distance;
}

