#include "state.h"
#include "external.h"
#include "scs.h"
#include "navcon.h"
#include "stdlib.h"

// ******** maybe put NAVCON into a new file for simplicity's sake.... ********

void run_idle(enum States* state) {
    struct Packet packet_out;
    reset_packet(&packet_out);
    // set SYS<1:0> to idle
    packet_out.bytes[0] = 16;
    packet_out.bytes[1] = 0;
    packet_out.bytes[2] = 0;
    packet_out.bytes[3] = 0;
    
    send_packet(packet_out);

    while (run_touch() == false) {
        send_packet(packet_out);
    }

    // now send that there was a touch :)
    packet_out.bytes[dat1] = 1;
    send_packet(packet_out);   

    // head to calibrate state!
    reset_packet(&packet_out);
    *state = Calibrate;  
}

void run_calibrate(enum States* state){
    // set SYS<1:0> to calibrate
    struct Packet packet_in;
    struct Packet packet_out;
    reset_packet(&packet_out);
    packet_out.bytes[controlByte] = 80;
    
    
    // wait for initial packets from SS & MDPS
    while (packet_in.bytes[controlByte] != 113){
        packet_in = receive_packet();
    }

    send_packet(packet_out);
    
    while (run_touch() == false) {
        packet_in = receive_packet();
        // do stuff with MDPS data
        packet_in = receive_packet();
        // do stuff with SS data

        // send that I am waiting for a touch :/
        send_packet(packet_out);
    }

    // we got a touch :D
    packet_out.bytes[dat1] = 1;
    send_packet(packet_out);

    // head to maze state!
    reset_packet(&packet_out);
    *state = Maze;
}

void run_maze(enum States* state, struct MDPS* motorSystem, struct SS* sensorSystem, struct NAVCON* navcon){
    struct Packet packet_in;
    struct Packet packet_out;
    
    reset_packet(&packet_out);
    packet_out.bytes[controlByte] = 145;
    // tell the other's where I'm at
    send_packet(packet_out);

    if (run_clap() == true) {
        // oh nooo! SOS!
        *state = Sos;
        return;
    }

    reset_packet(&packet_out);
    packet_out.bytes[controlByte] = 146;
    send_packet(packet_out);
    
    if (run_touch() == false) {
        // go back to idle ;-;
        *state = Idle;
        return;
    }
    
    // execute NAVCON :D
    run_navcon(motorSystem, sensorSystem, navcon);
    
    // we are in NAVCON state bois
    reset_packet(&packet_out);
    packet_out.bytes[controlByte] = 147;
    
    switch (navcon-> state) {
        case Forward: {
            packet_out.bytes[dec] = 0;
            packet_out.bytes[dat0] = 10;
            packet_out.bytes[dat1] = 10;
            
            break;
        }
        case Reverse: {
            packet_out.bytes[dec] = 1;
            packet_out.bytes[dat0] = 10;
            packet_out.bytes[dat1] = 10;
            
            break;
        }
        case RotateLeft: {
            packet_out.bytes[dec] = 2;
            packet_out.bytes[dat1] = (uint8_t)((navcon->AOI_correction & 0b1111111100000000) >> 8);
            packet_out.bytes[dat0] = (uint8_t)(navcon->AOI_correction & 0b0000000011111111);
            
            break;
        }
        case RotateRight: {
            packet_out.bytes[dec] = 3;
            packet_out.bytes[dat1] = (uint8_t)((navcon->AOI_correction & 0b1111111100000000) >> 8);
            packet_out.bytes[dat0] = (uint8_t)(navcon->AOI_correction & 0b0000000011111111);
            
            break;
        }
        case Stop: {
            /* no need to change the packet*/
        }
    }
    
    send_packet(packet_out);
    
    struct Packet packets[6];
    
    while (packet_in.bytes[controlByte] != 161) {
        packet_in = receive_packet();
    }
    
    packets[0] = packet_in;
    
    while (packet_in.bytes[controlByte] != 162) {
        packet_in = receive_packet();
    }
    
    packets[1] = packet_in;
    
    while (packet_in.bytes[controlByte] != 163) {
        packet_in = receive_packet();
    }
    
    packets[2] = packet_in;
    
    while (packet_in.bytes[controlByte] != 164) {
        packet_in = receive_packet();
    }
    
    packets[3] = packet_in;
    
    while (packet_in.bytes[controlByte] != 177) {
        packet_in = receive_packet();
    }
    
    packets[4] = packet_in;
    
    while (packet_in.bytes[controlByte] != 178) {
        packet_in = receive_packet();
    }
    
    packets[5] = packet_in;
    
    
    // MDPS Level
    motorSystem->level = packets[0].bytes[dat1];

    // MDPS Rotation
    motorSystem->rotation = (uint16_t)((packets[1].bytes[dat1] << 8) + (packets[1].bytes[dat0]));

    // MDPS Speed
    motorSystem->right_wheel_speed = packets[2].bytes[dat1];
    motorSystem->left_wheel_speed = packets[2].bytes[dat0];
    
    // MDPS Distance
    motorSystem->distance = (uint16_t)((packets[3].bytes[dat1] << 8) + (packets[3].bytes[dat0])); 

    // SS Colours OR End-Of-Maze
    
    if (packets[1].bytes[controlByte] == 179) {
        // end-of-maze
        *state = Idle;
        return;
    }
    
    uint16_t colours = ((uint16_t)((packets[4].bytes[dat1] << 8) + packets[4].bytes[dat0]));
    
    // beautiful bit shifting
    sensorSystem->sensor[0] = (enum Colours)((colours & 0b0111000000000000) >> 12);
    sensorSystem->sensor[1] = (enum Colours)((colours & 0b0000111000000000) >> 9);
    sensorSystem->sensor[2] = (enum Colours)((colours & 0b0000000111000000) >> 6);
    sensorSystem->sensor[3] = (enum Colours)((colours & 0b0000000000111000) >> 3);
    sensorSystem->sensor[4] = (enum Colours)((colours & 0b0000000000000111) >> 0);
    
    
    // SS Incidence
    sensorSystem->incidence = packets[5].bytes[dat1];
}

void run_sos(enum States* state){
    
}