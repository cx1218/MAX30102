#include "user.h"
#include "oled.h"
#include "delay.h"
#include "led.h"




void dispaly(void)
{
	
	GUI_DrawFont16(0,0,"����",0);
	OLED_ShowString(32,0,":",16);
	OLED_ShowString(70,0,"T/M",16);
	
	GUI_DrawFont16(0,2,"Ѫ��",0);
	OLED_ShowString(32,2,":",16);
	OLED_ShowString(70,2,"%",16);
	
//	GUI_DrawFont16(0,6,"���",0);
//	OLED_ShowString(32,6,":",16);
}
