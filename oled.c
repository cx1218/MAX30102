//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : Evk123
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : 0.96寸OLED 接口演示例程(STM32F103ZE系列IIC)
//              说明: 
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PB12（SCL）
//              SDA   接PB13（SDA）            
//              ----------------------------------------------------------------
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "string.h"
#include "delay.h"
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 	
u8 OLED_GRAM[128][8];
/**********************************************
//IIC Start
**********************************************/
/**********************************************
//IIC Start
**********************************************/
void OLED_IIC_Start()
{

	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void OLED_IIC_Stop()
{
OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void OLED_IIC_Wait_Ack()
{

	//GPIOB->CRH &= 0XFFF0FFFF;	//设置PB12为上拉输入模式
	//GPIOB->CRH |= 0x00080000;
//	OLED_SDA = 1;
//	delay_us(1);
	//OLED_SCL = 1;
	//delay_us(50000);
/*	while(1)
	{
		if(!OLED_SDA)				//判断是否接收到OLED 应答信号
		{
			//GPIOB->CRH &= 0XFFF0FFFF;	//设置PB12为通用推免输出模式
			//GPIOB->CRH |= 0x00030000;
			return;
		}
	}
*/
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void OLED_Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
		da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
}
/**********************************************
// IIC Write Command
**********************************************/
void OLED_Write_IIC_Command(unsigned char IIC_Command)
{
		OLED_IIC_Start();
		OLED_Write_IIC_Byte(0x78);            //Slave address,SA0=0
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(0x00);			//write command
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(IIC_Command); 
		OLED_IIC_Wait_Ack();	
		OLED_IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void OLED_Write_IIC_Data(unsigned char IIC_Data)
{
		OLED_IIC_Start();
		OLED_Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(0x40);			//write data
		OLED_IIC_Wait_Ack();	
		OLED_Write_IIC_Byte(IIC_Data);
		OLED_IIC_Wait_Ack();	
		OLED_IIC_Stop();
}
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
   OLED_Write_IIC_Data(dat);
	}
	else
	{
   OLED_Write_IIC_Command(dat);
	}
}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1);
			}
	}
}


/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
	unsigned int m;
	for(;Del_50ms>0;Del_50ms--)
		for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}

//坐标设置

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //更新显示
}

///////////////////////////////////////////////////////////////

void OLED_Show_Pos(u8 x,u8 y,u16 c)
{
	unsigned char i=0;	
	OLED_Set_Pos(x,y);
	for(i=0;i<8;i++)
	OLED_WR_Byte(TS8X16[c*16+i],OLED_DATA);
	OLED_Set_Pos(x,y+1);
	for(i=0;i<8;i++)
	OLED_WR_Byte(TS8X16[c*16+i+8],OLED_DATA);
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	y = 63 - y;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	
	
	OLED_WR_Byte (0xb0+1,OLED_CMD);    //设置页地址（0~7）
	OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
	OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
	OLED_WR_Byte(OLED_GRAM[10][1],OLED_DATA);
}

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y; 
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);//63-y
	}													    
	OLED_Refresh_Gram();//更新显示
}
//OLED_Fill(0,0,8,8,1);
//////////////////////////////////////////////////////////////////////

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	while (*chr!='\0')
	{		OLED_ShowChar(x,y,*chr,Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			chr++;
	}
}
//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
	{
		OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
		adder+=1;
    }	
	OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
	{	
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
    }					
}

//******************************************************************
//函数名：  GUI_DrawFont16
//作者：    阿尔法嵌入式
//日期：    2013-02-22
//功能：    显示单个16X16中文字体
//输入参数：x,y :起点坐标
//			*s需要显示的汉子字符串
//			mode:模式	0,实心显示;1,镂空显示
//返回值：  无
//修改记录：无
//******************************************************************
#define HZnum sizeof(tfont16)/sizeof(typFNT_GB16)	//统计字库汉字数目
void GUI_DrawFont16(u16 x, u16 y, u8 *s,u8 mode)
{
	u8 t,adder=0;
	u8 Strnum;
	u16 k,m;
	u8 data;
	Strnum=strlen(s)/2;	
	for(m=0;m<Strnum;)//需要显示多少个汉子就在字库中检索多少次
	{
		for (k=0;k<HZnum;k++)//字库循环检索
		{
			if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
			{ 	
				//printf("k = %d\r\n",k);
				s = s + 2;
				OLED_Set_Pos(x,y);	
				for(t=0;t<16;t++)
				{
					if(mode == 1) data = ~tfont16[k].Msk[t];
					else data = tfont16[k].Msk[t];
					OLED_WR_Byte(data,OLED_DATA);
					adder+=1;
				}	
				OLED_Set_Pos(x,y+1);	
				for(t=0;t<16;t++)
				{	
					if(mode == 1) data = ~tfont16[k].Msk[16+t];
					else data = tfont16[k].Msk[16+t];
					OLED_WR_Byte(data,OLED_DATA);
					adder+=1;
				}
				x=x+16;				
				m++;
				break;
			}
		}
		continue; 
	}
} 
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 
void OLED_Float(unsigned char Y,unsigned char X,double real,unsigned char N) 
{
   unsigned char   i_Count=1;
   unsigned char   n[12]={0};
   long   j=1;  
   int    real_int=0;
   double decimal=0;
   unsigned int   real_decimal=0;
   if(real<0)
	 {
		 real_int=(int)(-real);
	 }
	 else
	 {
		 real_int=(int)real;
   }
	 decimal=real-real_int;
   real_decimal=decimal*1e4;
   while(real_int/10/j!=0)
   {
      j=j*10;i_Count++;  
   } 
   n[0]=(real_int/10000)%10; 
   n[1]=(real_int/1000)%10;
   n[2]=(real_int/100)%10;
   n[3]=(real_int/10)%10;
   n[4]=(real_int/1)%10; 
   n[5]='.';
   n[6]=(real_decimal/1000)%10;
   n[7]=(real_decimal/100)%10;
   n[8]=(real_decimal/10)%10;
   n[9]=real_decimal%10;
   n[6+N]='\0'; 
   for(j=0;j<10;j++) n[j]=n[j]+16+32;
	 if(real<0) 
	 {		 
		 i_Count+=1;
		 n[5-i_Count]='-';
	 }
   n[5]='.';
   n[6+N]='\0';   
   OLED_ShowString(X,Y,&n[5-i_Count],12); 
}

 void OLED_Float2(unsigned char Y,unsigned char X,double real,unsigned char N1,unsigned char N2) 
{
   unsigned char   i_Count=1;
   unsigned char   n[12]={0};
   long   j=1;  
   unsigned int   real_int=0;
   double decimal=0;
   unsigned int   real_decimal=0;
   X=X*8;
   real_int=(int)real;
   //Dis_Num(2,0,real_int,5);
   decimal=real-real_int;
   real_decimal=decimal*1e4;
   //Dis_Num(2,6,real_decimal,4);
   while(real_int/10/j!=0)
   {
      j=j*10;i_Count++;  
   } 
   n[0]=(real_int/10000)%10; 
   n[1]=(real_int/1000)%10;
   n[2]=(real_int/100)%10;
   n[3]=(real_int/10)%10;
 
   n[5]='.';
   n[6]=(real_decimal/1000)%10;
   n[7]=(real_decimal/100)%10;
   n[8]=(real_decimal/10)%10;
   n[9]=real_decimal%10;
   n[6+N2]='\0'; 
   for(j=0;j<10;j++) n[j]=n[j]+16+32;
   n[5]='.';
   n[6+N2]='\0';   
   OLED_ShowString(X,Y,&n[5-N1],12); 
}

  void OLED_Num2(unsigned char x,unsigned char y, int number)
{
        unsigned char shi,ge;
	      int num =number;
	if(num<0)
	{ 
		num=-num;
		shi=num%100/10;
    ge=num%10;
	  OLED_fuhao_write(x,y,13); 
    OLED_Num_write(x+1,y,shi);
    OLED_Num_write(x+2,y,ge); 
  } 
  else
	{

		shi=num%100/10;
    ge=num%10;
		OLED_fuhao_write(x,y,11);
    OLED_Num_write(x+1,y,shi);
    OLED_Num_write(x+2,y,ge); 
  }
        
}

void OLED_Num3(unsigned char x,unsigned char y,int number)
{
  unsigned char ge,shi,bai;
	int num =number;
	if(num<0)
	{
		    num=-num;
		    OLED_fuhao_write(x,y,13); //显示-号
        ge = num %10;
        shi = num/10%10;
        bai = num/100;
        OLED_Num_write(x+3,y,ge);
        OLED_Num_write(x+2,y,shi);
        OLED_Num_write(x+1,y,bai);
	}
	else
	{
       OLED_fuhao_write(x,y,11);
        ge = num %10;
        shi = num/10 %10;
        bai = num/100;
        OLED_Num_write(x+3,y,ge);
        OLED_Num_write(x+2,y,shi);
        OLED_Num_write(x+1,y,bai);
  }
}

void OLED_Num4(unsigned char x,unsigned char y, int number)
{
	unsigned char qian,bai,shi,ge;
	int num =number;
	if(num<0)
	{
		num=-num;
	}
	qian=num/1000;
	bai=num%1000/100;
	shi=num%100/10;
	ge=num%10;

	OLED_Num_write(x,y,qian);
	OLED_Num_write(x+1,y,bai);
	OLED_Num_write(x+2,y,shi);
	OLED_Num_write(x+3,y,ge);
}

void OLED_Num_write(unsigned char x,unsigned char y,unsigned char asc) 
{
	int i=0;
	OLED_Set_Pos(x*6,y);
	for(i=0;i<6;i++)
	{
		 OLED_WR_Byte(F6x8[asc+16][i],OLED_DATA);         
	}
}	
void OLED_fuhao_write(unsigned char x,unsigned char y,unsigned char asc) 
{

	  int i=0;
    OLED_Set_Pos(x*6,y);
    for(i=0;i<6;i++)
    {
       OLED_WR_Byte(F6x8[asc][i],OLED_DATA);         
    }
 
}			

void OLED_Num5(unsigned char x,unsigned char y,unsigned int number)
{
        unsigned char wan,qian,bai,shi,ge;
        wan=number/10000;
	    	qian = number%10000/1000;
        bai=number%1000/100;
        shi=number%100/10;
        ge=number%10;
        OLED_Num_write(x,y,wan);
        OLED_Num_write(x+1,y,qian);
        OLED_Num_write(x+2,y,bai);
        OLED_Num_write(x+3,y,shi);
		OLED_Num_write(x+4,y,ge);
}

//初始化SSD1306					    
void OLED_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //使能B端口时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB8,9
 	GPIO_SetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13);	


	delay_ms(800);
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}  

//进度方块X点左右极限
const u8 Left = 0,right = 96;

/*********************************************************************************************
函数名：进度条框架显示
参  数：y      竖向显示位置
		x      横向显示位置
返回值：无
**********************************************************************************************/
void progress_bar(u8 x,u8 y)
{
    u8 i;
	OLED_Show_Pos(x,y,0);
	for(i=0;i<(right-x)/8;i++)
	{
		OLED_Show_Pos(x+8+i*8,y,2);
	}
	OLED_Show_Pos(right,y,1);
}

/*********************************************************************************************
函数名：进度条进度显示
参  数：progress    进度0-100%
		x      		进度方块横向显示位置
返回值：无
**********************************************************************************************/
void Moving_progress_bar(u8 progress,u8 y)
{
	//last_x 记录上一次进度方块x点位置
	//last_y 记录上一次进度方块y点位置
	//progress 当前进度
	static u8 last_x = Left,last_y,x;
	//显示进度方块左右极限位置
	if(x >= right) x = right;
	if(x <= Left)  x = Left;
	//当前进度计算
	x = (progress/100.0)*96;
	if(progress != 100)//进度0-99%显示
	{
		OLED_ShowNumber(right+8,y,progress,2,16);
		OLED_ShowString(right+24,y,"%",16);
	}else OLED_ShowNumber(right+8,y,progress,3,16);//特殊点100%显示
	//进度方块位置变化则刷新
	if(last_x != x)
	{
		//原方块位置刷新
		if(last_x == Left || last_x == right)//左右极限特殊处理
		{	if(last_x == Left)
			{
				OLED_Show_Pos(last_x,last_y,0);	
			}
			if(last_x == right)
			{
				OLED_Show_Pos(last_x,last_y,1);	
			}
		}else{//非极限特殊情况处理
			OLED_Show_Pos(last_x,last_y,2);
		}
		OLED_Show_Pos(x,y,3);//显示进度方块
	}
	//记录进度方块本次想xy位置
	last_x = x;
	last_y = y;
}

/*********************************************************************************
函数名：初刷参数选择框
参  数：x      参数选择框x位置
           num 参数选择框数量和位置
           start  实心圆位置
返回值：无
*********************************************************************************/
void Display_advance(u8 x,u16 num,u16 start)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		if(((num>>i) & 0x01))
		{
			OLED_Show_Pos(x+0,i*2,8);//空心
			OLED_Show_Pos(x+8,i*2,9);
		}
	}
	OLED_Show_Pos(x+0,start,6);//实心
	OLED_Show_Pos(x+8,start,7);
}

/*************************************************************************************
函数名：参数选择提示
参  数：obj    对象控制参数
		start  显示起始位置
返回值：无
*************************************************************************************/
void Select_object(u8 x,u8 obj,u8 start)
{
	static u8 last_obj=0;
	if(last_obj != obj)
	{
		OLED_Show_Pos(x+0,obj*2+start,6);//实心
		OLED_Show_Pos(x+8,obj*2+start,7);
		
		OLED_Show_Pos(x+0,last_obj*2+start,8);//空心
		OLED_Show_Pos(x+8,last_obj*2+start,9);
	}
	last_obj = obj;
}


