#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

enum sysParams {
    SYSCLK = 40000000L,   // System clock frequency (Hz)
    PBCLOCK = 20000000L   // Peripheral Bus Clock frequency (Hz)
};

char buf[100];

void uartInit(uint16_t baudrate, char parity, uint8_t stopbits){
    // Baudrate Calculation (no trunc error equation)
    U1BRG = ((PBCLOCK + 8 * baudrate)/(16 * baudrate)) - 1;
    
    U1MODEbits.BRGH = 0;	// 16x baud clock
    
    // Paraty and Data bits Configuration
    switch(parity){
        case('N') :                 // 8-bit data, no parity
            U1MODEbits.PDSEL0 = 0;
            U1MODEbits.PDSEL1 = 0;
            break;   
        case('E') :                 // 8-bit data, even parity
            U1MODEbits.PDSEL0 = 1;
            U1MODEbits.PDSEL1 = 0;
            break;   
        case('O') :                 // 8-bit data, odd parity
            U1MODEbits.PDSEL0 = 0;
            U1MODEbits.PDSEL1 = 1;
            break;	
        case('M') :                 // 9-bit data, no parity
            U1MODEbits.PDSEL0 = 1;
            U1MODEbits.PDSEL1 = 1;
            break;   
        default :                   // 8-bit data, no parity
            U1MODEbits.PDSEL0 = 0;
            U1MODEbits.PDSEL1 = 0;
            break;		
    }

    // Stop Bits Configuration
    switch(stopbits){
        case(1) : U1MODEbits.STSEL = 0;	break;		// 1 stop bit
        case(2) : U1MODEbits.STSEL = 1;	break;		// 2 stop bit
        default : U1MODEbits.STSEL = 0;	break;		// 1 stop bit 
    }
    
    // Advance Configuration Options
    U1MODEbits.SIDL = 0;    // Stop UART in IDLE Mode disabled
    U1MODEbits.IREN = 0;    // IrDA Encoder and Decoder disabled
    U1MODEbits.RTSMD = 0;   // UART1 Pin in Flow Control Mode
    U1MODEbits.UEN = 00;    // UART1 enable bits
    U1MODEbits.WAKE = 0;    // Wake-up on Start bit During Sleep Mode disabled
    U1MODEbits.LPBACK = 0;  // Loopback Mode disabled
    U1MODEbits.ABAUD = 0;   // Baud rate measurement disabled or completed
    U1MODEbits.RXINV = 0;   // Receive Polarity Inversion: Idle state is '0'

    U1STAbits.ADM_EN = 0;       // Automatic Address Detect Mode disabled
    U1STAbits.ADDR = 0x0;       // Automatic Address Mast Bits
    U1STAbits.UTXISEL = 0x0;    // TX Interrupt Mode Select bits:
                                // Interrupt is generated when the transmit 
                                // buffer contains at least one empty space
    U1STAbits.UTXINV = 0;       // IrDA encoded U1TX Idle state is ?0?
    U1STAbits.UTXBRK = 0;       // Break transmission is disabled or completed
    U1STAbits.URXISEL = 0x0;    // Interrupt flag bit is set when a character 
                                // is received
    U1STAbits.ADDEN = 0;        // Address Detect mode is disabled
    U1STAbits.OERR = 0;         // Receive Buffer Overrun Error Status bit
    
    // Activate receiver and transmitter modules 
	U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;
    
    // Enable UART1
    U1MODEbits.ON = 1;
    __XC_UART = 1;
}

void putChar(char c){
    while(U1STAbits.UTXBF);     // Wait for transmitter module ON
    U1TXREG = c;                // Put Char in register
}

char getChar(){
    if(U1STAbits.OERR == 1)         // Reset FIFO overrun flag if is ON
        U1STAbits.OERR = 0;         // Reset flag
    while(U1STAbits.URXDA == 0);    // Wait for char
    return U1RXREG;                 // Return received char
}

void putString(char *str){
	while(*str != '\0'){        // Print string char by char
		putChar(*str);          //
        str++;                  //
	}
}

void printInt10(int32_t v){
    snprintf(buf,100,"%d",v);
    putString(buf);
}

int getCoordinatesXX(){
    uint8_t count = 0, flagNegative = 0;
    int coordinate = 0, tmp;
    while(count < 3){
        tmp = getChar()-48;
        printInt10(tmp);
        if(tmp+48 == '0')putChar('0');
        
        if(count == 0 && tmp+48 == '-'){
            flagNegative = 1;
            //printf("asdasd\n");
            continue;
        }
        else{
            if(count == 0)tmp = tmp*100;
            else if(count == 1)tmp = tmp*10;
            else tmp = tmp;
        }
        coordinate = coordinate + tmp;
        count++;
    }
    
    
    if(coordinate > 105){
        putString("SetPoint out of bound\n");
        return 200;
    }
    
    if(flagNegative == 1)return (0 - coordinate) ;
    else return coordinate;
}
int getCoordinatesYY(){
    uint8_t count = 0, flagNegative = 0;
    int coordinate = 0, tmp;
    while(count < 3){
        tmp = getChar()-48;
        printInt10(tmp);
        if(tmp+48 == '0')putChar('0');
        
        if(count == 0 && tmp+48 == '-'){
            flagNegative = 1;
            //putString("asdasd\n");
            continue;
        }
        else{
            if(count == 0)tmp = tmp*100;
            else if(count == 1)tmp = tmp*10;
            else tmp = tmp;
        }
        coordinate = coordinate + tmp;
        count++;
    }
    
    if(coordinate > 82){
        putString("SetPoint out of bound\n");
        return 200;
    }
    
    if(flagNegative == 1)return (0 - coordinate) ;
    else return coordinate;
}
