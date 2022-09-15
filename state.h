/* 
 * File: state.h
 * Author: Reinhardt von Brandis-Martini
 * Comments: functions to run the states in the system's state machine 
 * Revision history: 0.1
 * Date: 3 September 2022
 */
 
#ifndef STATES_H
#define	STATES_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdbool.h>

#include "ss.h"
#include "mdps.h"
#include "navcon.h"

#define FORWARD 0;
#define BACKWARD 1;
#define LEFT 2;
#define RIGHT 3;

enum States {
  Idle,
  Calibrate,
  Maze,
  Sos
};

void run_idle(enum States* state);
void run_calibrate(enum States* state);
void run_maze(enum States* state, struct MDPS* motorSystem, struct SS* sensorSystem, struct NAVCON* navcon);
void run_sos(enum States* state);

#endif	/* STATES_H */

