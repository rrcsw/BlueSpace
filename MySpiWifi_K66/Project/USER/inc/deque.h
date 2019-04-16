#ifndef _deque_h
#define _deque_h

#include "headfile.h"
#include <stdlib.h>

deque initDeque(int num);
int size(deque q);
int isEmpty(deque q);
int isFull(deque q);
void push_front(deque q, ElementType ele);
ElementType pop_front(deque q);
void push_back(deque q, ElementType ele);
ElementType pop_back(deque q, ElementType ele);
void traverse_deque(deque q);

#endif