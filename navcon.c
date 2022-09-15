#include "navcon.h"

void run_navcon(struct MDPS* motorSystem, struct SS *sensorSystem, struct NAVCON* navcon) {
    uint8_t white_count = 0;
    uint8_t green_count = 0;
    uint8_t blue_count = 0;
    uint8_t red_count = 0;

    // loop through the sensor values to count the number of sensors that see each colour
    for (int i = 0; i < 5; i++) {
        switch (sensorSystem->sensor[i]) {
            case White: {white_count++; break;}
            case Red: {                
                case (navcon->first_red) {
                    Unseen:{
                        if (i < 3) {
                            navcon->first_red = Left;
                        }
                        else {
                            navcon->first_red = Right;
                        }
                    break;}

                    Right:{
                        if (i == 0) {
                            navcon->state = MazeDone;
                            return;
                        }

                        break;
                    }

                    Left:{
                        if (i == 4) {
                            navcon->state = MazeDone;
                            return;
                        }

                        break;
                    }
                }
                     
                break;
            }
            case Green: {green_count++; break;}
            case Blue: {blue_count++; break;}
            case Black: {blue_count++; break;}
        }
    }
    
    switch (navcon->state){
        /* what to do if we are busy going FORWARD */
        case Forward:
            if (white_count == 5) {
                // if all white, continue going forward
                return;
            }
            else {
                if (blue_count == 0 && sensorSystem->incidence <= 5) {

                    return;
                }
                
                navcon->state = Stop;
                navcon->prev = Forward;
                
                // then we can start doing correction if necessary
                if (blue_count == 0) {
                    // NAVCON QTP 1: traverse green/red at AOI <= 5 with correction for 5 < AOI < 45
                    // NAVCON QTP 2: green/red at AOI >= 45
                        
                    if (sensorSystem->incidence < 45) {
                        // the rotation required for an AOI less than 45 will simply be the incidence
                        navcon->AOI_correction = (uint16_t)sensorSystem->incidence;
                    }
                    else {
                        // but if it is more than 45, for some reason 5 degree increments have to be used :/
                        navcon->AOI_correction = 5; 
                    }
                    
                    if (sensorSystem->sensor[0] == White) {
                            navcon->next = RotateRight;
                    }
                    else {
                        navcon->next = RotateLeft;
                    }
                }
                else if (green_count == 0) {
                    // QTP 3 & 4
                    if (sensorSystem->incidence < 45) {
                        // the rotation required for an AOI less than 45 will simply be the incidence
                        navcon->AOI_correction = (uint16_t)sensorSystem->incidence;
                    }
                    else {
                        // but if it is more than 45, for some reason 5 degree increments have to be used :/
                        navcon->AOI_correction = 5; 
                    }
                    
                    if (sensorSystem->sensor[0] == White) {
                        if (navcon->AOI_correction != 5){
                            navcon->AOI_correction += 90;
                        }
                        
                        navcon->next = RotateRight;
                    }
                    else {
                        if (navcon->AOI_correction != 5){
                            navcon->AOI_correction = 90 - navcon->AOI_correction;
                        }
                        
                        navcon->next = RotateLeft;
                    }
                }
            }
            break;
        case Reverse:
            /* code */
            // until some distance covered, keep reversing....
            if (motorSystem->distance < 5) {
                return;
            }
            
            navcon->prev = Reverse;
            navcon->state = Stop;
            
            break;
        case RotateLeft:
            /* code */
            if (motorSystem->rotation < navcon->AOI_correction) {
                // if the rotation is still in progress, then keep rotating
                return;
            }
            
            navcon->state = Forward;
            
            break;
        case RotateRight:
            /* code */
            if (motorSystem->rotation < navcon->AOI_correction) {
                // if the rotation is still in progress, then keep rotating
                return;
            }
            
            navcon->state = Forward;
            
            break;
        case Stop: 
            /* code */
            if (navcon->prev == Forward) {
                navcon->state = Reverse;
                return;
            }
            
            navcon->state = navcon->next;
            
            break;
        case MazeDone:
            /* code */
            navcon->AOI_correction = 360;
            nacon->state = RotateRight;

            break;
    }
}