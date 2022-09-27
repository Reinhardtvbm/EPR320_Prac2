#include "scs.h"
#include "ss.h"
#include "mdps.h"
#include "navcon.h"
#include "state.h"
#include "mcc_generated_files/mcc.h"

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    
    struct SS sensorSystem;
    
    // assume initial conditions for SS are all white and 0 degree incidence
    sensorSystem.sensor[0] = White;
    sensorSystem.sensor[1] = White;
    sensorSystem.sensor[2] = White;
    sensorSystem.sensor[3] = White;
    sensorSystem.sensor[4] = White;
    sensorSystem.incidence = 0;
    
    struct MDPS motorSystem;
    
    // assume no movement for MDPS initial conditions
    motorSystem.distance = 0;
    motorSystem.left_wheel_speed = 0;
    motorSystem.right_wheel_speed = 0;
    motorSystem.level = 0;
    motorSystem.rotation = 0;
    
    struct NAVCON navcon;
    
    // first round of NAVCON with give forward with no rotation, hence:
    navcon.state = Forward;
    navcon.next = Forward;
    navcon.prev = Forward;
    navcon.red_at_sensor = 255;
    navcon.first_red = Unseen;
    navcon.colour = White;
    navcon.outside_sensor = false;
    navcon.reference_distance = 200;
    navcon.AOI_correction = 0;
    navcon.blue_count = 0;

    
    
    PIR1bits.TX1IF = 0;     // clear TX interrupt flag
    TXREG1 = 0;             // clear TX Register
    TXSTA1bits.TXEN = 1;    // enable transmit
    // set startup state to idle  
    enum States state = Idle;
    
    while (1){
        // here I pass in a pointer to the state (&state), which each function can then manipulate.
        // After the function has finished its processing, it will return to this switch and run the 
        // next state (which gets set by the previous function)
        // =============================================================================
        // e.g. run_idle() sets state to Calibrate, then run_calibrate() will run and set
        // the state to Maze. Then run_maze() will set the state to SOS if a clap is 
        // detected or Idle if a touch is detected/the maze is done

        switch (state) {
            case Idle: {run_idle(&state);  break;}
            case Calibrate: {run_calibrate(&state);  break;}
            case Maze: {run_maze(&state, &motorSystem, &sensorSystem, &navcon);  break;}
            case Sos: {run_sos(&state);  break;}
        }
    }
}
/**
 End of File
*/