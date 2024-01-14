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
 ʵ��0��ALIENTEK STM32F103�����幤��ģ��
 ע�⣬�����ֲ��еĵ����½�ʹ�õ�main�ļ�
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

u8  tempbuff[20];
extern u8 key_static;

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 	  
	uart_init(115200);	 
	LED_Init();
	EXTIX_Init();
	SMBus_Init();        
	OLED_Init();			//OLED��ʼ��
	OLED_Clear();			//����
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

