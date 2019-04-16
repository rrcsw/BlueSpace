#include "fsm.h"

deque action_deque;
State m_state = STATE_INITIAL;//双车状态变量，初始化为STATE_INITIAL

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
    //回到赛道中央
    //然后触发 对方失败 事件
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
      //检测断路会车区
      //detectMeetingArea();
      //发送 感知层 检测 会车区 消息
      emitInitialEvent(&m_state);
      Debug("执行 初始状态下 循环检测断路动作！\r\n");
      break;
  case action_other_detect_meeting_area:
      //检测断路会车区
      //detectMeetingArea();
      //发送 感知层 检测 会车区 消息
      emitOtherCarWaitingMeetingAreaEvent(&m_state);
      Debug("执行 对方会车等待状态下 我方检测断路动作！\r\n");
      break;
  case action_waiting:
      //拐出去走半米，并使速度为零
      //通知对方我停好了
      //注册超时回调函数
    
      //发送 控制层 开环 让出会车区域 消息
      //发送 控制层 闭环 停止 消息
      //发送 定时器 启动会车等待超时事件 消息
    //timer_mcw->setTimeout(timer_mcw, deal_mc_waiting, 5000);
    deal_mc_waiting();
      //setLeftMotor_u(0);
      //setRightMotor_u(0);
    Debug("执行 会车等待！\r\n");
    break;
  case action_meeting_car:
      //取消等待超时事件
      timer_mcw->stop(timer_mcw);
      //发送 控制层 开环 会车消息
      //发送 感知层 会车成败检测 消息
//      setLeftMotor_u(2400);
//      setRightMotor_u(2400);
      emitMeetingCarEvent(&m_state);
      Debug("执行 会车动作！\r\n");
      break;
  case action_finish_meeting_car:
      
      //发送 控制层 闭环行驶 消息
      emitHandShakeEvent(&m_state);
      Debug("执行 完成会车后的动作！\r\n");
      break;
      
  case action_hand_shake_detect_finish_line:
      
      //发送 通信层 交换数据 消息
      //发送 感知层 检测起跑线 消息
    
      emitHandShakeEvent(&m_state);
      Debug("执行 握手动作, 循环检测终点线！\r\n");
      break;
  case action_other_detect_finish_line:
    //发送 通信层 交换数据 消息
    //发送 感知层 检测起跑线 消息
    emitOtherCarWaitingFinishLineEvent(&m_state);
    Debug("执行 对方终点线等待状态下 我方检测终点线动作！\r\n");
    break;
      
  case action_finish_line_waiting:
    timer_rfw->setTimeout(timer_rfw, deal_rf_waiting, 10000);
      //发送 控制层 必环 停止 消息
      //发送 定时器 启动停车等待超时事件 消息
    emitOtherCarWaitingFinishLineEvent(&m_state);
    Debug("执行 终点线等待！\r\n");
    break;
      
  case action_rushing_dest:
    Debug("执行 冲过终点线！\r\n");
    break;
      
  case action_single_car_mode:
    Debug("执行 开启单车模式！\r\n");
    break;
      
      
  case action_inform_another_car_meeting_car:
      //message msg;
      //msg.data[] = #1#
      //PushTcpBuff(msg,);
      //发送 通信层 通知对方我方已进入会车状态 消息
    Debug("执行 通知对方，我检测到断路，准备会车！\r\n");
      break;
  case action_inform_another_car_fail:
      //message msg;
      //msg.data[] = #1#
      //PushMsg(msg,);
      //发送 通信层 通知对方我方会车失败或出界 消息
    Debug("执行 通知对方，我会车失败！\r\n");
      break;
  case action_inform_another_car_finish_meeting_car:
      //message msg;
      //msg.data[] = #1#
      //PushMsg(msg,);
      //发送 通信层 通知对方我方完成会车 消息
    Debug("执行 通知对方，我完成会车！\r\n");
      break;
  case action_inform_another_car_detected_finish_line:
      //message msg;
      //msg.data[] = #1#
      //PushMsg(msg,);
      //发送 通信层 通知对方我方检测到起跑线 消息
    Debug("执行 通知对方，我检测到起跑线！\r\n");
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
    //不存在这样的事件
    break;
  }
}

//void UpdateState()
//{
//    //标志位控制触发 时序，触发时机，触发次数等
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
    Debug("########状态保持为-> |初始| ########\r\n\n");
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
    Debug("########状态保持为-> |对方会车区等待| ########\r\n\n");
    AppendAction(action_other_detect_meeting_area, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitAnotherCarDetectedMeetingAreaEvent(State *next_state)
{
  Debug("****************************触发 |对方检测到会车区| 事件****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_INITIAL:
    *next_state = STATE_ANOTHER_CAR_WAITING;
    Debug("########状态由 |初始| 变为-> |对方会车区等待| ########\r\n\n");
    AppendAction(action_other_detect_meeting_area, FROM_BACK);
    //do nothing
    break;
  case STATE_WAITING_FOR_MEETING:
    *next_state = STATE_MEETING_CAR;
    Debug("########状态由 |我方会车区等待| 变为-> |正在会车| ########\r\n\n");
    AppendAction(action_meeting_car, FROM_BACK);
    break;
  default:
    break;
  }
}


void emitDetectedMeetingAreaEvent(State *next_state)
{
  Debug("****************************触发 |我方检测到会车区| 事件****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_INITIAL:
    AppendAction(action_inform_another_car_meeting_car, FROM_BACK);
    *next_state = STATE_WAITING_FOR_MEETING;
    Debug("########状态由 |初始| 变为-> |我方会车区等待| ########\r\n\n");
    AppendAction(action_waiting, FROM_BACK);
    break;
  case STATE_ANOTHER_CAR_WAITING:
    AppendAction(action_inform_another_car_meeting_car, FROM_BACK);
    *next_state = STATE_MEETING_CAR;
    Debug("########状态由 |对方会车区等待| 变为-> |正在会车| ########\r\n\n");
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
    Debug("########状态保持为-> |正在会车| ########\r\n\n");
    AppendAction(action_meeting_car, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitFinishMeetingCarEvent(State *next_state)
{
  Debug("****************************触发 |我方会车成功| 事件****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch(cur_state)
  {
  case STATE_MEETING_CAR:
    AppendAction(action_inform_another_car_finish_meeting_car, FROM_BACK);
    *next_state = STATE_FINISH_MEETING_CAR;
    Debug("########状态由 |正在会车| 变为-> |会车完成| ########\r\n\n");
    AppendAction(action_finish_meeting_car, FROM_BACK);
    break;
  default:
    break;
  }
}


void emitFailMeetingCarEvent(State *next_state)
{
  Debug("****************************触发 |我方会车失败| 事件****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_MEETING_CAR:
    *next_state = STATE_FAIL_MEETING_CAR;
    Debug("########状态由 |正在会车| 变为-> |会车失败| ########\r\n\n");
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
    Debug("########状态由 |会车完成| 变为-> |握手| ########\r\n\n");
    AppendAction(action_hand_shake_detect_finish_line, FROM_BACK);
    break;
  case STATE_HAND_SHAKE:
    *next_state = STATE_HAND_SHAKE;
    Debug("########状态保持为-> |握手| ########\r\n\n");
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
    Debug("########状态保持为-> |对方终点线前等待| ########\r\n\n");
    AppendAction(action_other_detect_finish_line, FROM_BACK);
    break;
  default:
    break;
  }
}

void emitAnotherCarDetectedFinishLineEvent(State *next_state)
{
  Debug("****************************触发 |对方检测到终点线| 事件****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_HAND_SHAKE:
    *next_state = STATE_ANOTHER_CAR_WAITING_FOR_DEST;
    Debug("########状态由 |握手| 变为-> |对方终点线前等待| ########\r\n\n");
    AppendAction(action_other_detect_finish_line, FROM_BACK);
    break;
  case STATE_WAITING_FOR_DEST:
    *next_state = STATE_RUSHING_DEST;
    Debug("########状态由 |我方终点线前等待| 变为-> |冲刺终点线| ########\r\n\n");
    AppendAction(action_rushing_dest, FROM_BACK);
  default:
    break;
  }
}

void emitDetectedFinishLineEvent(State *next_state)
{
  Debug("****************************触发 |我方检测到终点线| 事件****\r\n\n");
  State cur_state;
  cur_state = *next_state;
  switch (cur_state)
  {
  case STATE_HAND_SHAKE:
    AppendAction(action_inform_another_car_detected_finish_line, FROM_BACK);
    *next_state = STATE_WAITING_FOR_DEST;
    Debug("########状态由 |握手| 变为-> |我方终点线前等待| ########\r\n\n");
    AppendAction(action_finish_line_waiting, FROM_BACK);
    break;
  case STATE_ANOTHER_CAR_WAITING_FOR_DEST:
    AppendAction(action_inform_another_car_detected_finish_line, FROM_BACK);
    *next_state = STATE_RUSHING_DEST;
    Debug("########状态由 |对方终点线前等待| 变为-> |冲刺终点线| ########\r\n\n");
    AppendAction(action_rushing_dest, FROM_BACK);
  default:
    break;
  }
}

void emitAnotherCarFailEvent(State *next_state)
{
  Debug("****************************触发 |对方失败| 事件****\r\n\n");
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
    Debug("########状态直接变为-> |单车模式| ########\r\n\n");
    AppendAction(action_single_car_mode, FROM_FRONT);
    break;
  }
}

void emitOurCarFailEvent(State *next_state)
{
  Debug("****************************触发 |我方失败| 事件****\r\n\n");
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
    Debug("########状态直接变为-> |我方失败| ########\r\n\n");
    AppendAction(action_inform_another_car_fail, FROM_FRONT);
    break;
  }
}
