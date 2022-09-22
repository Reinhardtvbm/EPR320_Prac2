#include "navcon.h"
#include "state.h"

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

void run_navcon(struct MDPS* motorSystem, struct SS *sensorSystem, struct NAVCON* navcon) {
    switch (navcon->state) {
        case Forward:
        	if ((sensorSystem->sensor[0] != White || sensorSystem->sensor[4] != White) && navcon->reference_colour == White && sensorSystem->sensor[1] == White && sensorSystem->sensor[3] == White && sensorSystem->incidence == 0) {
        			navcon->reference_distance = motorSystem->distance;

        			if (sensorSystem->sensor[0] != White) {
        				navcon->reference_colour = sensorSystem->sensor[0];

        				navcon->first_detect = Left;
        			}
        			else {
        				navcon->reference_colour = sensorSystem->sensor[4];
        				navcon->first_detect = Right;
        			}
        		}

        		if (navcon->reference_colour != White) {
        			if ((motorSystem->distance - navcon->reference_distance) > ISD) {
        				switch (navcon->reference_colour) {
        				case Green:
        					green_encounter(navcon, 50, navcon->first_detect);
        					break;
        				case Red:
        					red_encounter(navcon, 50, navcon->first_detect);
        					break;
        				case Blue:
        					blue_encounter(navcon, 50, navcon->first_detect);
        					break;
        				case Black:
        					blue_encounter(navcon, 50, navcon->first_detect);
        					break;
        				}
        				return;
        			}
        		}
            if (sensorSystem->sensor[1] != White) {
                switch (sensorSystem->sensor[1]){
                    case Green:
                        green_encounter(navcon, sensorSystem->incidence, Left);
                        break;
                    case Red:
                        red_encounter(navcon, sensorSystem->incidence, Left);
                        navcon->reference_colour = White;
                        break;
                    case Blue:
                        blue_encounter(navcon, sensorSystem->incidence, Left);
                        break;
                    case Black:
                        blue_encounter(navcon, sensorSystem->incidence, Left);
                        break;
                }
            }
            else if (sensorSystem->sensor[3] != White) {
                switch (sensorSystem->sensor[3]){
                    case Green:
                        green_encounter(navcon, sensorSystem->incidence, Right);
                        break;
                    case Red:
                        red_encounter(navcon, sensorSystem->incidence, Right);
                        navcon->reference_colour = White;
                        break;
                    case Blue:
                        blue_encounter(navcon, sensorSystem->incidence, Right);
                        break;
                    case Black:
                        blue_encounter(navcon, sensorSystem->incidence, Right);
                        break;
                }
            }
            else if (navcon->first_red == Seen) {
                if (sensorSystem->sensor[navcon->red_at_sensor] == Red) {
                    navcon->first_red = CrossedLine;
                }
            }
            if (navcon->first_red == CrossedLine) {
                if (sensorSystem->sensor[navcon->red_at_sensor] == White) {
                    navcon->state = MazeDone;
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

            navcon->reference_colour = White;
            navcon->state = Forward;

            break;
        case RotateRight:
            /* code */
            if (motorSystem->rotation < navcon->AOI_correction) {
                // if the rotation is still in progress, then keep rotating
                return;
            }

            navcon->reference_colour = White;
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
            navcon->state = RotateRight;

            break;
    }
}

