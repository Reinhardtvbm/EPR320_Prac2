#include "external.h"
#define NO_SAMPLES 3
#define _XTAL_FREQ 64000000
/* NOT TESTED */

bool run_touch(uint32_t threshold, uint32_t* counts) {
    uint32_t sum = 0;
    uint8_t count = 0;
    ANSELAbits.ANSA1 = 0;
    
    for (int i = 0; i < NO_SAMPLES; i++) {
        // set RA1 to output
        TRISAbits.TRISA1 = 0;
        
        // set RA1 to 1 and let cap charge
        PORTAbits.RA1 = 1;
        __delay_ms(1);
        
        // set RA1 to input
        TRISAbits.TRISA1 = 1;
        
        // while the cap still discharging count 
        while (PORTAbits.RA1 == 1) {
            count += 1;
        }
        
        // if a new max is found then set max to that value
        sum += count;
        
        count = 0;
    }
    uint32_t threshold_and_some = threshold+10;
    // change the threshold with testing
    counts = sum / NO_SAMPLES;
    return (sum / NO_SAMPLES) > threshold_and_some;
}

bool run_clap(void) {
    return false;
}
