#include "cap_touch.h"

/* NOT TESTED */

#define _XTAL_FREQ 64000000

uint32_t calibrate_cap_touch(void) {
    // calibrate the cap touch threshold
    uint32_t sum = 0;
    uint8_t count = 0;
    ANSELAbits.ANSA1 = 0;
    
    for (int i = 0; i < 100; i++) {
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
    
    // change the threshold with testing
    return (uint32_t)(sum / 100);
}