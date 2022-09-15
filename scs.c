#include "scs.h"

void send_packet(struct Packet p) {
    for (int i = 0; i < 4; i++) {
        EUSART1_Write(p.bytes[i]);
        while (!EUSART1_is_tx_done()) {}
    }
}

struct Packet receive_packet(void) {
    struct Packet p;
    
    for (int i = 0; i < 4; i++) {
        p.bytes[i] = EUSART1_Read();
    }
    
    return p;
}

void reset_packet(struct Packet* p) {
  p->bytes[controlByte] = nav;
  p->bytes[dat1] = 0;
  p->bytes[dat0] = 0;
  p->bytes[dec] = 0;
}

// creates a new queue for incoming packets
struct Node create_queue(struct Packet p) {
    struct Node new_node;
    
    new_node.p = p;
    new_node.next = NULL;
    
    return new_node;
}

// add a new packet to the queue
void append_packet(struct Packet p, struct Node* head) {
    struct Node* traverse = head;
    
    while (traverse->next != NULL) {
        traverse = traverse->next;
    }
    
    struct Node new_node;
    
    new_node.p = p;
    new_node.next = NULL;
    
    traverse->next = &new_node;
}

// get next packet in the queue
struct Packet get_packet(struct Node* head) {
    struct Node* next_node = head;
    struct Packet next_packet = next_node->p;
    
    head = head->next;
    
    next_node = NULL;
    next_node->next = NULL;
    
    return next_packet;
}