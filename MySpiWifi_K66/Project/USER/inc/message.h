#ifndef _message_h
#define _message_h

#include "headfile.h"
#include <string.h>

void initMessage(void);
uint8 hasMessage(uint8 type);
MSG ReadMessage(uint8 type, uint8 data[MSG_LENGTH]);
uint8 SendMessage(uint8 type, Msg msg);


void initMsgBuffer(msg_buffer *mb);
uint8 is_Empty(msg_buffer *mb);
uint8 is_Full(msg_buffer *mb);
uint16 getMsgBufferLen(msg_buffer *mb);
uint8 pushMsgBuffer(msg_buffer *mb, Msg msg);
uint8 popMsgBuffer(msg_buffer *mb, Msg *msg);

#endif