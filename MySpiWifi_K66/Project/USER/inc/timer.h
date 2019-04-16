#ifndef _timer_h
#define _timer_h

#include "headfile.h"
#include <stdlib.h>

void setTimeout(Timer *this, void (*cb_func)(void), uint16 timeout);
void setInterval(Timer *this, void (*cb_func)(void), uint16 interval);
void startTimer(Timer *this, void (*cb_func)(void), uint16 ms);
void stopTimer(Timer *this);
void destroyTimer(Timer *this);
Timer *new_Timer();


void initClock(void);
void timeout(void);

#endif