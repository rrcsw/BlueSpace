#ifndef _util_h
#define _util_h

#include "headfile.h"


void Debug(char *str);//���� ��ӡ����
void Increase(void);//ʱ����������
int currentTime(void);//��ȡ��ǰʱ��
void timeFormatToStr(int time, char *str);//ʱ��countתstr

void range_protect(void * value,float upper_limit,float lower_limit,uint8_t type);//��Χ�޷���֧���������Ͳ�������typeָ�������c�ļ�
#define getArraySize(x) (sizeof(x) / sizeof(x[0]))//��ȡ���鳤��
float calc_array_sum(float array[], int start, int end);//�������
void move_weight(float w_src[],float w_des[],int p);//�ƶ�Ȩ������

#endif