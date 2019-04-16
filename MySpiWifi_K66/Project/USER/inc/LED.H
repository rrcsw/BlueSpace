
#ifndef __LED_H__
#define __LED_H__


/*! 枚举LED编号 */
typedef enum
{
  LED0,
  LED1,
  LED2,
  LED3,
  LED_MAX,
}LED_e;

/*! 枚举LED亮灭状态 */
typedef enum LED_status
{
  LED_ON  =   0,            //灯亮(对应低电平)
  LED_OFF =   1             //灯暗(对应高电平)
}LED_status; 


extern void    led_init(LED_e);            //初始化LED端口
extern void    led(LED_e,LED_status);      //设置LED灯亮灭
extern void    led_turn(LED_e);            //设置LED灯亮灭反转

void LED_set(uint8_t led_no, uint8_t on);

#endif  //__LED_H__