#include "common.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include "LED.H"


/* 
 * ����LED ��Ŷ�Ӧ�Ĺܽ�
 */
PTX_n LED_PTxn[LED_MAX] = {D8,D7,D1,D0};


/*! 
 *  @brief      ��ʼ��LED�˿�
 *  @param      LED_e    LED���
 *  @since      v5.0
 *  Sample usage:       LED_init (LED0);    //��ʼ�� LED0
 */
void    led_init(LED_e ledn)
{
    if(ledn < LED_MAX)
    {
        gpio_init(LED_PTxn[ledn],GPO,LED_OFF);
    }
    else
    {
        ledn = LED_MAX;
        while(ledn--)
        {
            gpio_init(LED_PTxn[ledn],GPO,LED_OFF);
        }

    }
}



/*! 
 *  @brief      ����LED������
 *  @param      LED_e           LED��ţ�LED0��LED1��LED2��LED3��
 *  @param      LED_status      LED����״̬��LED_ON��LED_OFF��
 *  @since      v5.0
 *  Sample usage:       LED (LED0,LED_ON);    //���� LED0
 */
void    led(LED_e ledn,LED_status status)
{
    gpio_set(LED_PTxn[ledn],status);
}

/*! 
 *  @brief      ����LED������ת
 *  @param      LED_e           LED��ţ�LED0��LED1��LED2��LED3��
 *  @since      v5.0
 *  Sample usage:       LED_turn (LED0);    // LED0������ת
 */
void led_turn(LED_e ledn)
{
    gpio_turn(LED_PTxn[ledn]);
}


void LED_Init(void)
{
   led_init(LED0); led_init(LED1);  led_init(LED2); led_init(LED3);
}

void LED_set(uint8_t led_no, uint8_t on)
{
  led((LED_e)led_no,on?LED_ON:LED_OFF);
}


