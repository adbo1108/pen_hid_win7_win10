#ifndef __SW_IIC_H__
#define __SW_IIC_H__



#include "stm32f4xx_hal.h"
#include "sys.h"
#include <stdio.h>
#include "main.h"

typedef u8 uint8_t ;

/*  PA8:SCL 
 *  PC9:SDA
 */

//IO方向
#define SDA_IN()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}	
#define SDA_OUT() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} 

#define SCL_IN() {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=0<<8*2;}
#define SCL_OUT()  {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=1<<8*2;}	

//IO Function
#define IIC_SCL    PAout(8) //SCL
#define IIC_SDA    PCout(9) //SDA	 
#define READ_SDA   PCin(9)  //input SDA 
#define READ_SCL   PAin(8)  // input SCL

void delay_us_(uint16_t delay);

//IIC Functions
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				   //發送IIC起始訊號
void IIC_Stop(void);	  			      //發送IIC結束信號
void IIC_Send_Byte(u8 txd);			//IIC發送一個byte
u8 IIC_Read_Byte(u8 ack);           //IIC讀取一個byte ,ack=1 送ACK，ack=0送nACK
u8 IIC_Wait_Ack(void); 				   //IIC等待ACK訊號
void IIC_Ack(void);					   //IIC發送ACK訊號
void IIC_NAck(void);				      //IIC不發送ACK訊號

void T_IIC_Tx (uint8_t data);

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 

/*Program NT68863 Function*/
void T_ISPMode(u8 On) ;

void Enter_ISP(void) ;
void  ISP_Process(void);

/******************************************************************************/
void Cmd_T_EnterIsp(void) ;
void test_process(void) ;

void handle_buffer() ;
	

extern uint8_t CMD_START_FLASH[] ;
extern uint8_t CMD_END_FLASH[] ;

extern uint8_t CMD_SET_BANK0[] ;
extern uint8_t CMD_SET_BANK1[] ;
extern uint8_t CMD_SET_BANK2[] ;
extern uint8_t CMD_SET_BANK3[] ;

extern uint8_t CMD_END_FW_SEND_BANK0[] ;
extern uint8_t CMD_END_FW_SEND_BANK1[] ;
extern uint8_t CMD_END_FW_SEND_BANK2[] ;
extern uint8_t CMD_END_FW_SEND_BANK3[] ;


#endif 

