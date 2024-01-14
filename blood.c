#include "blood.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "led.h"
#include "key.h"
uint16_t g_fft_index = 0;         	 	//fft��������±�

struct compx s1[FFT_N+16];           	//FFT������������S[1]��ʼ��ţ����ݴ�С�Լ�����
struct compx s2[FFT_N+16];           	//FFT������������S[1]��ʼ��ţ����ݴ�С�Լ�����

struct
{
	float 	Hp	;			//Ѫ�쵰��	
	float 	HpO2;			//����Ѫ�쵰��
	
}g_BloodWave;//ѪҺ��������

BloodData g_blooddata = {0};					//ѪҺ���ݴ洢

#define CORRECTED_VALUE			47   			//�궨ѪҺ��������
/*funcation start ------------------------------------------------------------*/
//ѪҺ�����Ϣ����
void blood_data_update(void)
{
	//��־λ��ʹ��ʱ ��ȡFIFO
	g_fft_index=0;
	while(g_fft_index < FFT_N)
	{
		while(MAX30102_INTPin_Read()==0)
		{
			//��ȡFIFO
			max30102_read_fifo();  //read from MAX30102 FIFO2
			//������д��fft���벢������
			if(g_fft_index < FFT_N)
			{
				//������д��fft���벢������
				s1[g_fft_index].real = fifo_red;
				s1[g_fft_index].imag= 0;
				s2[g_fft_index].real = fifo_ir;
				s2[g_fft_index].imag= 0;
				g_fft_index++;
			}
		}
	}
}

//ѪҺ��Ϣת��
void blood_data_translate(void)
{	
	float n_denom;
	float Heart_Rate=0.0;
	float R=0.0;
	float sp02_num=0.0;
	uint16_t i;
	int s1_max_index=0;
	int s2_max_index=0;
	//ֱ���˲�
	float dc_red =0; 
	float dc_ir =0;
	float ac_red =0; 
	float ac_ir =0;
	
	for (i=0 ; i<FFT_N ; i++ ) 
	{
		dc_red += s1[i].real ;
		dc_ir +=  s2[i].real ;
	}
		dc_red =dc_red/FFT_N ;
		dc_ir =dc_ir/FFT_N ;
	for (i=0 ; i<FFT_N ; i++ )  
	{
		s1[i].real =  s1[i].real - dc_red ; 
		s2[i].real =  s2[i].real - dc_ir ; 
	}
	
	//�ƶ�ƽ���˲�
	for(i = 1;i < FFT_N-1;i++) 
	{
			n_denom= ( s1[i-1].real + 2*s1[i].real + s1[i+1].real);
			s1[i].real=  n_denom/4.00; 
			
			n_denom= ( s2[i-1].real + 2*s2[i].real + s2[i+1].real);
			s2[i].real=  n_denom/4.00; 			
	}
	//�˵�ƽ���˲�
	for(i = 0;i < FFT_N-8;i++) 
	{
			n_denom= ( s1[i].real+s1[i+1].real+ s1[i+2].real+ s1[i+3].real+ s1[i+4].real+ s1[i+5].real+ s1[i+6].real+ s1[i+7].real);
			s1[i].real=  n_denom/8.00; 
			
			n_denom= ( s2[i].real+s2[i+1].real+ s2[i+2].real+ s2[i+3].real+ s2[i+4].real+ s2[i+5].real+ s2[i+6].real+ s2[i+7].real);
			s2[i].real=  n_denom/8.00; 	
	}
	/*for(i = 0;i < FFT_N;i++) 
	{
		printf("%f\r\n",s2[i].real);	
	}*/
	//��ʼ�任��ʾ	
	g_fft_index = 0;	
	//���ٸ���Ҷ�任
	FFT(s1);
	FFT(s2);
	
	//��ƽ��
	for(i = 0;i < FFT_N;i++) 
	{
		s1[i].real=sqrtf(s1[i].real*s1[i].real+s1[i].imag*s1[i].imag);
		s1[i].real=sqrtf(s2[i].real*s2[i].real+s2[i].imag*s2[i].imag);
	}
	//���㽻������
	for (i=1 ; i<FFT_N ; i++ ) 
	{
		ac_red += s1[i].real ;
		ac_ir +=  s2[i].real ;
	}

	s1_max_index = find_max_num_index(s1, 100);
	s2_max_index = find_max_num_index(s2, 100);	
	Heart_Rate = 60.00 * ((100.0 * s2_max_index )/ 512.00);
	g_blooddata.heart = Heart_Rate;
	R = (ac_ir*dc_red)/(ac_red*dc_ir);
	sp02_num =-45.060*R*R+ 30.354 *R + 94.845;
	g_blooddata.SpO2 = sp02_num;
			
}



extern float temp;
void blood_Loop(void)//��ȡѪ����������
{
	blood_data_update();
	blood_data_translate();
	if(g_blooddata.heart<=46 || g_blooddata.heart>=130)
	{
		OLED_ShowString(42,0,"---",16);
		printf("����:---T/min\r\n");
	}
	else
	{
		OLED_ShowNumber(42,0,g_blooddata.heart,3,16);
		printf("����:%3dT/min\r\n",g_blooddata.heart);
	}
	if(g_blooddata.SpO2<99.99 && g_blooddata.SpO2>50)
	{
		OLED_ShowNumber(42,2,(int)g_blooddata.SpO2,3,16);
		printf("Ѫ��:%d%%\r\n",(int)g_blooddata.SpO2);
	}
	else
	{	
		OLED_ShowString(42,2,"---",16);
		printf("Ѫ��:---%%\r\n");
	}
	if( (g_blooddata.heart<70 || g_blooddata.heart>=130) || (g_blooddata.SpO2>100||g_blooddata.SpO2<50))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_6);//����������
		if(Key_Scan(KEY2_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
			GPIO_ResetBits(GPIOA,GPIO_Pin_6);//�رշ�����
	}
	else
	{

		GPIO_ResetBits(GPIOA,GPIO_Pin_6);//�رշ�����
	}
	printf("--------------\r\n");
}


