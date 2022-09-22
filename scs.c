#include "scs.h"

void send_packet(struct Packet p) {
    for (int i = 0; i < 4; i++) {
        EUSART1_Write(p.bytes[i]);
        while (!EUSART1_is_tx_done()) {}  // wait for other side to read before sending next byte
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