#ifndef SS_H
#define	SS_H

#include <xc.h>  
#include "stdint.h"

enum Colours {
    White = 0b000,
    Red = 0b001,
    Green = 0b010,
    Blue = 0b011,
    Black = 0b100
};

struct SS {
    enum Colours sensor[5];
    uint8_t incidence;
};

#endif	/* SS_H */

