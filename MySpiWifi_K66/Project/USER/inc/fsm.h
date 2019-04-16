#ifndef _fsm_h
#define _fsm_h

#include "headfile.h"
#include <stdlib.h>

#define FROM_FRONT 0
#define FROM_BACK 1

void initFSM(int num);//留出的接口
void ActionLoop();//留出的接口
void emitEvent(Event event);//留出的接口

void takeAction(Action action);
void AppendAction(Action action, int prior);

//初始时
void emitInitialEvent(State *next_state);

//会车时
void emitOtherCarWaitingMeetingAreaEvent(State *next_state);
void emitAnotherCarDetectedMeetingAreaEvent(State *next_state);
void emitDetectedMeetingAreaEvent(State *next_state);

void emitMeetingCarEvent(State *next_state);
void emitFinishMeetingCarEvent(State *next_state);
void emitFailMeetingCarEvent(State *next_state);

//握手时
void emitHandShakeEvent(State *next_state);

//冲刺终点线
void emitOtherCarWaitingFinishLineEvent(State *next_state);
void emitAnotherCarDetectedFinishLineEvent(State *next_state);
void emitDetectedFinishLineEvent(State *next_state);


//对方或自己失败
void emitAnotherCarFailEvent(State *next_state);
void emitOurCarFailEvent(State *next_state);

#endif