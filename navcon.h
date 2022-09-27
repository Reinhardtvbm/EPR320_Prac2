#ifndef NAVCON_H
#define	NAVCON_H

#include <xc.h> 
#include "stdbool.h"
#include "stdint.h"
#include "mdps.h"
#include "ss.h"

enum NavStates {
    Forward,
    Reverse,
    RotateRight,
    RotateLeft,
    Stop,
    MazeDone
};

enum SensorPosition {
    Left,
    Right,
    Unseen,
    Seen,
    CrossedLine
};

struct NAVCON {
    enum NavStates state;
    enum NavStates prev;
    enum NavStates next;
    enum SensorPosition first_red;
    bool outside_sensor;
    enum SensorPosition first_sensor_side;
    enum Colours colour;
    uint16_t reference_distance;
    uint8_t red_at_sensor;
    uint8_t blue_count;
    uint16_t AOI_correction;
};

void run_navcon(struct MDPS* motorSystem, struct SS *sensorSystem, struct NAVCON* navcon);

#endif	/* NAVCON_H */

