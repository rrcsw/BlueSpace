#include "deque.h"

deque initDeque(int num)
{
  if (num <= 2)
    return NULL;
  deque Q;
  Q = (deque) malloc(sizeof(struct deque_));
  if (Q == NULL)
    return NULL;
  Q->data_ptr = (ElementType *) malloc(sizeof(ElementType) * num);
  if (Q->data_ptr == NULL)
  {
    free(Q);
    return NULL;
  }
  Q->size = 0;
  Q->capacity = num;
  Q->front = 1;
  Q->rear = 0;
  return Q;
}

int size(deque q)
{
    return q->size;
}

int isEmpty(deque q)
{
    return q->size == 0;
}

int isFull(deque q)
{
    return q->size == q->capacity;
}

void push_front(deque q, ElementType ele)
{
    if (!isFull(q))
    {
        q->front = (q->front - 1 + q->capacity) % q->capacity;
        q->data_ptr[q->front] = ele;
        q->size++;
    }
}

ElementType pop_front(deque q)
{
    if (!isEmpty(q))
    {
        ElementType ele = q->data_ptr[q->front];
        q->front = (q->front + 1 + q->capacity) % q->capacity;
        q->size--;
        return ele;
    }
    return (ElementType)(- 100);
}

void push_back(deque q, ElementType ele)
{
    if (!isFull(q))
    {
        q->rear = (q->rear + 1 + q->capacity) % q->capacity;
        q->data_ptr[q->rear] = ele;
        q->size++;
    }
}

ElementType pop_back(deque q, ElementType ele)
{
    if (!isEmpty(q))
    {
        ElementType ele = q->data_ptr[q->rear];
        q->rear = (q->rear - 1 + q->capacity) % q->capacity;
        q->size--;
        return ele;
    }
    return (ElementType)(- 100);
}

void traverse_deque(deque q)
{
    if (!isEmpty(q))
    {
        int index = q->front;
        while (index != q->rear)
        {
            //ElementType ele = q->data_ptr[index];
            //printf("%d\r\n", ele);
            index = (index + 1 + q->capacity) % q->capacity;
        }
        //ElementType ele = q->data_ptr[q->rear];
        //printf("%d\r\n", ele);
    }
}