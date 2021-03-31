/*
*
*  This file only for enter novatek ISP mode 
*
*/

#include "sw_iic.h"

typedef unsigned char   Byte;
typedef unsigned short  Word;
#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80


#define comSetRegSRAM	0x78


void PulseDelay_()
{
	uint8_t i;
	for(i=0;i<40;i++);
}

void T_IIC_Start(void)
{
	IIC_SDA=0; //SDA low
	delay_us_(15);
	IIC_SCL=0; // SCL low
	delay_us_(15);
}

void T_IIC_Stop(void)
{
	IIC_SDA=0; // SDA low 
	delay_us_(15);
	IIC_SCL=1; //SCL high
	delay_us_(15);
	IIC_SDA=1; // SDA high
	delay_us_(15);
}

Byte T_IIC_Tx_(Byte Data)
{	
	Byte Ack;
	uint16_t AckLoop;
	Byte i;
	for(i=0;i<8;i++)
	{
	   IIC_SCL=0; // SCL LOW
		delay_us_(15);
		if(Data & BIT7)	
			IIC_SDA=1;//SDA HIGH
		else	
			IIC_SDA=0;// SDA LOW
		
		delay_us_(15);
		IIC_SCL=1;// SCL HIGH

		delay_us_(15);
		Data <<= 1;
	}
	delay_us_(15);
	IIC_SCL=0;	//SCL LOW
	delay_us_(15);
	IIC_SDA=1;	//SDA HIGH
	delay_us_(15);
	IIC_SCL=1;	//SCL HIGH
	delay_us_(15);
	SDA_IN();	//SDA Input
	for(AckLoop=0;AckLoop<4000;AckLoop++) //300uS
		{
		Ack = READ_SDA;
		
		if(Ack == 0x00)
			break;
		}
	delay_us_(15);	
	SDA_OUT();	//SDA Output	
	IIC_SCL=0;  //SCL LOW
	delay_us_(15);
	return Ack;			//return success=0 / failure=1
}

void T_WaitSCL(void)	//wait SCL all HI
{
	uint32_t i;
	Byte Ck;
	IIC_SCL=1; // SCL HIGH
	delay_us_(15);
	PulseDelay_();
	SCL_IN(); // SCL INPUT
	for(i=0;i<15000;i++) //10ms
		{
		Ck = (READ_SCL) & 0x80;
		
		if(Ck == 0x80)
			break;
		}
	SCL_OUT();// SCL Output
}


void T_ISPMode(uint8_t On)
{
	T_IIC_Start();
	T_IIC_Tx_(0x04);
	T_WaitSCL();	//wait SCL go hi or delay >10uS
	//
	T_IIC_Start();
	T_IIC_Tx_(0x34);
	T_WaitSCL();	//wait SCL go hi or delay >10uS
	//
	T_IIC_Start();
	T_IIC_Tx_(0x06);
	T_WaitSCL();	//wait SCL go hi or delay >10uS
	//
	T_IIC_Start();
	if(On)
		T_IIC_Tx_(0x36);
	else
		T_IIC_Tx_(0x3c);
	T_IIC_Stop();
	
}
