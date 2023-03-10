#include "Sim_Engine.h"

#define A 0
#define B 1
#define Q_MAXSIZE 100
struct pkt packetA;
struct msg queue[Q_MAXSIZE];
float waitingTime;
bool sending;
int q_size = -1;

int checkSumA(struct msg message) {
  int sum = 0;
  for(int i = 0; i < strlen(message.data); i++) {
    sum += message.data[i];
  }
  return sum;
}

void push(struct msg message) {
  queue[++q_size] = message;
  return;
}

struct msg pop() {
  struct msg pop = queue[0];
  for(int i = 0; i < Q_MAXSIZE; i++) {
    if(i < Q_MAXSIZE - 1) {
      queue[i] = queue[i+1];
    }
  }
  q_size--;
  return pop;
}

/* Called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message) {
  if(sending) {
    push(message);
    //printf("SKIPPED, Q_SIZE: %d\n", q_size);
    return;
  }
  printf("A_output: %s\n", message.data);

  memcpy(packetA.payload, message.data, sizeof(message.data));
  packetA.checksum = checkSumA(message);
  if(packetA.seqnum == 1) {
    packetA.seqnum = 0;
  } else {
    packetA.seqnum = 1;
  }
  printf("SEQ %d\n", packetA.seqnum);

  sending = true;
  tolayer3(A, packetA);
  starttimer(A, waitingTime);
}

/* Called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packetB) {
  printf("A_input: ACK %d\n", packetB.acknum);
  stoptimer(A);
  if(packetB.acknum != packetA.seqnum) {
    printf("A_input: Corruption occured. %s\n", packetA.payload);
    tolayer3(A, packetA);
    starttimer(A, waitingTime);
  } else {
    sending = false;
    if(q_size != -1) {
      A_output(pop());
    }
  }
}

/* Called when A's timer goes off */
void A_timerinterrupt() {
  printf("TIMEOUT! Resending packet %d\n", packetA.seqnum);
  tolayer3(A, packetA);
  starttimer(A, waitingTime);
}  

/* The following routine will be called once (only) before any other */
/* Host A routines are called. You can use it to do any initialization */
void A_init() {
  packetA.acknum = 0;
  packetA.checksum = 0;
  packetA.seqnum = 1;
  waitingTime = 100.0;
  sending = false;
}
