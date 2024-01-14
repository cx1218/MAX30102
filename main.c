#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "mlx90614.h"
#include "oled.h"
#include "user.h"
#include "led.h"
#include "MAX30102.h"
#include "algorithm.h"
#include "blood.h"
#include "exti.h"
#include "key.h"
/************************************************
 实验0：ALIENTEK STM32F103开发板工程模板
 注意，这是手册中的调试章节使用的main文件
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

u8  tempbuff[20];
extern u8 key_static;

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();	    	 	  
	uart_init(115200);	 
	LED_Init();
	EXTIX_Init();
	SMBus_Init();        
	OLED_Init();			//OLED初始化
	OLED_Clear();			//清屏
	MAX30102_init();
	OLED_Clear();
	KEY_Init();
	while(1)
	{
		dispaly();

		if(key_static==1)
		{
			blood_Loop();	
		}
		delay_ms(10);
	}	 
} 

