#ifndef NAVCON_H
#define	NAVCON_H

#include <xc.h> 
#include "stdbool.h"
#include "stdint.h"

enum NavStates {
    Forward,
    Reverse,
    RotateRight,
    RotateLeft,
    Stop
};

struct NAVCON {
    enum NavStates state;
    enum NavStates prev;
    enum NavStates next;
    bool stopped;
    uint8_t blue_count;
    uint16_t AOI_correction;
};

#endif	/* NAVCON_H */

