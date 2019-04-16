#include "timer.h"

Clock clock[CLOCK_NUM];

void initClock(void)
{
  for(int i = 0; i < CLOCK_NUM; i++)
  {
    Clock *this = &clock[i];
    this->run_flag = 0;
    this->repeat_flag = 0;
    this->repeat_count = 0;
    this->timeout = 1;
    this->left_time = this->timeout;
  }
}

void timeout(void)
{
  for(int i = 0; i < CLOCK_NUM; i++)
  {
    if(clock[i].run_flag != 0)
    {
      if(clock[i].left_time > 0)
      {
        clock[i].left_time--;
      }
      else
      {
        clock[i].call_back();
        if(clock[i].repeat_flag != 0 || (--clock[i].repeat_count > 0))
        {
          clock[i].left_time = clock[i].timeout;
        }
        else
        {
          clock[i].run_flag = 0;
        }
      }
    }
  }
}

Timer *new_Timer()
{
  Timer *this;
  this = (Timer *) malloc(sizeof(Timer));
  this->start = startTimer;
  this->stop = stopTimer;
  this->destroy = destroyTimer;
  this->setTimeout = setTimeout;
  this->setInterval = setInterval;

  int i = 0;
  for(i = 0; i < CLOCK_NUM; i++)
  {
    if(clock[i].used_flag != 0)
    {
      continue;
    }
    else
    {
      this->clock = &clock[i];
      this->clock->run_flag = 0;
      this->clock->used_flag = 1;
      break;
    }
  }
  if(i == CLOCK_NUM)
  {
    free(this);
    return NULL;
  }
  return this;
}

void setTimeout(Timer *this, void (*cb_func)(void), uint16 timeout)
{
  if(!this)
    return;
  this->start(this, cb_func, timeout);
  this->clock->call_back = cb_func;
  this->clock->repeat_count = 1;
  this->clock->repeat_flag = 0;
}

void setInterval(Timer *this, void (*cb_func)(void), uint16 interval)
{
  if(!this)
    return;
  this->start(this, cb_func, interval);
  this->clock->call_back = cb_func;
  this->clock->repeat_count = 9999;
  this->clock->repeat_flag = 1;
}

void startTimer(Timer *this, void (*cb_func)(void), uint16 ms)
{
  if(!this)
    return;
  this->clock->timeout = ms;
  this->clock->left_time = this->clock->timeout;
  this->clock->run_flag = 1;
  this->clock->call_back = cb_func;
  this->clock->repeat_count = 9999;
  this->clock->repeat_flag = 1;
}

void stopTimer(Timer *this)
{
  if(!this)
    return;
  this->clock->run_flag = 0;
}

void destroyTimer(Timer *this)
{
  if(!this)
    return;
  this->clock->used_flag = 0;
  if(this)
    free(this);
}
