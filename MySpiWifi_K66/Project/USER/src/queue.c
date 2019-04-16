#include "queue.h"

void initQueue(queue *q)
{
  q->front = q->rear = 0;
  for(uint16 i = 0; i < TCPBUFF_CAP; i++)
    for(uint16 j = 0; j < TCPBUFF_LENGTH; j++)
      q->tcp_data[i].data[j] = 0;
}

uint16 getQueueLen(queue *q)
{
  return (q->rear - q->front + TCPBUFF_CAP) % TCPBUFF_CAP;
}

uint8 is_empty(queue *q)
{
  return (q->front == q->rear);
}

uint8 is_full(queue *q)
{
  return (q->front == ((q->rear + 1) % TCPBUFF_CAP));
}

uint8 enQueue(queue *q, TcpData tcp_data)
{
  if(is_full(q))
  {
    return 0;
  }
  memcpy(q->tcp_data[q->rear].data, tcp_data.data, TCPBUFF_LENGTH);
  q->rear = (q->rear + 1) % TCPBUFF_CAP;
  return 1;
}

uint8 deQueue(queue *q, TcpData *tcp_data)
{
  if(is_empty(q))
  {
    return 0;
  }
  memcpy(tcp_data->data, q->tcp_data[q->front].data, TCPBUFF_LENGTH);
  q->front = (q->front + 1) % TCPBUFF_CAP;
  return 1;
}

/*
uint8 traverseQueue(void)
{
  int index = mq.front;
  while(index != mq.rear)
  {
    //send()mq->msg[index];
    index = (index + 1) % TCPBUFF_CAP;
  }
}*/
