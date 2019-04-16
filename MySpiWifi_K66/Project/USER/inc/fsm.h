#ifndef _fsm_h
#define _fsm_h

#include "headfile.h"
#include <stdlib.h>

#define FROM_FRONT 0
#define FROM_BACK 1

void initFSM(int num);//�����Ľӿ�
void ActionLoop();//�����Ľӿ�
void emitEvent(Event event);//�����Ľӿ�

void takeAction(Action action);
void AppendAction(Action action, int prior);

//��ʼʱ
void emitInitialEvent(State *next_state);

//�ᳵʱ
void emitOtherCarWaitingMeetingAreaEvent(State *next_state);
void emitAnotherCarDetectedMeetingAreaEvent(State *next_state);
void emitDetectedMeetingAreaEvent(State *next_state);

void emitMeetingCarEvent(State *next_state);
void emitFinishMeetingCarEvent(State *next_state);
void emitFailMeetingCarEvent(State *next_state);

//����ʱ
void emitHandShakeEvent(State *next_state);

//����յ���
void emitOtherCarWaitingFinishLineEvent(State *next_state);
void emitAnotherCarDetectedFinishLineEvent(State *next_state);
void emitDetectedFinishLineEvent(State *next_state);


//�Է����Լ�ʧ��
void emitAnotherCarFailEvent(State *next_state);
void emitOurCarFailEvent(State *next_state);

#endif