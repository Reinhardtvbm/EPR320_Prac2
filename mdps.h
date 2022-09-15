#ifndef MDPS_H
#define	MDPS_H

#include <xc.h> 
#include "stdint.h"

struct MDPS {
    uint8_t level;
    uint16_t rotation;
    uint8_t left_wheel_speed;
    uint8_t right_wheel_speed;
    uint16_t distance;
};

#endif	/* MDPS_H */

