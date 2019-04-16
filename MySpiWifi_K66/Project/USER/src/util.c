#include "util.h"

int time = 0;

void Debug(char *str)
{
#ifdef DEBUG
  char cur_time[20];
  timeFormatToStr(currentTime(), cur_time);
  printf("当前时间: %s\r\n", cur_time);
  printf("%s", str);
#endif
}

void Increase(void)
{
#ifdef DEBUG
  time++;
#endif
}

int currentTime(void)
{
  return time;
}

void timeFormatToStr(int time, char *str)
{
  int mm = 0, seconds = 0, minutes = 0;
  mm = time % 100;
  seconds = time / 100;
  minutes = seconds / 60;
  seconds = seconds % 60;
  minutes = minutes % 60;
  sprintf(str, "%02d:%02d.%02d", minutes, seconds, mm);
}
void range_protect(void * value,float upper_limit,float lower_limit,uint8_t type)
{
  switch(type)
  {
  case 1://uint8_t
    {
      uint8_t * p = (uint8_t *) value;
      if((* p) > (uint8_t) upper_limit)
        * p = (uint8_t) upper_limit;
      else if((* p) < (uint8_t) lower_limit)
        * p = (uint8_t) lower_limit;
      break;
    }
  case 2://int16_t
    {
      int16_t * p = (int16_t *) value;
      if((* p) > (int16_t) upper_limit)
        * p = (int16_t) upper_limit;
      else if((* p) < (int16_t) lower_limit)
        * p = (int16_t) lower_limit;
      break;
    }
  case 3://float
    {
      float * p = (float *) value;
      if((* p) > (float) upper_limit)
        * p = (float) upper_limit;
      else if((* p) < (float) lower_limit)
        * p = (float) lower_limit;
      break;
    }
  }
}

float calc_array_sum(float array[], int start,int end)
{
  float sum = 0;
  for(uint8 i = start;i < end;i++)
  {
    sum += array[i];
  }
  return sum;
}

void move_weight(float w_src[],float w_des[],int p)
{
  if(p > 10)
    p = 10;
  if(p < - 10)
    p = - 10;
  if(p >= 0)
  {
    for(uint8 i = 0;i < p;i++)
    {
      w_des[i] = 1;
    }
    for(uint8 i = p;i < IMAGE_HEIGHT;i++)
    {
      w_des[i] = w_src[i - p];
    }
  }
  else
  {
    for(uint8 i = 0;i < IMAGE_HEIGHT + p;i++)
    {
      w_des[i] = w_src[i - p];
    }
    for(uint8 i = IMAGE_HEIGHT + p;i < IMAGE_HEIGHT;i++)
    {
      w_des[i] = 1;
    }
  }
}