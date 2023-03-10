#include "Sim_Engine.h"

#define A 0
#define B 1
int prevSeq;

bool isCorrupted(struct pkt packet) {
  int sum = 0;
  for(int i = 0; i < strlen(packet.payload); i++) {
    sum += packet.payload[i];
  }

  if(sum != packet.checksum) {
    return true;
  }
  return false;
}

/* Called from layer 5, passed the data to be sent to other side */
void B_output( struct msg message) {
  /* DON'T IMPLEMENT */
}

/* Called from layer 3, when a packet arrives for layer 4 */
void B_input(struct pkt packet) {
  printf("B_input: %s\n", packet.payload);
  if(packet.seqnum == 0) {
    if(isCorrupted(packet)) {
      packet.acknum = 1;
      printf("CORRUPTED! Send ACK %d.\n", packet.acknum);
      tolayer3(B, packet);
    } else {
      packet.acknum = 0;
      if(packet.seqnum == prevSeq) {
        printf("Duplicate\n");
      } else {
        tolayer5(B, packet.payload);
        prevSeq = packet.seqnum;
      }
      tolayer3(B, packet);
    }
  } else {
    if(isCorrupted(packet)) {
      packet.acknum = 0;
      printf("CORRUPTED! Send ACK %d.\n", packet.acknum);
      tolayer3(B, packet);
    } else {
      packet.acknum = 1;
      if(packet.seqnum == prevSeq) {
        printf("Duplicate\n");
      } else {
        tolayer5(B, packet.payload);
        prevSeq = packet.seqnum;
      }
      tolayer3(B, packet);
    }
  }
}

/* Called when B's timer goes off */
void B_timerinterrupt() {
  /* TODO */
}  

/* The following routine will be called once (only) before any other */
/* Host B routines are called. You can use it to do any initialization */
void B_init() {
  prevSeq = 1;
}
