#include "user.h"
#include "oled.h"
#include "delay.h"
#include "led.h"




void dispaly(void)
{
	
	GUI_DrawFont16(0,0,"心率",0);
	OLED_ShowString(32,0,":",16);
	OLED_ShowString(70,0,"T/M",16);
	
	GUI_DrawFont16(0,2,"血氧",0);
	OLED_ShowString(32,2,":",16);
	OLED_ShowString(70,2,"%",16);
	
//	GUI_DrawFont16(0,6,"结果",0);
//	OLED_ShowString(32,6,":",16);
}
