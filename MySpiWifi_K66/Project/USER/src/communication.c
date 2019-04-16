#include "communication.h"

#define length TCPBUFF_LENGTH
#define DATA_LENGTH 2600

uint8 data[DATA_LENGTH] = { 0 };//缓冲器

queue tcp_buff;//缓冲区队列
int index = 0;//Tcp缓冲区当前索引

void ProcessTcpData(uint8 img_buff[IMAGE_SIZE], CarInfo car_info)//通信层
{
  /*static int count = 0;
  message msg;
  if(count >= 5)
  {
    uint16 real_len = setCarInfoPack(&msg, car_info);
    PushTcpBuff(msg);
  }
  else if(count >= 2)
  {
    uint16 real_len = setImagePack(&msg, img_buff);
    PushTcpBuff(msg);
  }
  count++;
  if(count >= 6)
  count = 0;
  
  SendTcpData();//发送消息*/
  ReceiveTcpData();//接收消息
}

void handleMessage_COMM(void)
{
  while(hasMessage(COMM_))
  {
    uint8 data[MSG_LENGTH] = { 0 };
    switch(ReadMessage(COMM_, data))
    {
    case WM_COMM:
      Debug("测试消息框架的通信层");
      break;
    default:
      break;
    }
  }
}

void initTcpBuff(void)
{
  initQueue(&tcp_buff);
}

uint8 PushTcpBuff(TcpData tcp_data)
{
  return enQueue(&tcp_buff,tcp_data);
}

int getIndex()
{
  return index;
}

void reset()
{
  index = 0;
}

uint8 isAvailable(int total_len, int len)
{
  return (index <= (total_len - len));
}

uint8 append(uint8 dest[],uint8 src[],int len,int total_len)
{
  if(index >= total_len)
  {
    printf("数组满了");
    return 0;
  }
  memcpy(dest + index,src,len);
  index += len;
  return 1;
}

void SendTcpData(void)
{
  int size = getQueueLen(&tcp_buff);
  
  for(uint16 i = 0; i < size; i++)
  {
    if(isAvailable(DATA_LENGTH,TCPBUFF_LENGTH))
    {
      TcpData tcp_data;
      if(deQueue(&tcp_buff, &tcp_data))
      {
        append(data, tcp_data.data, TCPBUFF_LENGTH,DATA_LENGTH);
        continue;
      }
    }
    printf("tcp缓冲区出问题");
    break;
  }
  M8266WIFI_SPI_Send_Data(data, getIndex(), 0, NULL);
  reset();//发送完成后清空
  //systick_delay_ms(5);
}

void ReceiveTcpData()
{
  if(M8266WIFI_SPI_Has_DataReceived() == 1)
  {
    WifiEvent event;
    event.cmd = 0x01;
    M8266WIFI_SPI_RecvData(event.msg, 50, 20, 0, NULL);
    onSpiWifiReceiveData(event);
//    printf("收到数据:\r\n");
//    emitEvent(EVENT_DETECTED_MEETING_AREA_EVENT);
  }
}

uint16 setImagePack(TcpData *tcp_data, uint8_t img[IMAGE_SIZE])
{
  uint16 data_length = 15 + 600;
  uint16 image_offset = 15;
  uint16 image_length = 600;
  
  uint16 tail_offset = 10 + 5 + image_offset + image_length;
  
  for(u16 i = 0; i < 5; i++)
    tcp_data->data[i] = 0x0F;
  for(u16 i = 5; i < 10; i++)
    tcp_data->data[i] = 0x1E;
  

  tcp_data->data[10] = 0x01;//数据去向
  tcp_data->data[11] = (data_length >> 8) & 0xFF;//数据总长 高
  tcp_data->data[12] = (data_length) & 0xFF;//数据总长 低
  tcp_data->data[13] = 0xFF;
  tcp_data->data[14] = 0xFF;
  
  tcp_data->data[15] = 0x02;//类型
  tcp_data->data[16] = 0xFF;
  tcp_data->data[17] = 0xFF;
  tcp_data->data[18] = 0xFF;
  tcp_data->data[19] = 0xFF;//校验码
  
  
  tcp_data->data[20] = (image_offset >> 8) & 0xFF;//image起始位置 高
  tcp_data->data[21] = (image_offset) & 0xFF;//image起始位置 低
  tcp_data->data[22] = (image_length >> 8) & 0xFF;//image长度 高
  tcp_data->data[23] = (image_length) & 0xFF;//image长度 低
  tcp_data->data[24] = 0x50;//图像宽度
  tcp_data->data[25] = 0x3C;//图像高度
  tcp_data->data[26] = 0xFF;
  tcp_data->data[27] = 0xFF;
  tcp_data->data[28] = 0xFF;
  tcp_data->data[29] = 0xFF;
  
  uint8 *ptr = &tcp_data->data[image_offset + 15];
  memcpy(ptr, img, image_length);
  
  for(u16 i = tail_offset; i < tail_offset + 5; i++)
    tcp_data->data[i] = 0x2D;
  for(u16 i = tail_offset + 5; i < tail_offset + 10; i++)
    tcp_data->data[i] = 0x3C;
  for(u16 i = tail_offset + 10; i < length; i++)
    tcp_data->data[i] = 0xFF;
  
  return (tail_offset + 10);
}

uint16 setCarInfoPack(TcpData *tcp_data, CarInfo car_info)
{
    uint16 data_length = 15 + sizeof(CarInfo) + 200;
    uint16 car_info_offset = 15;
    uint16 car_info_length = sizeof(CarInfo);
    uint16 custom_data_offset = car_info_offset + car_info_length;
    uint16 custom_data_length = 200;
    
    uint16 tail_offset = 10 + 5 + custom_data_offset + custom_data_length;
    
    for(u16 i = 0; i < 5; i++)
      tcp_data->data[i] = 0x0F;
    for(u16 i = 5; i < 10; i++)
      tcp_data->data[i] = 0x1E;
    
    tcp_data->data[10] = 0x01;//数据去向
    tcp_data->data[11] = (data_length >> 8) & 0xFF;//数据总长 高
    tcp_data->data[12] = (data_length) & 0xFF;//数据总长 低
    tcp_data->data[13] = 0xFF;
    tcp_data->data[14] = 0xFF;
    
    tcp_data->data[15] = 0x03;//类型
    tcp_data->data[16] = 0xFF;
    tcp_data->data[17] = 0xFF;
    tcp_data->data[18] = 0xFF;//校验码
    tcp_data->data[19] = 0xFF;//校验码
    
    tcp_data->data[20] = (car_info_offset >> 8) & 0xFF;//car_info起始位置 高
    tcp_data->data[21] = (car_info_offset) & 0xFF;//car_info起始位置 低
    tcp_data->data[22] = (car_info_length >> 8) & 0xFF;//car_info长度 高
    tcp_data->data[23] = (car_info_length) & 0xFF;//car_info长度 低
    tcp_data->data[24] = (custom_data_offset >> 8) & 0xFF;//custom_data起始位置 高
    tcp_data->data[25] = (custom_data_offset) & 0xFF;//custom_data起始位置 低
    tcp_data->data[26] = (custom_data_length >> 8) & 0xFF;//custom_data长度 高
    tcp_data->data[27] = (custom_data_length) & 0xFF;//custom_data长度 低                 
    tcp_data->data[28] = 0xFF;
    tcp_data->data[29] = 0xFF;
    
    uint8_t *ptr = &tcp_data->data[car_info_offset + 15];
    memcpy(ptr, (uint8_t *)(&car_info), car_info_length);
    
    //ptr = &msg->data[custom_data_offset + 15];
    //memcpy(ptr, (uint8_t *)custom_data, custom_data_length);
    /*for(u16 i = custom_data_offset + 15; i < 15 + custom_data_offset + custom_data_length; i++)
    {
      msg->data[i] = 0x30;
    }*/
    
    for(u16 i = tail_offset; i < tail_offset + 5; i++)
      tcp_data->data[i] = 0x2D;
    for(u16 i = tail_offset + 5; i < tail_offset + 10; i++)
      tcp_data->data[i] = 0x3C;
    for(u16 i = tail_offset + 10; i < length; i++)
      tcp_data->data[i] = 0xFF;
    
    return (tail_offset + 10);
}

uint16 setMeetingCarPack(TcpData *tcp_data, uint8_t img[600])
{
  uint16 data_length = 15 + 10;
  uint16 pack_offset = 15;
  uint16 pack_length = 10;
  
  uint16 tail_offset = 10 + 5 + pack_offset + pack_length;
  
  for(u16 i = 0; i < 5; i++)
    tcp_data->data[i] = 0x0F;
  for(u16 i = 5; i < 10; i++)
    tcp_data->data[i] = 0x1E;
  

  tcp_data->data[10] = 0x02;//数据去向,发对方
  tcp_data->data[11] = (data_length >> 8) & 0xFF;//数据总长 高
  tcp_data->data[12] = (data_length) & 0xFF;//数据总长 低
  tcp_data->data[13] = 0xFF;
  tcp_data->data[14] = 0xFF;
  
  tcp_data->data[15] = 0x01;//类型
  tcp_data->data[16] = 0xFF;
  tcp_data->data[17] = 0xFF;
  tcp_data->data[18] = 0xFF;
  tcp_data->data[19] = 0xFF;//校验码
  
  
  tcp_data->data[20] = (pack_offset >> 8) & 0xFF;//pack起始位置 高
  tcp_data->data[21] = (pack_offset) & 0xFF;//pack起始位置 低
  tcp_data->data[22] = (pack_length >> 8) & 0xFF;//pack长度 高
  tcp_data->data[23] = (pack_length) & 0xFF;//pack长度 低
  tcp_data->data[24] = 0xFF;
  tcp_data->data[25] = 0xFF;
  tcp_data->data[26] = 0xFF;
  tcp_data->data[27] = 0xFF;
  tcp_data->data[28] = 0xFF;
  tcp_data->data[29] = 0xFF;
  
  //uint8 *ptr = &msg->data[pack_offset + 15];
  //memcpy(ptr, meeting_car_data, pack_length);
  
  for(u16 i = tail_offset; i < tail_offset + 5; i++)
    tcp_data->data[i] = 0x2D;
  for(u16 i = tail_offset + 5; i < tail_offset + 10; i++)
    tcp_data->data[i] = 0x3C;
  for(u16 i = tail_offset + 10; i < length; i++)
    tcp_data->data[i] = 0xFF;
  
  return (tail_offset + 10);
}
