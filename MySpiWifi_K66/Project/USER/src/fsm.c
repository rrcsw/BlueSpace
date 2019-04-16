#include "fsm.h"

deque action_deque;
State m_state = STATE_INITIAL;//˫��״̬��������ʼ��ΪSTATE_INITIAL

Timer *timer_mcw;
Timer *timer_rfw;


void initFSM(int num)
{
  timer_mcw = new_Timer();
  timer_rfw = new_Timer();
  action_deque = initDeque(num);
}

void ActionLoop(void)
{
  int length = size(action_deque);
  for (int i = 0; i < length; i++)
  {
    Action action_id = (Action) pop_front(action_deque);
    takeAction(action_id);
  }
}

int timer[2] = { 0 };

void deal_mc_waiting(void)
{
  if(m_state == STATE_WAITING_FOR_MEETING)
  {
    //�ص���������
    //Ȼ�󴥷� �Է�ʧ�� �¼�
    emitEvent(EVENT_ANOTHER_CAR_FAIL);
  }
}

void deal_rf_waiting(void)
{
  timer[1]++;
  if(timer[1] >= 500)
  {
    emitEvent(EVENT_ANOTHER_CAR_FAIL);
  }
//  if(m_state == STATE_WAITING_FOR_DEST)
//  {
//    emitEvent(EVENT_ANOTHER_CAR_FAIL);
//  }
}

//DispatchMessage
void takeAction(Action action)
{
  switch (action)
  {
  case action_initial_detect_meeting_area:
      //����·�ᳵ��
      //detectMeetingArea();
      //���� ��֪�� ��� �ᳵ�� ��Ϣ
      emitInitialEvent(&m_state);
      Debug("ִ�� ��ʼ״̬�� ѭ������·������\r\n");
      break;
  case action_other_detect_meeting_area:
      //����·�ᳵ��
      //detectMeetingArea();
      //���� ��֪�� ��� �ᳵ�� ��Ϣ
      emitOtherCarWaitingMeetingAreaEvent(&m_state);
      Debug("ִ�� �Է��ᳵ�ȴ�״̬�� �ҷ�����·������\r\n");
      break;
  case action_waiting:
      //�ճ�ȥ�߰��ף���ʹ�ٶ�Ϊ��
      //֪ͨ�Է���ͣ����
      //ע�ᳬʱ�ص�����
    
      //���� ���Ʋ� ���� �ó��ᳵ���� ��Ϣ
      //���� ���Ʋ� �ջ� ֹͣ ��Ϣ
      //���� ��ʱ�� �����ᳵ�ȴ���ʱ�¼� ��Ϣ
    //timer_mcw->setTimeout(timer_mcw, deal_mc_waiting, 5000);
    deal_mc_waiting();
      //setLeftMotor_u(0);
      //setRightMotor_u(0);
    Debug("ִ�� �ᳵ�ȴ���\r\n");
    break;
  case action_meeting_car:
      //ȡ���ȴ���ʱ�¼�
      timer_mcw->stop(timer_mcw);
      //���� ���Ʋ� ���� �ᳵ��Ϣ
      //���� ��֪�� �ᳵ�ɰܼ�� ��Ϣ
//      setLeftMotor_u(2400);
//      setRightMotor_u(2400);
      emitMeetingCarEvent(&m_state);
      Debug("ִ�� �ᳵ������\r\n");
      break;
  case action_finish_meeting_car:
      
      //���� ���Ʋ� �ջ���ʻ ��Ϣ
      emitHandShakeEvent(&m_state);
      Debug("ִ�� ��ɻᳵ��Ķ�����\r\n");
      break;
      
  case action_hand_shake_detect_finish_line:
      
      //���� ͨ�Ų� �������� ��Ϣ
      //���� ��֪�� ��������� ��Ϣ
    
      emitHandShakeEvent(&m_state);
      Debug("ִ�� ���ֶ���, ѭ������յ��ߣ�\r\n");
      break;
  case action_other_detect_finish_line:
    //���� ͨ�Ų� �������� ��Ϣ
    //���� ��֪�� ��������� ��Ϣ
    emitOtherCarWaitingFinishLineEvent(&m_state);
    Debug("ִ�� �Է��յ��ߵȴ�״̬�� �ҷ�����յ��߶�����\r\n");
    break;
      
  case action_finish_line_waiting:
    timer_rfw->setTimeout(timer_rfw, deal_rf_waiting, 10000);
      //���� ���Ʋ� �ػ� ֹͣ ��Ϣ
      //���� ��ʱ�� ����ͣ���ȴ���ʱ�¼� ��Ϣ
    emitOtherCarWaitingFinishLineEvent(&m_state);
    Debug("ִ�� �յ��ߵȴ���\r\n");
    break;
      
  case action_rushing_dest:
    Debug("ִ�� ����յ��ߣ�\r\n");
    break;
      
  case action_single_car_mode:
    Debug("ִ�� ��������ģʽ��\r\n");
    break;
      
      
  case action_inform_another_car_meeting_car:
      //message msg;
      //msg.data[] = #1#
      //PushTcpBuff(msg,);
      //���� ͨ�Ų� ֪ͨ�Է��ҷ��ѽ���ᳵ״̬ ��Ϣ
    Debug("ִ�� ֪ͨ�Է����Ҽ�⵽��·��׼���ᳵ��\r\n");
      break;
  case action_inform_another_car_fail:
      //message msg;
      //msg.data[] = #1#
      //PushMsg(msg,);
      //���� ͨ�Ų� ֪ͨ�Է��ҷ��ᳵʧ�ܻ���� ��Ϣ
    Debug("ִ�� ֪ͨ�Է����һᳵʧ�ܣ�\r\n");
      break;
  case action_inform_another_car_finish_meeting_car:
      //message msg;
      //msg.data[] = #1#
      //PushMsg(msg,);
      //���� ͨ�Ų� ֪ͨ�Է��ҷ���ɻᳵ ��Ϣ
    Debug("ִ�� ֪ͨ�Է�������ɻᳵ��\r\n");
      break;
  case action_inform_another_car_detected_finish_line:
      //message msg;
      //msg.data[] = #1#
      //PushMsg(msg,);
      //���� ͨ�Ų� ֪ͨ�Է��ҷ���⵽������ ��Ϣ
    Debug("ִ�� ֪ͨ�Է����Ҽ�⵽�����ߣ�\r\n");
      break;
  default:
      break;
  }
}

void emitEvent(Event event)
{
  switch (event)
  {
  case EVENT_INITIAL_DETECT_MEETING_AREA:
    emitInitialEvent(&m_state);
    break;
  case EVENT_ANOTHER_CAR_DETECTED_MEETING_AREA:
    emitAnotherCarDetectedMeetingAreaEvent(&m_state);
    break;;
  case EVENT_DETECTED_MEETING_AREA:
    emitDetectedMeetingAreaEvent(&m_state);
    break;
  case EVENT_FAIL_MEETING_CAR:
    emitFailMeetingCarEvent(&m_state);
    break;
  case EVENT_FINISH_MEETING_CAR:
    emitFinishMeetingCarEvent(&m_state);
    break;
  case EVENT_ANOTHER_CAR_DETECTED_FINISH_LINE:
    emitAnotherCarDetectedFinishLineEvent(&m_state);
    break;
  case EVENT_DETECTED_FINISH_LINE:
    emitDetectedFinishLineEvent(&m_state);
    break;
  case EVENT_ANOTHER_CAR_FAIL:
    emitAnotherCarFailEvent(&m_state);
    break;
  case EVENT_OUR_CAR_FAIL:
    emitOurCarFailEvent(&m_state);
    break;
  default:
    //�������������¼�
    break;
  }
}

//void UpdateState()
//{
//    //��־λ���ƴ��� ʱ�򣬴���ʱ��������������
//    //if()
//}

void AppendAction(Action action, int prior)
{
  if (prior == FROM_FRONT)
    push_front(action_deque, action);
  else if (prior == FROM_BACK)
    push_back(action_deque, action);
}

void emitInitialEvent(State *next_state)
{
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_INITIAL:
    *next_state = STATE_INITIAL;
    Debug("########״̬����Ϊ-> |��ʼ| ########\r\n\n");
    AppendAction(action_initial_detect_meeting_area, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitOtherCarWaitingMeetingAreaEvent(State *next_state)
{
  State cur_state;
  cur_state = *next_state;
  switch(cur_state)
  {
  case STATE_ANOTHER_CAR_WAITING:
    *next_state = STATE_ANOTHER_CAR_WAITING;
    Debug("########״̬����Ϊ-> |�Է��ᳵ���ȴ�| ########\r\n\n");
    AppendAction(action_other_detect_meeting_area, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitAnotherCarDetectedMeetingAreaEvent(State *next_state)
{
  Debug("****************************���� |�Է���⵽�ᳵ��| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_INITIAL:
    *next_state = STATE_ANOTHER_CAR_WAITING;
    Debug("########״̬�� |��ʼ| ��Ϊ-> |�Է��ᳵ���ȴ�| ########\r\n\n");
    AppendAction(action_other_detect_meeting_area, FROM_BACK);
    //do nothing
    break;
  case STATE_WAITING_FOR_MEETING:
    *next_state = STATE_MEETING_CAR;
    Debug("########״̬�� |�ҷ��ᳵ���ȴ�| ��Ϊ-> |���ڻᳵ| ########\r\n\n");
    AppendAction(action_meeting_car, FROM_BACK);
    break;
  default:
    break;
  }
}


void emitDetectedMeetingAreaEvent(State *next_state)
{
  Debug("****************************���� |�ҷ���⵽�ᳵ��| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_INITIAL:
    AppendAction(action_inform_another_car_meeting_car, FROM_BACK);
    *next_state = STATE_WAITING_FOR_MEETING;
    Debug("########״̬�� |��ʼ| ��Ϊ-> |�ҷ��ᳵ���ȴ�| ########\r\n\n");
    AppendAction(action_waiting, FROM_BACK);
    break;
  case STATE_ANOTHER_CAR_WAITING:
    AppendAction(action_inform_another_car_meeting_car, FROM_BACK);
    *next_state = STATE_MEETING_CAR;
    Debug("########״̬�� |�Է��ᳵ���ȴ�| ��Ϊ-> |���ڻᳵ| ########\r\n\n");
    AppendAction(action_meeting_car, FROM_BACK);
    break;
  }
}


void emitMeetingCarEvent(State *next_state)
{
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_MEETING_CAR:
    *next_state = STATE_MEETING_CAR;
    Debug("########״̬����Ϊ-> |���ڻᳵ| ########\r\n\n");
    AppendAction(action_meeting_car, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitFinishMeetingCarEvent(State *next_state)
{
  Debug("****************************���� |�ҷ��ᳵ�ɹ�| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch(cur_state)
  {
  case STATE_MEETING_CAR:
    AppendAction(action_inform_another_car_finish_meeting_car, FROM_BACK);
    *next_state = STATE_FINISH_MEETING_CAR;
    Debug("########״̬�� |���ڻᳵ| ��Ϊ-> |�ᳵ���| ########\r\n\n");
    AppendAction(action_finish_meeting_car, FROM_BACK);
    break;
  default:
    break;
  }
}


void emitFailMeetingCarEvent(State *next_state)
{
  Debug("****************************���� |�ҷ��ᳵʧ��| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_MEETING_CAR:
    *next_state = STATE_FAIL_MEETING_CAR;
    Debug("########״̬�� |���ڻᳵ| ��Ϊ-> |�ᳵʧ��| ########\r\n\n");
    AppendAction(action_inform_another_car_fail, FROM_BACK);
    break;
  default:
    break;
  }
}


void emitHandShakeEvent(State *next_state)
{
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_FINISH_MEETING_CAR:
    *next_state = STATE_HAND_SHAKE;
    Debug("########״̬�� |�ᳵ���| ��Ϊ-> |����| ########\r\n\n");
    AppendAction(action_hand_shake_detect_finish_line, FROM_BACK);
    break;
  case STATE_HAND_SHAKE:
    *next_state = STATE_HAND_SHAKE;
    Debug("########״̬����Ϊ-> |����| ########\r\n\n");
    AppendAction(action_hand_shake_detect_finish_line, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitOtherCarWaitingFinishLineEvent(State *next_state)
{
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_ANOTHER_CAR_WAITING_FOR_DEST:
    *next_state = STATE_ANOTHER_CAR_WAITING_FOR_DEST;
    Debug("########״̬����Ϊ-> |�Է��յ���ǰ�ȴ�| ########\r\n\n");
    AppendAction(action_other_detect_finish_line, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitAnotherCarDetectedFinishLineEvent(State *next_state)
{
  Debug("****************************���� |�Է���⵽�յ���| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_HAND_SHAKE:
    *next_state = STATE_ANOTHER_CAR_WAITING_FOR_DEST;
    Debug("########״̬�� |����| ��Ϊ-> |�Է��յ���ǰ�ȴ�| ########\r\n\n");
    AppendAction(action_other_detect_finish_line, FROM_BACK);
    break;
  case STATE_WAITING_FOR_DEST:
    *next_state = STATE_RUSHING_DEST;
    Debug("########״̬�� |�ҷ��յ���ǰ�ȴ�| ��Ϊ-> |����յ���| ########\r\n\n");
    AppendAction(action_rushing_dest, FROM_BACK);
  default:
    break;
  }
}

void emitDetectedFinishLineEvent(State *next_state)
{
  Debug("****************************���� |�ҷ���⵽�յ���| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_HAND_SHAKE:
    AppendAction(action_inform_another_car_detected_finish_line, FROM_BACK);
    *next_state = STATE_WAITING_FOR_DEST;
    Debug("########״̬�� |����| ��Ϊ-> |�ҷ��յ���ǰ�ȴ�| ########\r\n\n");
    AppendAction(action_finish_line_waiting, FROM_BACK);
    break;
  case STATE_ANOTHER_CAR_WAITING_FOR_DEST:
    AppendAction(action_inform_another_car_detected_finish_line, FROM_BACK);
    *next_state = STATE_RUSHING_DEST;
    Debug("########״̬�� |�Է��յ���ǰ�ȴ�| ��Ϊ-> |����յ���| ########\r\n\n");
    AppendAction(action_rushing_dest, FROM_BACK);
  default:
    break;
  }
}

void emitAnotherCarFailEvent(State *next_state)
{
  Debug("****************************���� |�Է�ʧ��| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_SINGLE_CAR_MODE:
  case STATE_OUR_CAR_FAIL:
  case STATE_FAIL_MEETING_CAR:
    //do nothing
    break;
  default:
    *next_state = STATE_SINGLE_CAR_MODE;
    Debug("########״ֱ̬�ӱ�Ϊ-> |����ģʽ| ########\r\n\n");
    AppendAction(action_single_car_mode, FROM_FRONT);
    break;
  }
}

void emitOurCarFailEvent(State *next_state)
{
  Debug("****************************���� |�ҷ�ʧ��| �¼�****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch(cur_state)
  {
  case STATE_OUR_CAR_FAIL:
  case STATE_SINGLE_CAR_MODE:
  case STATE_FAIL_MEETING_CAR:
  case STATE_MEETING_CAR:
  case STATE_WAITING_FOR_MEETING:
    //do nothing
    break;
  default:
    *next_state = STATE_OUR_CAR_FAIL;
    Debug("########״ֱ̬�ӱ�Ϊ-> |�ҷ�ʧ��| ########\r\n\n");
    AppendAction(action_inform_another_car_fail, FROM_FRONT);
    break;
  }
}
