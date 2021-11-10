// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "serial.h"
#include "LCD.h"
#include "ADC.h"
#include "interrupts.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void) {
    LCD_Init();
    initUSART4(); 
    ADC_init();
    Interrupts_init();
    
    // setup pin for input (connected to left button)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin 

    char string[20];
    
    while (1) {
        // Need to discern between receiving and transmitting
        // Receiving
        while (isDataInRxBuf()==1) {LCD_sendbyte(getCharFromRxBuf(),1);}
                
        // Transmit Voltage for each button press
        unsigned int ADC_val = ADC_getval();
        int int_part;
        int frac_part;
        int_part = ADC_val/77; // i.e. 255 / 5V = 77.2727
        frac_part = (ADC_val*100)/77 - int_part*100;
        sprintf(string,"%d.%02dV",int_part,frac_part);
        
        // If button pressed
        if (!PORTFbits.RF2) {
            TxBufferedString(string);
            sendTxBuf();
            __delay_ms(200);
        }
    }
}

