#include "message.h"

msg_buffer msg_pool[LAYER_SIZE];//ÏûÏ¢³Ø

void initMessage(void)
{
  for(int i = 0; i < LAYER_SIZE; i++)
  {
    initMsgBuffer(&msg_pool[i]);
  }
}

uint8 hasMessage(uint8 type)
{
  uint8 id = (uint8) type;
  return is_Empty(&msg_pool[id]) != 0;
}

MSG ReadMessage(uint8 type, uint8 data[MSG_LENGTH])
{
  uint8 id = (uint8) type;
  Msg msg;
  if(popMsgBuffer(&msg_pool[id], &msg))
  {
    if(data != NULL)
      memcpy(data, msg.data, MSG_LENGTH);
    return (MSG) msg.id;
  }
  return (MSG) 0;
}

uint8 SendMessage(uint8 type, Msg msg)
{
  uint8 id = (uint8) type;
  return pushMsgBuffer(&msg_pool[id], msg);
}

void initMsgBuffer(msg_buffer *mb)
{
  mb->head = mb->tail = 0;
  for(uint16 i = 0; i < MSG_CAP; i++)
  {
    for(uint16 j = 0; j < MSG_LENGTH; j++)
    {
      mb->msg[i].id = (MSG) 0;
      mb->msg[i].data[j] = 0;
    }
  }
}

uint8 is_Empty(msg_buffer *mb)
{
  return (mb->head == mb->tail);
}

uint8 is_Full(msg_buffer *mb)
{
  return (mb->head == ((mb->tail + 1) % MSG_CAP));
}

uint16 getMsgBufferLen(msg_buffer *mb)
{
  return (mb->tail - mb->head + MSG_CAP) % MSG_CAP;
}

uint8 pushMsgBuffer(msg_buffer *mb, Msg msg)
{
  if(is_Full(mb))
  {
    return 0;
  }
  mb->msg[mb->tail].id = msg.id;
  memcpy(mb->msg[mb->tail].data, msg.data, MSG_LENGTH);
  mb->tail = (mb->tail + 1) % MSG_CAP;
  return 1;
}

uint8 popMsgBuffer(msg_buffer *mb, Msg *msg)
{
  if(is_Empty(mb))
  {
    return 0;
  }
  msg->id = mb->msg[mb->head].id;
  memcpy(msg->data, mb->msg[mb->head].data, MSG_LENGTH);
  mb->head = (mb->head + 1) % MSG_CAP;
  return 1;
}