#include "navcon.h"
#include "state.h"
#include "scs.h"

#define ISD 10

void green_encounter(struct NAVCON* navcon, uint8_t incidence, enum SensorPosition position) {
    if (incidence <= 5) {
        navcon->state = Forward;
        return;
    }
    else if (incidence < 45) {
        navcon->AOI_correction = (uint16_t)incidence;
    }
    else {
        navcon->AOI_correction = 5;
    }

    navcon->prev = Forward;
    navcon->state = Stop;

    switch (position) {
        case Left:
            navcon->next = RotateLeft;
            break;
        case Right:
            navcon->next = RotateRight;
            break;
    }
}

void red_encounter(struct NAVCON* navcon, uint8_t incidence, enum SensorPosition position) {
    if (incidence <= 5) {
        if (navcon->first_red == Unseen) {
            switch (position) {
                case Left:
                    navcon->red_at_sensor = 4;
                    break;
                case Right:
                    navcon->red_at_sensor = 0;
                    break;
            }

            navcon->first_red = Seen;
        }
    }
    else {
        green_encounter(navcon, incidence, position);
    }
}

void blue_encounter(struct NAVCON* navcon, uint8_t incidence, enum SensorPosition position) {
    if (incidence <= 5) {
        navcon->AOI_correction = 0;
    }
    else if (incidence < 45) {
        navcon->AOI_correction = (uint16_t)incidence;
    }
    else {
        navcon->AOI_correction = 5;
    }

    navcon->prev = Forward;
    navcon->state = Stop;


    switch (position) {
        case Left:
            navcon->next = RotateRight;
            navcon->AOI_correction = 90 - navcon->AOI_correction;
            break;
        case Right:
            navcon->next = RotateRight;
            navcon->AOI_correction += 90;
            break;
    }
}

void greater_than_45(struct NAVCON* navcon, enum SensorPosition side){
    switch (navcon->colour) {
        case Green:
            green_encounter(navcon, 50, side);
            break;
        case Red:
            red_encounter(navcon, 50, side);
            break;
        default:
            blue_encounter(navcon, 50, side);
            break;
    }
}

void less_than_45(struct NAVCON* navcon, enum SensorPosition side, uint8_t incidence) {
//    struct Packet in_packet = {{1,1,1,1}};
//    send_packet(in_packet);
    switch (navcon->colour){
        case Green:
            green_encounter(navcon, incidence, side);
            break;
        case Red:
            red_encounter(navcon, incidence, side);
            break;
        default:
            blue_encounter(navcon, incidence, side);
            break;
    }
}

void run_navcon(struct MDPS* motor_system, struct SS* sensor_system, struct NAVCON* navcon) {
    switch (navcon->state) {
        case Forward:
        	if (navcon->outside_sensor == false && (sensor_system->sensor[0] != White || sensor_system->sensor[4] != White) && sensor_system->incidence == 0) {
                navcon->outside_sensor = true;
                navcon->reference_distance = motor_system->distance;
                if (sensor_system->sensor[0] != White) {
                    navcon->colour = sensor_system->sensor[0];
                    navcon->first_sensor_side = Left;
                }
                else {
                    navcon->colour = sensor_system->sensor[4];
                    navcon->first_sensor_side = Right;
                }
            }

            if (navcon->outside_sensor == true) {
                if ((motor_system->distance - navcon->reference_distance) > ISD) {
                    greater_than_45(navcon, navcon->first_sensor_side);
                    navcon->colour = White;
                    navcon->outside_sensor = false;
                }
                else if (sensor_system->sensor[1] != White || sensor_system->sensor[3] != White){
                    enum SensorPosition side;
                    if (sensor_system->sensor[1] != White) {
                        navcon->colour = sensor_system->sensor[1];
                        side = Left;
                    }
                    else if (sensor_system->sensor[3] != White) {
                        navcon->colour = sensor_system->sensor[3];
                        side = Right;
                    }
                    less_than_45(navcon, side, sensor_system->incidence);
                    navcon->colour = White;
                    navcon->outside_sensor = false;
                }
                else if (navcon->first_red == Seen) {
                    if (sensor_system->sensor[navcon->red_at_sensor] == Red) {
                        navcon->first_red = CrossedLine;
                    }
                    
                    if (navcon->first_red == CrossedLine) {
                        if (sensor_system->sensor[navcon->red_at_sensor] == White) {
                            navcon->state = MazeDone;
                        }
                    } 
                }
                  
            }
            break;
        case Reverse:
            // until some distance covered, keep reversing....
            if (motor_system->distance < ISD+10) {
                return;
            }

            navcon->prev = Reverse;
            navcon->state = Stop;

            break;
        case RotateLeft:
            if (motor_system->rotation < navcon->AOI_correction) {
                // if the rotation is still in progress, then keep rotating
                return;
            }

            navcon->colour = White;
            navcon->state = Forward;

            break;
        case RotateRight:
            if (motor_system->rotation < navcon->AOI_correction) {
                // if the rotation is still in progress, then keep rotating
                return;
            }

            navcon->colour = White;
            navcon->state = Forward;

            break;
        case Stop:
            if (navcon->prev == Forward) {
                navcon->state = Reverse;
                return;
            }

            navcon->state = navcon->next;

            break;
        case MazeDone:
            navcon->AOI_correction = 360;
            navcon->state = RotateRight;

            break;
    }
}