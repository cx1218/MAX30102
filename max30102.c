#include "max30102.h"

uint8_t ack;
uint16_t fifo_red;
uint16_t fifo_ir;

void IIC_Init(void)
{					
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(IIC_SCL_CLK | IIC_SDA_CLK,ENABLE);
	GPIO_InitStruct.GPIO_Mode		=	GPIO_Mode_Out_OD ;
	GPIO_InitStruct.GPIO_Pin		=	IIC_SCL_GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin		=	IIC_SDA_GPIO_Pin;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStruct);
}

void IIC_Delay(void)
{
	uint8_t i=6; //i=10延时1.5us//这里可以优化速度 ，经测试最低到5还能写入
   while(i--);
}

void IIC_Start(void)
{
	IIC_SCL_1;
	IIC_SDA_1;//启始信号建立时间 0.6us 400KHz
	IIC_Delay();
	IIC_SDA_0;
	IIC_Delay();//启始信号保持时间0.6us
	IIC_SCL_0;
	IIC_Delay();//时钟低电平时间1.3us
}
 
void IIC_Stop(void)
{	
	IIC_SDA_0;
	IIC_SCL_1;
	IIC_Delay();//结束信号建立时间0.6us
	IIC_SDA_1;
	IIC_Delay();//总线空闲时间时间1.3us
}

void IIC_Send_Byte(uint8_t byte)
{
	uint8_t i;//先发送高位
	for(i=0;i<8;i++)
	{
		if(byte & 0x80)
		{
			IIC_SDA_1;
		}
		else
		{
			IIC_SDA_0;
		}
		IIC_Delay();
		IIC_SCL_1;
		IIC_Delay();
		IIC_SCL_0;
		IIC_Delay();
		byte<<=1;
	}
	IIC_SDA_1;  
	IIC_Delay();
	IIC_SCL_1;
	IIC_Delay();
	if(IIC_SDA_READ)
	{
		ack=1;
	}
	else
	{
		ack=0;
	}
	IIC_SCL_0;
	IIC_Delay();
}

uint8_t IIC_Receive_Byte(void)
{
   uint8_t receive=0;
   uint8_t i;//置数据线为输入方式
   for(i=0;i<8;i++)
   {     		
			receive<<=1;
			IIC_SCL_1;//置时钟线为高使数据线上数据有效
			IIC_Delay();
			if(IIC_SDA_READ)
			{
				receive++;//读数据位,接收的数据位放入retc中
			}
			IIC_SCL_0;  
			IIC_Delay();
   }
   return receive;
}

uint8_t IIC_Write_Byte(uint8_t device_addr,uint8_t register_addr,uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte(device_addr+0);
	if (ack == 1)return 0;
	IIC_Send_Byte(register_addr);   
	if (ack == 1)return 0;
	IIC_Send_Byte(data);
	if (ack == 1)return 0; 
	IIC_Stop();
	return 1;
}

void I2C_Ack(uint8_t a)
{ 
	if(a)
	{
		IIC_SDA_1;            //非应答
		IIC_Delay(); 
		IIC_SCL_1;  	
		IIC_Delay();  
		IIC_SCL_0; 	
		IIC_Delay();  		
	}
	else
	{
		IIC_SDA_0; 						//应答
		IIC_Delay(); 
		IIC_SCL_1;  
		IIC_Delay(); 
		IIC_SCL_0; 
		IIC_Delay(); 
		IIC_SDA_1; 
	}
}

uint8_t IIC_Read_Byte(uint8_t device_addr,uint8_t register_addr)
{
	uint8_t read_data;
	IIC_Start();
	IIC_Send_Byte(device_addr+0);
	if (ack == 1)return 0;
	IIC_Send_Byte(register_addr);
	if (ack == 1)return 0;
	IIC_Start();
	IIC_Send_Byte(device_addr+1);
	if (ack == 1)return 0;
	read_data = IIC_Receive_Byte();
	I2C_Ack(1);
	IIC_Stop();
	return read_data;
}

uint8_t IIC_Write_Array(uint8_t device_addr,uint16_t register_addr,u8 *Data,uint16_t Num)
{
	uint16_t i;
	IIC_Start();
	IIC_Send_Byte(device_addr+0);
	if (ack == 1)return 0;
	IIC_Send_Byte(register_addr);
	if (ack == 1)return 0;
	IIC_Start();
	IIC_Send_Byte(device_addr+1);
	if (ack == 1)return 0;
	for(i=0;i<Num;i++)
	{
		IIC_Send_Byte(*Data++);
		if (ack == 1)return 0;
	}
	IIC_Stop();
	return 1;
}

uint8_t IIC_Read_Array(uint8_t device_addr,uint16_t register_addr,uint8_t *Data,uint16_t Num)
{
	uint16_t i;
	IIC_Start();
	IIC_Send_Byte(device_addr+0);
	if (ack == 1)return 0;
	IIC_Send_Byte(register_addr);
	if (ack == 1)return 0;
	IIC_Start();
	IIC_Send_Byte(device_addr+1);
	if (ack == 1)return 0;
	for(i=0;i<Num;i++)
	{
		*Data++ = IIC_Receive_Byte();
		if(i==Num-1)
			I2C_Ack(1);
		else
			I2C_Ack(0);
	}
	IIC_Stop();
	return 1;
}

void MAX30102_GPIO(void)
{	
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(MAX30102_INTPin_CLK,ENABLE);
	GPIO_InitStruct.GPIO_Mode		=	GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin		=	MAX30102_INTPin_Pin;
	GPIO_Init(MAX30102_INTPin_PORT,&GPIO_InitStruct);
	
}

uint8_t Max30102_reset(void)
{
	if(IIC_Write_Byte(MAX30102_Device_address,REG_MODE_CONFIG, 0x40))
        return 1;
    else
        return 0;    
}

void MAX30102_Config(void)
{
	IIC_Write_Byte(MAX30102_Device_address,REG_INTR_ENABLE_1,0xc0);//// INTR setting
	IIC_Write_Byte(MAX30102_Device_address,REG_INTR_ENABLE_2,0x00);//
	IIC_Write_Byte(MAX30102_Device_address,REG_FIFO_WR_PTR,0x00);//FIFO_WR_PTR[4:0]
	IIC_Write_Byte(MAX30102_Device_address,REG_OVF_COUNTER,0x00);//OVF_COUNTER[4:0]
	IIC_Write_Byte(MAX30102_Device_address,REG_FIFO_RD_PTR,0x00);//FIFO_RD_PTR[4:0]
	
	IIC_Write_Byte(MAX30102_Device_address,REG_FIFO_CONFIG,0x0f);//sample avg = 1, fifo rollover=false, fifo almost full = 17
	IIC_Write_Byte(MAX30102_Device_address,REG_MODE_CONFIG,0x03);//0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	IIC_Write_Byte(MAX30102_Device_address,REG_SPO2_CONFIG,0x27);	// SPO2_ADC range = 4096nA, SPO2 sample rate (50 Hz), LED pulseWidth (400uS)  
	IIC_Write_Byte(MAX30102_Device_address,REG_LED1_PA,0x32);//Choose value for ~ 10mA for LED1
	IIC_Write_Byte(MAX30102_Device_address,REG_LED2_PA,0x32);// Choose value for ~ 10mA for LED2
	IIC_Write_Byte(MAX30102_Device_address,REG_PILOT_PA,0x7f);// Choose value for ~ 25mA for Pilot LED
}

void max30102_read_fifo(void)
{
	uint8_t ach_i2c_data[6];
	uint16_t un_temp;
	fifo_red=0;
	fifo_ir=0;

	//read and clear status register
	IIC_Read_Byte(MAX30102_Device_address,REG_INTR_STATUS_1);
	IIC_Read_Byte(MAX30102_Device_address,REG_INTR_STATUS_2);

	ach_i2c_data[0]=REG_FIFO_DATA;

	IIC_Read_Array(MAX30102_Device_address,REG_FIFO_DATA,ach_i2c_data,6);

	un_temp=ach_i2c_data[0];
	un_temp<<=14;
	fifo_red+=un_temp;
	un_temp=ach_i2c_data[1];
	un_temp<<=6;
	fifo_red+=un_temp;
	un_temp=ach_i2c_data[2];
	un_temp>>=2;
	fifo_red+=un_temp;

	un_temp=ach_i2c_data[3];
	un_temp<<=14;
	fifo_ir+=un_temp;
	un_temp=ach_i2c_data[4];
	un_temp<<=6;
	fifo_ir+=un_temp;
	un_temp=ach_i2c_data[5];
	un_temp>>=2;
	fifo_ir+=un_temp;

	if(fifo_ir<=10000)
	{
		fifo_ir=0;
	}
	if(fifo_red<=10000)
	{
		fifo_red=0;
	}
}

void MAX30102_init(void)
{
	int i=0;
	IIC_Init();
	MAX30102_GPIO();
	Max30102_reset();
	MAX30102_Config();
	for(i = 0;i < 128;i++) 
	{
		while(MAX30102_INTPin_Read()==0)
		{
			//读取FIFO
			max30102_read_fifo();
		}
	}
}

