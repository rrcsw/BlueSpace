#ifndef _util_h
#define _util_h

#include "headfile.h"


void Debug(char *str);//调试 打印函数
void Increase(void);//时间自增函数
int currentTime(void);//获取当前时间
void timeFormatToStr(int time, char *str);//时间count转str

void range_protect(void * value,float upper_limit,float lower_limit,uint8_t type);//范围限幅，支持三种类型参数，由type指定，详见c文件
#define getArraySize(x) (sizeof(x) / sizeof(x[0]))//获取数组长度
float calc_array_sum(float array[], int start, int end);//数组求和
void move_weight(float w_src[],float w_des[],int p);//移动权重数组

#endif