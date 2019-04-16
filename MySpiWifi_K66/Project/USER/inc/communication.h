#ifndef _communication_h
#define _communication_h

#include "headfile.h"
#include <string.h>

void initTcpBuff(void);//留出的接口
void ProcessMessage(uint8 img_buff[IMAGE_SIZE], CarInfo car_info);//留出的接口

uint8 PushTcpBuff(TcpData tcp_data);
void SendTcpData(void);
void ReceiveTcpData(void);


uint16 setImagePack(TcpData *tcp_data, uint8_t img[IMAGE_SIZE]);
uint16 setCarInfoPack(TcpData *tcp_data, CarInfo car_info);
uint16 setMeetingCarPack(TcpData *tcp_data, uint8_t img[600]);

#endif