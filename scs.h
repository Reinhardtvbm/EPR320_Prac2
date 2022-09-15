#ifndef SCS_H
#define	SCS_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "mcc_generated_files/eusart1.h"

enum p_bytes {
  controlByte = 0,
  dat1 = 1,
  dat0 = 2,
  dec = 3
};

enum sys {
  idle = 0<<6,
  cal = 1<<6,
  maze = 2<<6,
  sos = 3<<6
};

enum ist {
  touch = 0<<0,
  clap = 1<<0,
  touch3 = 2<<0,
  nav = 3<<0,
};

struct Packet {
  uint8_t bytes[4];
};

struct Node {
    struct Packet p;
    struct Node* next;
};

void send_packet(struct Packet p);

struct Packet receive_packet(void);

void reset_packet(struct Packet* p);

// creates a new queue for incoming packets
struct Node create_queue(struct Packet p);

// add a new packet to the queue
void append_packet(struct Packet p, struct Node* head);

// get next packet in the queue
struct Packet get_packet(struct Node* head);

#endif	/* SCS_H */

