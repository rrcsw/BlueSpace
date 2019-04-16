#include "common.h"
#include "MK60_port.h"
#include "MK60_gpio.h"
#include "LED.H"


/* 
 * 定义LED 编号对应的管脚
 */
PTX_n LED_PTxn[LED_MAX] = {D8,D7,D1,D0};


/*! 
 *  @brief      初始化LED端口
 *  @param      LED_e    LED编号
 *  @since      v5.0
 *  Sample usage:       LED_init (LED0);    //初始化 LED0
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
 *  @brief      设置LED灯亮灭
 *  @param      LED_e           LED编号（LED0、LED1、LED2、LED3）
 *  @param      LED_status      LED亮灭状态（LED_ON、LED_OFF）
 *  @since      v5.0
 *  Sample usage:       LED (LED0,LED_ON);    //点亮 LED0
 */
void    led(LED_e ledn,LED_status status)
{
    gpio_set(LED_PTxn[ledn],status);
}

/*! 
 *  @brief      设置LED灯亮灭反转
 *  @param      LED_e           LED编号（LED0、LED1、LED2、LED3）
 *  @since      v5.0
 *  Sample usage:       LED_turn (LED0);    // LED0灯亮灭反转
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


