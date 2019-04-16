#ifndef _queue_h
#define _queue_h

#include "headfile.h"
#include <string.h>

void initQueue(queue *q);
uint16 getQueueLen(queue *q);
uint8 is_empty(queue *q);
uint8 is_full(queue *q);
uint8 enQueue(queue *q, TcpData tcp_data);
uint8 deQueue(queue *q, TcpData *tcp_data);

#endif