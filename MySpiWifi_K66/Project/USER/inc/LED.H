
#ifndef __LED_H__
#define __LED_H__


/*! ö��LED��� */
typedef enum
{
  LED0,
  LED1,
  LED2,
  LED3,
  LED_MAX,
}LED_e;

/*! ö��LED����״̬ */
typedef enum LED_status
{
  LED_ON  =   0,            //����(��Ӧ�͵�ƽ)
  LED_OFF =   1             //�ư�(��Ӧ�ߵ�ƽ)
}LED_status; 


extern void    led_init(LED_e);            //��ʼ��LED�˿�
extern void    led(LED_e,LED_status);      //����LED������
extern void    led_turn(LED_e);            //����LED������ת

void LED_set(uint8_t led_no, uint8_t on);

#endif  //__LED_H__