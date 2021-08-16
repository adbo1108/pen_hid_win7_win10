#include <string.h>
#include "usb_device.h"
#include "usbd_custom_hid_if.h"
#include "main.h"
#include "sw_iic.h"

#define FW_UPDATE_REPORT_ID 0x03

void Send_Ack_To_PC(uint8_t* data , uint16_t size) ;

extern  uint8_t aRxBuffer[64] ;
uint8_t send_ack_buf[9];
extern UART_HandleTypeDef huart6;
volatile extern  uint8_t page_buf[512];
//extern  volatile uint8_t page_buf_full_idx ,flag_start_fill_page_buf ;
 uint8_t   page_idx ;

typedef unsigned short  Word;
typedef unsigned long	ULong;
typedef unsigned char   Byte;
#define comSetRegSRAM	0x78
#define comGetRegSRAM	0x87
#define comMultiFunc    0x99
#define comGetVersion   0x11    //new NT68670&NT68631A
#define comSPICmd       0x53
#define comChipErase	   0x1E
#define comWritePage24M	0x3C
#define comGetCheckSum  0x36    //new NT68670&NT68631A
#define comSetExtendAddr 0x0f   //new NT68670

uint8_t CMD_START_FLASH[] = { 0x0A, 0x0B,   0x1A,   0x1B,   0x2A,   0x2B,   0x3A,   0x3B,   0x4A,   0x4B,   0x00 };
uint8_t CMD_END_FLASH[] = { 0x00,	0x4B,	0x4A,	0x3B,	0x3A,	0x2B,	0x2A,	0x1B,	0x1A,	0x0B,	0x0A };


uint8_t CMD_SET_BANK0[] = { 0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };
uint8_t CMD_SET_BANK1[] = { 0x01,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };
uint8_t CMD_SET_BANK2[] = { 0x02,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };
uint8_t CMD_SET_BANK3[] = { 0x03,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };

uint8_t CMD_END_FW_SEND_BANK0[] = { 0x00,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};
uint8_t CMD_END_FW_SEND_BANK1[] = { 0x01,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};
uint8_t CMD_END_FW_SEND_BANK2[] = { 0x02,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};
uint8_t CMD_END_FW_SEND_BANK3[] = { 0x03,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};


uint16_t McuId;
Byte ErrorFlag;
// please define the WP here(according bit set 1)

Word SPageSum;		//Source Page checksum


Word usSPICmdBufAddr;
Byte ucSPI_BackUpBuf[7] = {0};
Word MaxPages;		//Max programming pages
Byte MultiID;

#define NACK 0
#define ACK 1

#define true 1
#define false 0
	
Byte DualFwEnable;
Byte Fe2pMode;	

// FE2P mode
#define NORMAL_MODE 0
#define FE2P_MODE 1
#define DIRECT_FE2P_MODE 2

typedef struct{
	uint8_t En;
	uint8_t ID1;
	uint8_t ID2;
	uint8_t ID3;
	uint8_t FE2P;
	uint8_t ByteWrite;
	uint8_t SE_Code;
	uint8_t CE_Code;
	Word Page;
	uint16_t SE_Time;
	ULong CE_Time;
	uint8_t Name[23];
}FLASH_DEF;
FLASH_DEF Flash;


#define HI_SPEED 8


typedef struct{
	Byte En;
	Byte Name[15];
	Word Id;
	Word page;
	Byte CpuClk;
	Byte IICSpeed;
	Byte SpcCmd;
	Byte NewProg;
	Byte Umb;
	Byte Fe2p;
	Byte s60k;
	Byte coP;
	Byte ExtFlash;
}MCU_DEF;

MCU_DEF  McuTypeTable[]={
//   En   Name                      Id     Pg CPUCLK IICSPD 	SpcCmd  NewProg Umb  Fe2p  60k  CoP   ExtFlash
#if 0    
	 {true,"NT68360        ",        0xA536,256,0x50 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68169        ",        0xA569,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68661        ",        0xA561,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68850        ",        0xA585,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68770        ",        0xA577,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68655        ",        0xA555,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68150        ",        0xA515,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68370        ",        0xA537,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,true, true},
    {true,"NT68790        ",        0xA790,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68658        ",        0xA658,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
    {true,"NT68380        ",        0xA380,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,true, true},
    {true,"NT68390        ",        0xA390,256,0x40 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,true, true},
    {true,"NT68810        ",        0xA810,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
#endif	
    {true,"NT68870        ",        0xA870,256,0x30 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
#if 0	 
    {true,"NT68400        ",        0xA400,256,0x40 ,HI_SPEED  ,0x02   ,true ,true, true, false,false,true},
    {true,"NT68450        ",        0xA450,256,0x40 ,HI_SPEED  ,0x02   ,true ,true ,true ,false,false,true},
#endif	 
};


FLASH_DEF  FlashIDTable[]={
//   En    ID1  ID2  ID3  FE2P  BYTE  SE   CE  PAGE SE_T CE_T  NAME
    {true ,0x55,0xaa,0x00,false,false,0xd7,0xc7,256 ,0   ,100 ,"Embeded/Unknown Flash "},
#if 0	 
    {true ,0x9d,0x7f,0x7c,true ,false,0xd7,0xc7,256 ,70  ,100 ,"PMC Pm25LV010A 128KB "},
    {true ,0x9d,0x7f,0x7d,true ,false,0xd7,0xc7,512 ,70  ,100 ,"PMC Pm25LV020A 256KB "},
    {true ,0x9d,0x7f,0x7e,true ,false,0xd7,0xc7,1024,300  ,3000 ,"PMC Pm25LV040A 512KB "}, // modify SE_T, CE_T to be compatible with Pm25LQ040B because their flash ID are the same.
    {true ,0x9d,0x7f,0x10,true ,false,0xd7,0xc7,256 ,70  ,100 ,"PMC_Pm25LD010C 128KB "},
    {true ,0x9d,0x7f,0x11,true ,false,0xd7,0xc7,512 ,70  ,100 ,"PMC_Pm25LD020C 256KB "},
    {true ,0x9d,0x7f,0x12,true ,false,0xd7,0xc7,1024,70  ,100 ,"PMC_Pm25LD040C 512KB "},
    {true ,0x9d,0x40,0x11,true ,false,0xd7,0xc7,256,300  ,1500 ,"ISSI IS25LQ010B   1MB"},
    {true ,0x9d,0x60,0x14,true ,false,0xd7,0xc7,2048,300  ,6000 ,"ISSI IS25LP080D   1MB"},
    {true ,0xc2,0x20,0x11,true ,false,0x20,0xc7,256 ,120 ,2000,"MXIC MX25L1005 128K  "},
    {true ,0xc2,0x20,0x12,true ,false,0x20,0xc7,512 ,120 ,3000,"MXIC MX25L2005 256K  "},
    {true ,0xc2,0x20,0x13,true ,false,0x20,0xc7,1024,120 ,5000,"MXIC MX25L4005A 512K "},
#endif	 
    {true ,0xc2,0x20,0x14,true ,false,0x20,0xc7,2048,300 ,15000,"MXIC MX25L8006E 1MB  "},
#if 0	 
    {true ,0xc2,0x20,0x15,true ,false,0x20,0xc7,4096,300 ,30000,"MXIC MX25L1606E 2MB  "},
    {true ,0xc2,0x20,0x17,true ,false,0x20,0xc7,2048,300 ,80000,"MXIC MX25L6406E 8MB  "}, // page set to 2048 means only read first 2048 pages, don't care other pages.
	{true ,0xc2,0x23,0x11,true ,false,0x20,0xc7, 256,240 , 2300,"MXIC MX25V1035F 128KB "},
	{true ,0xc2,0x23,0x12,true ,false,0x20,0xc7, 512,240 , 9000,"MXIC MX25V2035F 256KB "},
	{true ,0xc2,0x23,0x13,true ,false,0x20,0xc7,1024,240 , 9000,"MXIC MX25V4035F 512KB "},
	{true ,0xc2,0x23,0x14,true ,false,0x20,0xc7,2048,240 ,18000,"MXIC MX25V8035F 1MB  "},
    {true ,0xc2,0x23,0x15,true ,false,0x20,0xc7,4096,240 ,38000,"MXIC MX25V1635F 2MB  "},

	{true ,0xbf,0x49,0xff,true ,true ,0x20,0xc7,256 ,70  ,100 ,"SST SST25VF010A 128K "},
    {true ,0xbf,0x43,0xff,true ,true ,0x20,0xc7,512 ,70  ,100 ,"SST SST25VF020 256K  "},
    {true ,0xbf,0x44,0xff,true ,true ,0x20,0xc7,1024,70  ,100 ,"SST SST25LF040 512K  "},
    {true ,0xbf,0x8c,0xff,true ,true ,0x20,0xc7,512 ,30  ,60  ,"SST SST25VF020B 256K "},
    {true ,0xbf,0x8e,0xff,true ,true ,0x20,0xc7,2048 ,30  ,60  ,"PCT/SST 25VF080B 1MB "},
    {true ,0xef,0x10,0xff,true ,false,0x20,0xc7,256 ,150 ,3000,"WINBOND W25X10 128K  "},
    {true ,0xef,0x11,0xff,true ,false,0x20,0xc7,512 ,150 ,3000,"WINBOND W25X20 256K  "},
    {true ,0xef,0x12,0xff,true ,false,0x20,0xc7,1024,150 ,5000,"WINBOND W25X40 512K  "},
    {true ,0xef,0x40,0x12,true ,false,0x20,0xc7,512 ,300 ,2000,"WINBOND W25Q20CL 256K"},
    {true ,0xef,0x40,0x13,true ,false,0x20,0xc7,1024,300 ,5000,"WINBOND W25Q40CL 512K"},
    {true ,0xef,0x40,0x14,true ,false,0x20,0xc7,2048,400 ,8000,"WINBOND W25Q80BV 1MB "},
    {true ,0xef,0x70,0x14,true ,false,0x20,0xc7,2048,400 ,10000,"WINBOND W25Q80JV 1MB"},
    {true ,0xef,0x40,0x15,true ,false,0x20,0xc7,4096,400 ,10000,"WINBOND W25Q16CL/DV 2MB"},
    {true ,0xef,0x70,0x15,true ,false,0x20,0xc7,4096,400 ,25000,"WINBOND W25Q16JV 2MB"},
    {true ,0xef,0x40,0x16,true ,false,0x20,0xc7,4096,400 ,50000,"WINBOND W25Q32FV  4MB"},
    {true ,0x00,0x10,0x10,false,false,0xd8,0xc7,256 ,100 ,3000,"ST M25P10 128K       "},
    {true ,0x00,0x11,0x11,false,false,0xd8,0xc7,512 ,100 ,3000,"ST M25P20 256K       "},
    {true ,0x00,0x12,0x12,false,false,0xd8,0xc7,1024,100 ,5000,"ST M25P40 512K       "},
    {true ,0x20,0x20,0x12,false,false,0xd8,0xc7,512 ,100 ,6000,"NUMONYX M25P20  256K "},
    {true ,0x20,0x10,0x11,false,false,0xd8,0xc7,256 ,100 ,3000,"NUMONYX/ST M25P10 128K"},
    {true ,0x37,0x11,0x30,true ,false,0x20,0xc7,256 ,120 ,2000,"AMIC A25L010A(B) 128K"},
    {true ,0x37,0x12,0x30,true ,false,0x20,0xc7,512 ,120 ,4000,"AMIC A25L020A(B) 256K"},
    {true ,0x37,0x13,0x30,true ,false,0x20,0xc7,1024,120 ,6000,"AMIC A25L040A(B) 512K"},
    {true ,0x1c,0x11,0x31,true ,false,0x20,0xc7,256 ,300 ,3000,"EON EN25F10 128K     "},
    {true ,0x1c,0x12,0x31,true ,false,0x20,0xc7,512 ,300 ,4000,"EON EN25F20 256K     "},
    {true ,0x1c,0x13,0x31,true ,false,0x20,0xc7,1024,300 ,6000,"EON EN25F40 512K     "},
    {true ,0x1c,0x32,0xff,false,false,0xd8,0xc7,1024,300 ,6000,"EON EN25B40 512K     "},
    {true ,0x1c,0x13,0x30,false,false,0x20,0xc7,1024,300 ,7500,"EON EN25Q40A 512K    "},
    {true ,0x1c,0x14,0x30,false,false,0x20,0xc7,2048,300 ,15000,"EON EN25Q80B 1MB    "},
    {true ,0x1c,0x15,0x70,true ,false,0x20,0xc7,4096,300 ,30000,"EON EN25QH16A 2MB   "},
    {true ,0x8c,0x30,0x12,true ,true ,0x20,0xc7,512, 200 ,4000,"ESMT F25L02PA 256K   "},
    {true ,0x8c,0x30,0x13,true ,true ,0x20,0xc7,1024,200 ,4000,"ESMT F25L04PA 512K   "},
    {true ,0x8c,0x30,0x14,true ,true ,0x20,0xc7,2048,200 ,5000,"ESMT F25L08PT  1024K "},
    {true ,0x8c,0x31,0x12,true ,true ,0x20,0xc7,512, 200 ,4000,"ESMT F25L02PTA 256K  "},
    {true ,0x8c,0x31,0x13,true ,true ,0x20,0xc7,1024,200 ,4000,"ESMT F25L04PTA 512K  "},
    {true ,0x8c,0x31,0x14,true ,true ,0x20,0xc7,2048,200 ,5000,"ESMT F25L08PTA 1024K "},
    {true ,0x8c,0x40,0x14,true ,true ,0x20,0xc7,2048,400 ,20000,"ESMT F25L08QA 1024K "},
    {true ,0xc8,0x40,0x11,true ,false,0x20,0xc7,256, 500 ,2000,"GIGA GD25Q10 128K    "},
    {true ,0xc8,0x40,0x12,true ,false,0x20,0xc7,512, 500 ,5000,"GIGA GD25Q20 256K    "},
	{true ,0xc8,0x41,0x12,true ,false,0x20,0xc7,512, 500 ,4000,"GIGA GD25M21 256K	 "},
    {true ,0xc8,0x40,0x13,true ,false,0x20,0xc7,1024,500 ,11250, "GIGA 25Q40/41B/40C 512K"},
	{true ,0xc8,0x41,0x13,true ,false,0x20,0xc7,1024,400 ,5000, "GIGA GD25M41 512K	 "},
    {true ,0xc8,0x40,0x14,true ,false,0x20,0xc7,2048, 300 ,10000,"GIGA GD25Q80C 1MB  "},
    {true ,0xc8,0x40,0x15,true ,false,0x20,0xc7,4096, 300 ,20000,"GIGA GD25Q16C 2MB  "},
    {true ,0xd5,0x30,0x13,true ,false,0x20,0xc7,1024,200 ,7500,"NANTRONICS N25S40 512k"},
    {true ,0xa1,0x31,0x13,true ,false,0x20,0xc7,1024,300 ,10000,"FUDAN FM25F04 512k    "},
    {true ,0xa1,0x31,0x12,true ,false,0x20,0xc7,512 ,300 ,5000,"FUDAN FM25F02 256k    "},
    {true ,0xa1,0x31,0x11,true ,false,0x20,0xc7,256 ,300 ,4000,  "FUDAN FM25F01 128k    "},
    {true ,0xa1,0x40,0x12,true ,false,0x20,0xc7,512,300 ,2500,"FUDAN FM25Q02 256k    "},
    {true ,0xa1,0x40,0x13,true ,false,0x20,0xc7,1024 ,300 ,5000,"FUDAN FM25Q04 512k    "},
    {true ,0xa1,0x40,0x14,true ,false,0x20,0xc7,2048 ,300 ,32000,  "FUDAN FM25Q08 1MB    "},
    {true ,0xa1,0x40,0x15,true ,false,0x20,0xc7,4096 ,300 ,20000,  "FUDAN FM25Q16 2MB    "},
    {true ,0xe0,0x40,0x15,true ,false,0x20,0xc7,1024 ,300 ,25000,"BergMicro BG25Q40 512k"},
    {true ,0xe0,0x40,0x12,true ,false,0x20,0xc7,512 ,300 ,5000,"BergMicro BG25Q20 256k"},
    {true ,0xe0,0x40,0x11,true ,false,0x20,0xc7,256 ,300 ,5000,"BergMicro BG25Q10 128k"},
    {true ,0x1f,0x01,0x86,true ,false,0x20,0xc7,4096 ,300 ,20000,"adesto AT25SF161 2MB  "},
    {true ,0x5e,0x60,0x12,true ,false,0x20,0xc7,512 ,300 ,5000,"ZB25VQ20 256KB        "},
    {true ,0x5e,0x60,0x13,true ,false,0x20,0xc7,1024 ,300 ,5000,"ZB25VQ40 512KB        "},
    {true ,0x68,0x40,0x12,true ,false,0x20,0xc7,512 ,300 ,30000,"BoHong BH25D20A 256KB "},
    {true ,0x68,0x40,0x13,true ,false,0x20,0xc7,1024 ,300 ,30000,"BoHong BH25D40A 512KB "},
#endif	 
    {false,0xff,0xff,0xff,false,false,0xff,0   ,0   ,0   ,0   ,"END OF TABLE         "},
    };

void ack_report_err() ;

//

/*  PA8:SCL
 *  PC9:SDA
 */



/* USER CODE BEGIN 0 */

/**I2C3 GPIO Configuration
 PC9     ------> I2C3_SDA
 PA8     ------> I2C3_SCL
 */
#define HIGH  1
#define LOW   0


void T_IIC_Tx (uint8_t data);

void delay_us_(uint16_t delay) {
	volatile uint16_t delay_t = delay * 0x3;
	while (delay_t--)
		;

}

void IIC_delay() {
	delay_us_(10);
}

//初始化IIC
void IIC_Init(void) {
	IIC_SCL = 1;
	IIC_SDA = 1;
}

void IIC_Start(void) {
	SDA_OUT(); //sda output
	IIC_SDA = 1;
	delay_us_(1);
	IIC_SCL = 1;
	delay_us_(4);
	IIC_SDA = 0; //START:when CLK is high,DATA change form high to low
	delay_us_(4);
	IIC_SCL = 0; // hold住i2c, 準備接收或傳送數據
}
//產生停止信號
void IIC_Stop(void) {
	SDA_OUT(); //sda 輸出
	IIC_SCL = 0;
	IIC_SDA = 0; //STOP:when CLK is high DATA change form low to high
	delay_us_(4);
	IIC_SCL = 1;
	delay_us_(4);
	IIC_SDA = 1; //i2c結束訊號
	//delay_us_(4);
}
#if 0
//等待ACK信號到來
//返回值：1，接收ACK失敗
//        0，接收ACK成功

u8 IIC_Wait_Ack_No_Stop_Signal() {
#if 1

	u8 ucErrTime = 0; //這邊故意寫錯才可以進ISP Mode, 先workaround....
	SDA_IN(); //SDA設置輸入
	IIC_SDA = 1;
	delay_us_(20);
	IIC_SCL = 1;
	delay_us_(20);
	//SDA_IN(); //SDA設置輸入
	while (READ_SDA) {
		ucErrTime++;
		if (ucErrTime > 6000) //
				{

			return 1;
		}
	}

	IIC_SCL = 0;
	printf("a1\n\r");

	return 0;
#endif
}
#endif
u8 IIC_Wait_Ack() {
#if 1

	u16 ucErrTime = 0;
	
//	delay_us_(3);
//	IIC_SDA = 1;
	SDA_IN(); //SDA設置輸入
	delay_us_(3);
	IIC_SCL = 1;
	delay_us_(10);
	
	while (READ_SDA) {
		ucErrTime++;
		if (ucErrTime > 2500) {

			//IIC_Stop();
			ack_report_err();
			return 1;
		}
	}

	IIC_SCL = 0;
	//printf("a\n\r");
	delay_us_(8);
	return 0;
#endif

}
//產生ACK信號
void IIC_Ack(void) {
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us_(20);
	IIC_SCL = 1;
	delay_us_(20);
	IIC_SCL = 0;
}
//不產生ACK信號
void IIC_NAck(void) {
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us_(20);
	IIC_SCL = 1;
	delay_us_(20);
	IIC_SCL = 0;
}


//IIC發送一個Byte
void IIC_Send_Byte(u8 txd) {

	u8 t;
	SDA_OUT();

	IIC_SCL = 0; //拉低SCL開始數據傳輸
	for (t = 0; t < 8; t++) {
		IIC_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		delay_us_(8); //這裡delay是必須的
		IIC_SCL = 1;
		delay_us_(8);
		IIC_SCL = 0;
		delay_us_(8);
	}

}
//讀一個Byte
u8 IIC_Read_Byte(u8 ack) {
	unsigned char i, receive = 0;
	SDA_IN(); //SDA設置輸入
	for (i = 0; i < 8; i++) {
		IIC_SCL = 0;
		delay_us_(10);
		IIC_SCL = 1;
		receive <<= 1;
		if (READ_SDA)
			receive++;
		delay_us_(10);
	}
	if (!ack)
		IIC_NAck(); //送nACK
	else
		IIC_Ack(); //送ACK
	return receive;

}




void T_IIC_Tx (uint8_t data)
{
	IIC_Send_Byte(data);

	if (IIC_Wait_Ack()) {
		printf(" sw iic err 1 \n\r");
	}
}

uint8_t T_IIC_Rx(u8 ack)
{
	return IIC_Read_Byte(ack);
}

void PulseDelay()
{
	delay_us_(3);
}	
typedef union {
	uint16_t w;
	uint8_t b[2]; //0:H, 1:L
} Union2;

void T_HandShake(uint16_t timeout) {
	SCL_IN();
	HAL_Delay(timeout);
   SCL_OUT();
}
volatile uint8_t ck;

void T_GetRegSRAM(uint8_t AddrH,uint8_t AddrL) {
	ck = 0x6e^0x03^comGetRegSRAM^AddrH^AddrL;
	IIC_Start();
	IIC_Send_Byte(0x6E);

	if (IIC_Wait_Ack()) {
		printf("T_GetRegSRAM sw iic err 1 \n\r");
	}
	IIC_Send_Byte(0x03);
	if (IIC_Wait_Ack()) {
		printf("T_GetRegSRAM sw iic err 1 \n\r");
	}
	IIC_Send_Byte(comGetRegSRAM);
	if (IIC_Wait_Ack()) {
		printf("T_GetRegSRAM sw iic err 1 \n\r");
	}
	IIC_Send_Byte(AddrH);
	if (IIC_Wait_Ack()) {
		printf("T_GetRegSRAM sw iic err 1 \n\r");
	}
	IIC_Send_Byte(AddrL);
	if (IIC_Wait_Ack()) {
		printf("T_GetRegSRAM sw iic err 1 \n\r");
	}
	IIC_Send_Byte(ck);
	if (IIC_Wait_Ack()) {
		printf("T_GetRegSRAM sw iic err 1 \n\r");
	}
	
	IIC_Stop();
	
}	

uint8_t T_GetRegSRAMReply(void)
{
	uint8_t Status,Data;
	//unsigned long Data;
	IIC_Start();
	IIC_Send_Byte(0x6f);
	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}
	HAL_Delay(1); //delay >100uS for stability
	IIC_Read_Byte(1);
	Status = IIC_Read_Byte(1);;
	Data = IIC_Read_Byte(1);;
	IIC_Read_Byte(1); // sample code be 0 , nACK
	IIC_Stop();
	if((Status^comGetRegSRAM) != 0xff)
		printf("T_GetRegSRAMReply fail\n\r");
	
	HAL_Delay(10);
	return Data;
}


void T_SetRegSRAM(uint8_t AddrH, uint8_t AddrL, uint8_t Data) {

	uint8_t ck = (uint8_t)(0x6e ^ 0x04 ^ 0x78 ^ AddrH ^ AddrL ^ Data);

#if 0
	uint8_t pData[]= {0x04,0x78,AddrH,AddrL,Data,ck};
	HAL_I2C_Master_Transmit( &hi2c3, 0x6E, pData, 6, 10000 );
#endif
#if 1
	IIC_Start();
	IIC_Send_Byte(0x6E);

	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	IIC_Send_Byte(0x04);
	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	IIC_Send_Byte(comSetRegSRAM); //OP code = 0x78
	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	IIC_Send_Byte(AddrH);
	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	IIC_Send_Byte(AddrL);
	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	IIC_Send_Byte(Data);

	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	IIC_Send_Byte(ck); 
	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	IIC_Stop();
	
	

#endif

}

void T_SetRegSRAMReply() {
#if 0

	uint8_t pData[3];
	HAL_I2C_Master_Receive(&hi2c3, 0x6E, pData, 3, 10000);

#endif
#if 1
	uint8_t ReplyCode;
	IIC_Start();
	IIC_Send_Byte(0x6f);
	if (IIC_Wait_Ack()) {
		printf("T_SetRegSRAM sw iic err 1 \n\r");
	}

	HAL_Delay(1); //delay >100uS for stability
	IIC_Read_Byte(1); //送ACK，收
	ReplyCode = IIC_Read_Byte(1);
	//IIC_Read_Byte(0);
	IIC_Read_Byte(1);
	IIC_Stop();
	if ((ReplyCode ^ comSetRegSRAM) != 0xff)
		printf("T_SetRegSRAMReply err \n\r");
	
	HAL_Delay(20);
#endif
}

void Cmd_T_SendSpecialCommand() {
	uint8_t i, No, temp, Dat;
	Union2 Ad;
	uint16_t NT810_870Table[] =
	{

			0xf054, 0x00, 
			0xf04f, 0x11, 
			0xf050, 0x90, 
			0xf053, 0x83, 
			0x8b57, 0xff, 
			0x8b04, 0x00, 
			0x80d0, 0x41, 
			0xf022, 0x00, 
			0xf024, 0x00, // don't set 0xB46 = 0x00
			0xf050, 0x91, 
			0x8b57, 0xff, 
			0x8b3e, 0xf2, 
			0x8b3f, 0x51,
			0x8b45, 0x60, 
	};
	//printf("loop=%d\n\r",sizeof(NT810_870Table)/sizeof(uint16_t));
	for (i = 0; i < 28; i += 2) {
		Ad.w = NT810_870Table[i];
		Dat = NT810_870Table[i + 1];
		//printf("H=%x,L=%x,D=%x\n\r",Ad.b[1],Ad.b[0],Dat);
		T_SetRegSRAM(Ad.b[1], Ad.b[0], Dat); //ex. AD.b[0]=0xf0,AD.b[1]=0x54,Dat.b[1]=0x00

		T_HandShake(7); //wait SCL go High for 5mS
		T_SetRegSRAMReply();
		HAL_Delay(50);

	}
//	if (McuId == 0xa870) 
	{
		T_SetRegSRAM(0xF0, 0x80, 0x00);
		T_HandShake(7); //wait SCL go High for 5mS
		T_SetRegSRAMReply();
		HAL_Delay(50);
	}
	// set cpu clock
	T_SetRegSRAM(0xF0, 0x4C, 0x30);
	T_HandShake(7); //wait SCL go High for 5mS
	T_SetRegSRAMReply();
	HAL_Delay(50);
	
	/* Tony Add base on analyser */
	T_SetRegSRAM(0xF0, 0x1C, 0x48);
	T_HandShake(7); //wait SCL go High for 5mS
	T_SetRegSRAMReply();

	HAL_Delay(50); // T_HandShake(50ms);

}


void ISP_Reply() {
	uint8_t length, ID_H, ID_L, ck;
	printf("ISP_Reply start \n\r");
	IIC_Start();
	IIC_Send_Byte(0x6f);
	if (IIC_Wait_Ack()) {
		printf("ISP_Reply sw iic err 1 \n\r");
	}

	length = IIC_Read_Byte(1); //送ACK，收

	ID_H = IIC_Read_Byte(1); //送ACK, 收

	ID_L = IIC_Read_Byte(1); //送ACK, 收

	//ck = IIC_Read_Byte(0); //送NACK, 收
	ck = IIC_Read_Byte(1); //送NACK, 收

	IIC_Stop();

	McuId = (ID_H << 8) + ID_L;
	printf("MCU ID=%x \n\r", McuId);
}




void FnSetSPICmdBufAndBackUpValue()
{
	Byte Dat, tmp, i;

	T_HandShake(10);
	T_GetRegSRAM(0xa0,0x00);
    T_HandShake(5);
    Dat = T_GetRegSRAMReply();
	tmp = Dat;
	
	T_SetRegSRAM(0xa0,0x00,0xaa);
	T_HandShake(5);
	T_SetRegSRAMReply();

	T_HandShake(10);
	T_GetRegSRAM(0xa0,0x00);
	T_HandShake(5);
	Dat = T_GetRegSRAMReply();	
	if (Dat == 0xaa) {
		T_SetRegSRAM(0xa0,0x00,0x55);
		T_HandShake(5);
		T_SetRegSRAMReply();
		T_HandShake(100);
		T_GetRegSRAM(0xa0,0x00);
		T_HandShake(5);
		Dat = T_GetRegSRAMReply();	

		if (Dat == 0x55) {
			usSPICmdBufAddr = 0xa800;
		}
		else
			usSPICmdBufAddr = 0x9800;
	}
	else {
		usSPICmdBufAddr = 0x9800;
	}
	T_SetRegSRAM(0xa0,0x00,0x55);
	T_HandShake(5);
	T_SetRegSRAMReply();
	T_HandShake(10);

	// back up f0c0~f0c6
	for (i=0; i<sizeof(ucSPI_BackUpBuf); i++) {
		T_GetRegSRAM(0xf0,0xc0+i);
		T_HandShake(5);
		Dat = T_GetRegSRAMReply();	
		ucSPI_BackUpBuf[i] = Dat;
		//printf("%x ",(Word)Dat);
	}
}

void T_RunNewSPICmd(void)
{
	Byte CheckSum;
	
	IIC_Start();
	IIC_Send_Byte(0x6e);
	if (IIC_Wait_Ack()) {
		printf("T_RunNewSPICmd sw iic err 1 \n\r");
	}
	IIC_Send_Byte(0x01);
	if (IIC_Wait_Ack()) {
		printf("T_RunNewSPICmd sw iic err 1 \n\r");
	}
	IIC_Send_Byte(0x25);
	if (IIC_Wait_Ack()) {
		printf("T_RunNewSPICmd sw iic err 1 \n\r");
	}
	
	CheckSum = 0x6e^0x01^0x25;
	IIC_Send_Byte(CheckSum);
	if (IIC_Wait_Ack()) {
		printf("T_RunNewSPICmd sw iic err 1 \n\r");
	}
	
	IIC_Stop();
	
}


void T_RunNewSPICmdReply(void)
{
	Byte Status;
	
	IIC_Start();
	T_IIC_Tx(0x6f);
	PulseDelay();
	PulseDelay();
	T_IIC_Rx(ACK);
	Status = T_IIC_Rx(ACK);
	T_IIC_Rx(NACK);
	IIC_Stop();
	if((Status^0x25) != 0xff)
		printf("T_RunNewSPICmdReply\n\r");
	
	
}
Byte SearchFlashID(Byte mcuFlashID[3])
{
    Byte i,j;
    Byte m1,m2,m3;
	
    for(i=0;i<128;i++)
	{
        m1 = 0;
		m2 = 0;
		m3 = 0;
        if(FlashIDTable[i].ID1 == 0xff)
        	break;
        for(j=0;j<3;j++)
    	{
        	if(mcuFlashID[j] == FlashIDTable[i].ID1 && !m1)
            	m1 = 1;
            else
                if(mcuFlashID[j] == FlashIDTable[i].ID2 && !m2)
                	m2 = 1;
            else
                if(mcuFlashID[j] == FlashIDTable[i].ID3 && !m3)
                	m3 = 1;
        }
        if(FlashIDTable[i].ID3 == 0xff)
        	m3 = 1;
        if(m1 && m2 && m3)
			return i;
    }
	return 0xff;
}

Byte T_MultiFunc(Byte Data)
{
	Byte ack;
	
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x02);
	T_IIC_Tx(comMultiFunc);
	T_IIC_Tx(Data);
	T_IIC_Tx(0x6E^0x02^comMultiFunc^Data);
	IIC_Stop();
	
	return ack;
}

void T_MultiFuncReply()
{
	Byte Status;
	
	IIC_Start();
	T_IIC_Tx(0x6f);
	PulseDelay();
	PulseDelay();
	T_IIC_Rx(ACK);
	Status = T_IIC_Rx(ACK);
	T_IIC_Rx(NACK);
	IIC_Stop();
	
	if(Status == 0x66)
	{
		printf("T_MultiFuncReply ok \n\r");
	}
		
	else {
		printf("T_MultiFuncReply fail \n\r");
	}
}	

uint8_t CheckSpecialFlash(void)
{
    Byte WrtCntTab[16] = {0};
    Byte WData[16] = {0};
	Byte WrtCntTabLength = 0;
	Byte WDataLength = 0;
    Word SPI_Cmd_Buf_Addr = usSPICmdBufAddr;
    Word TmpAddr;
    Word i;
    Byte F0C0_value, dummy;
    Byte RData[16] = {0};

	Byte Id;
	Byte fID[3];
	Byte ret;

	// Setup write count table and write data sequence according to different flash spec.
	// WrtCntTabLength and WDataLength will accumulate automatically.
	WrtCntTab[WrtCntTabLength++] = 1;
	WrtCntTab[WrtCntTabLength++] = 3;
	WData[WDataLength++] = 0x9f;

    TmpAddr = SPI_Cmd_Buf_Addr;
    F0C0_value = (WrtCntTabLength/2)<<4|0x03;
	
	T_SetRegSRAM(0xf0,0xc0,F0C0_value);
	T_HandShake(5);
	T_SetRegSRAMReply();

	T_SetRegSRAM(0xf0,0xc1,SPI_Cmd_Buf_Addr>>8);
	T_HandShake(5);
	T_SetRegSRAMReply();

    dummy = SPI_Cmd_Buf_Addr&0xFF;	
	T_SetRegSRAM(0xf0,0xc2,dummy);
	T_HandShake(5);
	T_SetRegSRAMReply();
	
	T_SetRegSRAM(0xf0,0xc3,SPI_Cmd_Buf_Addr>>8);
	T_HandShake(5);
	T_SetRegSRAMReply();

    dummy = (SPI_Cmd_Buf_Addr + WrtCntTabLength)&0xFF;	
	T_SetRegSRAM(0xf0,0xc4,dummy);
	T_HandShake(5);
	T_SetRegSRAMReply();

	T_SetRegSRAM(0xf0,0xc5,SPI_Cmd_Buf_Addr>>8);
	T_HandShake(5);
	T_SetRegSRAMReply();

    dummy = (SPI_Cmd_Buf_Addr + WrtCntTabLength + WDataLength)&0xFF;
	T_SetRegSRAM(0xf0,0xc6,dummy);
	T_HandShake(5);
	T_SetRegSRAMReply();

    for (i=0; i<WrtCntTabLength; i++) {		
		T_SetRegSRAM((Byte)(TmpAddr>>8),(Byte)TmpAddr,WrtCntTab[i]);
		T_HandShake(5);
		T_SetRegSRAMReply();
		TmpAddr++;
    }
    for (i=0; i<WDataLength; i++) {
		T_SetRegSRAM((Byte)(TmpAddr>>8),(Byte)TmpAddr,WData[i]);
		T_HandShake(5);
		T_SetRegSRAMReply();
		TmpAddr++;
    }

    // send New SPI Command.
   T_RunNewSPICmd();
	T_HandShake(100);
	T_RunNewSPICmdReply();
	T_HandShake(200);

    for (i=0; i<sizeof(RData); i++) {		
		T_GetRegSRAM((Byte)((SPI_Cmd_Buf_Addr+i)>>8),(Byte)(SPI_Cmd_Buf_Addr+i));
		T_HandShake(5);
		RData[i] = T_GetRegSRAMReply();	
		//printf("RData[%d]=%x\r\n", (Word)i, (Word)RData[i]);
    }
	
	// Read return data and identify which flash
	// W25Q16DV flash
	fID[0] = RData[WrtCntTabLength + WDataLength];
	fID[1] = RData[WrtCntTabLength + WDataLength+1];
	fID[2] = RData[WrtCntTabLength + WDataLength+2];
	 
	// printf("fID[0]=%x ,fID[1]=%x ,fID[2]=%x  \n\r",fID[0],fID[1],fID[2]);
	

	Id = SearchFlashID(fID);
	if(Id != 0xff)
	{
	    Flash = FlashIDTable[Id];    //default flash PMC010A W/O fe2p
		MaxPages = Flash.Page;
		T_SetRegSRAM(0xf0,0x67,Flash.SE_Code);
		T_HandShake(5);
		T_SetRegSRAMReply();
		
		//MultiID = 0x04;	//MX2026 CMD if use this flash
		MultiID = 0x00;
		
		if(Flash.ByteWrite  == 0)
			MultiID |= 0x02;
		ret = T_MultiFunc(MultiID);	//Byte Write
		T_HandShake(5);
		T_MultiFuncReply();
		HAL_Delay(20);
		
		return true;
	}
	
    return false;
}

void RestoreSPIRegValue()
{
	Byte i;

	for (i=0; i<sizeof(ucSPI_BackUpBuf); i++) {		
		T_SetRegSRAM(0xf0,0xc0+i,ucSPI_BackUpBuf[i]);
		T_HandShake(5);
		T_SetRegSRAMReply();
		T_HandShake(10);
	}
}

void T_GetVersion(void)	//for Get Flash ID
{
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x01);
	T_IIC_Tx(comGetVersion);
	T_IIC_Tx(0x6e^0x01^0x11);
	IIC_Stop();
}


void Cmd_T_GetFlashID()
{
	uint8_t SPI_ID_Tab[]={0x90,0x9f,0xab};
	uint8_t i,l,Id;
	uint8_t Data ;
	uint8_t fID[3];
	uint16_t ret;
	
	Flash.En = 0;
	// use new spi command to get flash id
	FnSetSPICmdBufAndBackUpValue();
	
	
	if (CheckSpecialFlash()) {
        RestoreSPIRegValue();            
		return;
	}
	
	
	
	
	/**下面都不會跑  **/
	
	
	RestoreSPIRegValue();
	   for(l=0;l<3;l++)
    {
		T_SetRegSRAM(0xf0,0x68,SPI_ID_Tab[l]);
		T_HandShake(5);
		T_SetRegSRAMReply();
	
		T_GetVersion();
		T_HandShake(20);	//delay 20mS
        for(i=0;i<3;i++)
        {
            T_GetRegSRAM(0xf0,0x64+i);
            T_HandShake(5);
            Data = T_GetRegSRAMReply();
		
			fID[i] = Data;	
            if(fID[0] == 0x00 || fID[0] == 0xff)
                break;
        }
		  
		
		Id = SearchFlashID(fID);
		 
		
		if(Id != 0xff)
		{
		   Flash = FlashIDTable[Id];    //default flash PMC010A W/O fe2p
			MaxPages = Flash.Page;
			printf("MAXPages = %d\n\r",MaxPages);
			T_SetRegSRAM(0xf0,0x67,Flash.SE_Code);
			T_HandShake(5);
			T_SetRegSRAMReply();
			//MultiID = 0x04;	//MX2026 CMD if used.
			MultiID = 0x00;
			if(Flash.ByteWrite  == 0)
				MultiID |= 0x02;
			
			T_MultiFunc(MultiID);	//Byte Write
			T_HandShake(5);
			return;
		}
	}
	
}	

void WpEnable(void)
{

		uint8_t temp ;
	/*0xF007 , General I/O Ports R/W Direction Control Registers*/
		T_GetRegSRAM(0xf0,0x07);
	
		T_HandShake(5); //wait SCL go High for 5mS
		temp = T_GetRegSRAMReply();	
		temp &= ~(1<<0) ;
		HAL_Delay(10);
		        
		T_SetRegSRAM(0xf0,0x07,temp);
		T_HandShake(5); //wait SCL go High for 5mS
		T_SetRegSRAMReply();
		HAL_Delay(2);
	
	
	/*0xF002,General I/O Ports Control Registers*/
		T_GetRegSRAM(0xf0,0x02);	
		T_HandShake(5); //wait SCL go High for 5mS
		temp = T_GetRegSRAMReply();	
		temp |= (1<<0) ;
		HAL_Delay(10);
		        
		T_SetRegSRAM(0xf0,0x02,temp);
		T_HandShake(5); //wait SCL go High for 5mS
		T_SetRegSRAMReply();
		HAL_Delay(2);
		
	/*0xF0b2, General I/O Ports Push-Pull Setting */
		T_GetRegSRAM(0xf0,0xb2);	
		T_HandShake(5); //wait SCL go High for 5mS
		temp = T_GetRegSRAMReply();
		temp |= (1<<0) ;
		HAL_Delay(10);
		        
		T_SetRegSRAM(0xf0,0xb2,temp);
		T_HandShake(5); //wait SCL go High for 5mS
		T_SetRegSRAMReply();
		HAL_Delay(2);
	
}



void T_SpiCommand(Byte *Data,Byte WLen,Byte RLen)
{
    Byte i;
    Byte Length,chksum;
    Length = (RLen<<4) | WLen;
	
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x09);
	T_IIC_Tx(0x53);
   chksum = 0x6e^0x09^0x53;
	for(i=0;i<7;i++)
    	{
		T_IIC_Tx(*Data);
	    chksum ^= *Data;
    	Data++;
    	}
	T_IIC_Tx(Length);
	chksum ^= Length;
	T_IIC_Tx(chksum);
	IIC_Stop();
}

void T_SpiCommandReply()
{
	Byte ReplyCode;
	IIC_Start();
	T_IIC_Tx(0x6f);
	PulseDelay();	//delay >100us
	T_IIC_Rx(ACK);
	ReplyCode = T_IIC_Rx(ACK);
	T_IIC_Rx(NACK);
	IIC_Stop();
   if((ReplyCode^comSPICmd)!=0xFF)
	{	
		ErrorFlag = 1;
		printf("T_SpiCommandReply FAIL \n\r");
	}	
}

void Cmd_T_BlockProtect(uint8_t Wp)
{
	Byte i,Cmd[7],sta;
	for(i=0;i<7;i++)
		Cmd[i] = 0;
	if(Flash.ByteWrite)	//SST Only
		Cmd[0] = 0x50;
	else
		Cmd[0] = 0x06;

    if(Wp)
        sta = 0x9c;
    else
        sta = 0;
    T_SpiCommand(Cmd,1,0);	//WEN
	T_HandShake(20);	//timeout 20mS
	T_SpiCommandReply();
	 
	 HAL_Delay(10);
	 
    Cmd[0] = 0x01;
    Cmd[1] = sta;
    T_SpiCommand(Cmd,2,0);	//WRSR
	T_HandShake(20);	//timeout 20mS
	T_SpiCommandReply();
	//Sleep(1200);	//delay 120mS for Flash block protect time
	T_HandShake(Flash.SE_Time);	//120mS
	// printf("Flash.SE_Time = %d\n\r",Flash.SE_Time);
	 T_HandShake(130);
}


Byte T_ChipErase(void)
{
	Byte Ack,CheckSum;
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x07);
	T_IIC_Tx(comChipErase);
	T_IIC_Tx(0xaa);
	T_IIC_Tx(0x55);
	T_IIC_Tx(0xcc);
	T_IIC_Tx(0x33);
	T_IIC_Tx(0xe2);
	T_IIC_Tx(0x1d);
	CheckSum = 0x6e^0x07^comChipErase^0xaa^0x55^0xcc^0x33^0xe2^0x1d;
	T_IIC_Tx(CheckSum);
	IIC_Stop();
	return Ack;
}

Byte T_ChipEraseReply(void)
{
	Byte ReplyCode;
	IIC_Start();
	T_IIC_Tx(0x6f);
	PulseDelay();	//delay > 100us
	T_IIC_Rx(ACK);	//length, no used
	ReplyCode = T_IIC_Rx(ACK);	//status
	T_IIC_Rx(NACK);	//Checksum, no used
	IIC_Stop();
	//if((ReplyCode^comChipErase) != 0xff)
	if(ReplyCode != 0xE1)
	{	
		ErrorFlag = 1;
		printf("erase reply err \n\r");
	}else{
		printf("erase reply done \n\r");
	}		
}

void Cmd_T_Erase(void){

		T_ChipErase();
		//T_HandShake(Flash.CE_Time);	
		T_HandShake(10000);	
		T_ChipEraseReply();
}

void T_WritePageDataReply(void)
{
	Byte ReplyCode;//Dummy,;
	IIC_Start();
	T_IIC_Tx(0x6f);
	PulseDelay();	//delay > 100us
	T_IIC_Rx(ACK);	//length
	ReplyCode = T_IIC_Rx(ACK);	//status
	T_IIC_Rx(NACK);	//Checksum
	IIC_Stop();
	if((ReplyCode^comWritePage24M) != 0xff)
		ErrorFlag = 1;
}

Byte T_WritePagedata(Byte Pg, Byte* buf )
{
	Byte CheckSum ,Ack;
	uint16_t i ;
	CheckSum = 0x6e^0x00^comWritePage24M^Pg^0xf0^0x0f^0x6e^0x91^0x13^0xec;
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x00);
	T_IIC_Tx(comWritePage24M);
	T_IIC_Tx(Pg);
	T_IIC_Tx(0xf0);
	T_IIC_Tx(0x0f);
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x91);
	T_IIC_Tx(0x13);
	T_IIC_Tx(0xec);
	for(i=0;i<512;i++)
	{
		
		T_IIC_Tx(buf[i]);
		CheckSum ^= buf[i];
	}
	T_IIC_Tx(CheckSum);
	IIC_Stop();
	return Ack;
}

Byte T_GetCheckSumReply(void)
{
	Byte ReplyCode,ChkH,ChkL ,ck;

	IIC_Start();
	T_IIC_Tx(0x6f);
	PulseDelay();	//delay >100us
	T_IIC_Rx(ACK);	//length
	ReplyCode = T_IIC_Rx(ACK);	//length
	ChkH = T_IIC_Rx(ACK);	//checksum from scaler msb
	ChkL = T_IIC_Rx(ACK);	//checksum from scaler lsb
	ck = T_IIC_Rx(NACK);
	IIC_Stop();
	return ck ;
#if 0	
	if(SPageSum != (Word)(ChkH*0x100+ChkL))
     	ErrorFlag = 1;
   	if((ReplyCode^comGetCheckSum)!=0xFF)
     	ErrorFlag = 1;
#endif	
}


Byte T_GetCheckSum(Word PageNo)
{
	Byte CheckSum,tmp;
   tmp = PageNo%256;
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x02);
	T_IIC_Tx(comGetCheckSum);
	T_IIC_Tx(tmp);
	CheckSum = 0x6e^0x02^comGetCheckSum^tmp;
	T_IIC_Tx(CheckSum);
	IIC_Stop();
	return CheckSum ;
}

Byte Cmd_T_GetCheckSum(Word Pg)
{
	Byte ck ;
	T_GetCheckSum(Pg);
	T_HandShake(50);	//handshake timeout 200ms
	ck = T_GetCheckSumReply();
	HAL_Delay(2);
	return ck ;
}
void T_GetPageData(Byte PageNo)
{
	Byte CheckSum;
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x02);
	T_IIC_Tx(0x4b);
	T_IIC_Tx(PageNo);
	CheckSum = 0x6e^0x02^0x4b^PageNo;
	T_IIC_Tx(CheckSum);
	IIC_Stop();
	
}

void T_GetPageData_Reply()
{
	uint16_t i ;
	IIC_Start();
	T_IIC_Tx(0x6f);
	T_IIC_Rx(ACK); // length
	T_IIC_Rx(ACK);//Command,should be 0xB5
	for (i=0;i<512;i++)
	{
		printf(",0x%x, ",T_IIC_Rx(ACK));
	}	
	T_IIC_Rx(ACK);//checksum
	
	IIC_Stop();
}	




void Enter_ISP() {
	uint8_t i,data;
	data=0;
	T_ISPMode(1);
	
	/* delay for wait scaler enter ISP Mode*/
	HAL_Delay(500);

	ISP_Reply();
	HAL_Delay(50);
#if 0
	MX_I2C3_Init();
#endif
	Cmd_T_SendSpecialCommand();
	delay_us_(5);
	Cmd_T_GetFlashID();

	delay_us_(5);
	WpEnable();
	delay_us_(5);
	Cmd_T_BlockProtect(0); 
	delay_us_(5);

}


void Cmd_T_Read_Page (uint8_t page)
{
	T_GetPageData(page);
	T_HandShake(300);
	T_GetPageData_Reply();
	
}	


void Cmd_T_Program(Word Pg, Byte* UseBuffer)
{
	T_WritePagedata(Pg, UseBuffer);
	//T_HandShake(200);	//Handshake time out 200mS, W25x40b program time 3ms x 2pages(256bytes)
	T_HandShake(50);
	T_WritePageDataReply();
	HAL_Delay(3);
	
}


void T_SetExtendAddrReply()
{
	Byte ReplyCode;
	IIC_Start();
	T_IIC_Tx(0x6f);
	//PulseDelay();	//delay >100us
	delay_us_(100);
	T_IIC_Rx(ACK);
	ReplyCode = T_IIC_Rx(ACK);
	T_IIC_Rx(ACK);
	T_IIC_Rx(NACK);
	IIC_Stop();
   	if((ReplyCode^comSetExtendAddr)!=0xFF)
     	ErrorFlag = 1;
}


void T_SetExtendAddr(Byte Block)
{
	Byte CheckSum;
	IIC_Start();
	T_IIC_Tx(0x6e);
	T_IIC_Tx(0x02);
	T_IIC_Tx(comSetExtendAddr);	//op code =0x0f
	T_IIC_Tx(Block);
	CheckSum = 0x6e^0x02^comSetExtendAddr^Block;
	T_IIC_Tx(CheckSum);
	IIC_Stop();
}


/* Set Bank*/
void Cmd_T_SetBank(Byte bank)
{

	T_SetExtendAddr(bank);
	delay_us_(300);	//delay > 300us
	T_SetExtendAddrReply();
	
}



void ack_report_err()
{
	memset(send_ack_buf,0xFF,9);
	send_ack_buf[0]= FW_UPDATE_REPORT_ID;
	send_ack_buf[1]=0xFE;
	send_ack_buf[2]=0xF1;
	send_ack_buf[3]=0xF2;
	send_ack_buf[4]=0xF3;
	
	Send_Ack_To_PC(send_ack_buf,9);
	
}	



void ack_ready_to_flash()
{
	
	memset(send_ack_buf,0xFF,9);
	send_ack_buf[0]= FW_UPDATE_REPORT_ID;
	send_ack_buf[1]=0xFE;
	send_ack_buf[2]=0x0F;
	send_ack_buf[3]=0xFF;
	
	flag_start_fill_page_buf = 0;
	
	Send_Ack_To_PC(send_ack_buf,9);
}	

void set_bank0()
{
	//uint8_t CMD_SET_BANK0[] = { 0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };
	flag_start_fill_page_buf = 0;
	/*Wait SET BANK0 CMD*/
	while(1){
		if(memcmp(aRxBuffer,CMD_SET_BANK0,11)== 0)
		{
			
			break;
		}
	}
	
	Cmd_T_SetBank(0);
	
	HAL_Delay(1);
	
	/*Ack SET BANK0 Success */
	memset(send_ack_buf,0x00,9);
	send_ack_buf[0]= FW_UPDATE_REPORT_ID;
	send_ack_buf[1]=0xFE;
	

	Send_Ack_To_PC(send_ack_buf,9);
	
	//HAL_Delay(10); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
	
	flag_start_fill_page_buf = 1;
	page_idx = 0;
	page_buf_full_idx= 0;
	
	printf("set bank 0 \n\r");
}	

void flash_bank0_fw()
{	
	//uint8_t CMD_END_FW_SEND_BANK0[] = { 0x00,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};
	uint8_t checksum;
	uint16_t i;
	
	while(1)
	{
		if(memcmp(aRxBuffer,CMD_END_FW_SEND_BANK0,11)== 0)
		{
				
			flag_start_fill_page_buf = 0;
			//USBD_CUSTOM_HID_SendReport_FS(msg,8) ;
			break;
		}
		
		if(page_buf_full_idx == 16)
		{
			flag_start_fill_page_buf = 0;
			Cmd_T_Program(page_idx,page_buf);
			
			HAL_Delay(1);
		//	debug_msg[0]= page_idx ;
		//	USBD_CUSTOM_HID_SendReport_FS(debug_msg,8) ;
		//	USBD_CUSTOM_HID_SendReport_FS(page_buf,8) ;
			
			Cmd_T_GetCheckSum(page_idx);
			HAL_Delay(1);
			
			for(i=0;i<512;i++)
			{
				checksum = checksum+page_buf[i];
			}	
			page_buf_full_idx = 0;
			/*這邊寫完一個Page就要回ACK給PC*/
			memset(send_ack_buf,0x00,9);
			send_ack_buf[0]=FW_UPDATE_REPORT_ID;
			send_ack_buf[1]=0xFE;
			send_ack_buf[2]=0xAB;
			send_ack_buf[3]=checksum;
			Send_Ack_To_PC(send_ack_buf,9);
			
		//	printf(".");
			
		//	HAL_Delay(2); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
			
			flag_start_fill_page_buf = 1;
			
			page_idx ++ ;
			
		}		
	}
	
	flag_start_fill_page_buf = 0;
	
	page_idx = 0x80;
}	




void set_bank1()
{
	//uint8_t CMD_SET_BANK1[] = { 0x01,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };
	
	/*Wait SET BANK1 CMD*/
	while(1){
		if(memcmp(aRxBuffer,CMD_SET_BANK1,11)== 0)
		{
			
			break;
		}
	}
	
	//Cmd_T_SetBank(1);
	
	HAL_Delay(1);
	
	/*Ack SET BANK1 Success */
	memset(send_ack_buf,0x00,9);
	send_ack_buf[0]=FW_UPDATE_REPORT_ID;
	send_ack_buf[1]=0xFE;
	send_ack_buf[2]=0x01;
	
	
	Send_Ack_To_PC(send_ack_buf,9);
//	HAL_Delay(2); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
	
	flag_start_fill_page_buf = 1;
	page_idx = 0x80;
	page_buf_full_idx= 0;
	printf("set bank 1 \n\r");
}	

void flash_bank1_fw()
{	
	//uint8_t CMD_END_FW_SEND_BANK1[] = { 0x01,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};
	
	uint8_t checksum;
	uint16_t i;
	
	while(1)
	{
		if(memcmp(aRxBuffer,CMD_END_FW_SEND_BANK1,11)== 0)
		{
				
			flag_start_fill_page_buf = 0;
			//USBD_CUSTOM_HID_SendReport_FS(msg,8) ;
			break;
		}
		
		if(page_buf_full_idx == 16)
		{
			flag_start_fill_page_buf = 0;
			Cmd_T_Program(page_idx,page_buf);
			HAL_Delay(1);
		//	debug_msg[0]= page_idx ;
		//	USBD_CUSTOM_HID_SendReport_FS(debug_msg,8) ;
		//	USBD_CUSTOM_HID_SendReport_FS(page_buf,8) ;
			
			Cmd_T_GetCheckSum(page_idx);
			HAL_Delay(1);
			
			
			page_buf_full_idx = 0;
			for(i=0;i<512;i++)
			{
				checksum = checksum+page_buf[i];
			}	
			/*這邊寫完一個Page就要回ACK給PC*/
			memset(send_ack_buf,0x00,9);
			send_ack_buf[0]=FW_UPDATE_REPORT_ID;
			send_ack_buf[1]=0xFE;
			send_ack_buf[2]=0xAB;
			send_ack_buf[3]=checksum;
			Send_Ack_To_PC(send_ack_buf,9);
	//		printf(".");
	//		HAL_Delay(2); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
			flag_start_fill_page_buf = 1;
			
			page_idx ++ ;
			
		}		
	}
	
	
	flag_start_fill_page_buf = 0;
	page_idx = 0;
}	



void set_bank2()
{
	uint8_t CMD_SET_BANK2[] = { 0x02,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };
	
	/*Wait SET BANK2 CMD*/
	while(1){
		if(memcmp(aRxBuffer,CMD_SET_BANK2,11)== 0)
	
		{
			
			break;
		}
	}
	
	Cmd_T_SetBank(1);
	
	HAL_Delay(1);
	
	/*Ack SET BANK2 Success */
	memset(send_ack_buf,0x00,9);
	send_ack_buf[0]=FW_UPDATE_REPORT_ID;
	send_ack_buf[1]=0xFE;
	send_ack_buf[2]=0x02;
	
	
	Send_Ack_To_PC(send_ack_buf,9);
	
	HAL_Delay(2); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
	flag_start_fill_page_buf = 1;
	page_idx = 0;
	page_buf_full_idx= 0;
	
	printf("set bank 2 \n\r");
}	

void flash_bank2_fw()
{	
	//uint8_t CMD_END_FW_SEND_BANK2[] = { 0x02,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};
	
	uint8_t checksum;
	uint16_t i;
	
	while(1)
	{
		if(memcmp(aRxBuffer,CMD_END_FW_SEND_BANK2,11)== 0)
		{
				
			flag_start_fill_page_buf = 0;
			//USBD_CUSTOM_HID_SendReport_FS(msg,8) ;
			break;
		}
		
		if(page_buf_full_idx == 16)
		{
			flag_start_fill_page_buf = 0;
			Cmd_T_Program(page_idx,page_buf);
			HAL_Delay(1);
			
			Cmd_T_GetCheckSum(page_idx);
			HAL_Delay(1);
			
			page_buf_full_idx = 0;
			for(i=0;i<512;i++)
			{
				checksum = checksum+page_buf[i];
			}	
			/*這邊寫完一個Page就要回ACK給PC*/
			memset(send_ack_buf,0x00,9);
			send_ack_buf[0]=FW_UPDATE_REPORT_ID;
			send_ack_buf[1]=0xFE;
			send_ack_buf[2]=0xAB;
			send_ack_buf[3]=checksum;
			
			Send_Ack_To_PC(send_ack_buf,9);
		//	printf(".");
			
			//HAL_Delay(2); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
			flag_start_fill_page_buf = 1;
			
			page_idx ++ ;
			
		}		
	}
	
	
	flag_start_fill_page_buf = 0;
	page_idx = 0x80;
}



void set_bank3()
{
	//uint8_t CMD_SET_BANK3[] = { 0x03,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07,	0x08,	0x09,	0x0A };
	
	/*Wait SET BANK2 CMD*/
	while(1){
		if(memcmp(aRxBuffer,CMD_SET_BANK3,11)== 0)
		{
			
			break;
		}
	}
	
	//Cmd_T_SetBank(3);
	
	HAL_Delay(1);
	
	/*Ack SET BANK2 Success */
	memset(send_ack_buf,0x00,9);
	send_ack_buf[0]=FW_UPDATE_REPORT_ID;
	send_ack_buf[1]=0xFE;
	send_ack_buf[2]=0x03;
	
	Send_Ack_To_PC(send_ack_buf,9);
	
	//HAL_Delay(2); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
	flag_start_fill_page_buf = 1;
	page_idx = 0x80;
	page_buf_full_idx= 0;
	
	printf("set bank 3 \n\r");
}	

void flash_bank3_fw()
{	
	//uint8_t CMD_END_FW_SEND_BANK3[] = { 0x03,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB,	0xAA,	0xBB};
	
	uint8_t checksum;
	uint16_t i;
	
	while(1)
	{
		if(memcmp(aRxBuffer,CMD_END_FW_SEND_BANK3,11)== 0)
		{
				
			flag_start_fill_page_buf = 0;
			//USBD_CUSTOM_HID_SendReport_FS(msg,8) ;
			break;
		}
		
		if(page_buf_full_idx == 16)
		{
			flag_start_fill_page_buf = 0;
			Cmd_T_Program(page_idx,page_buf);
			HAL_Delay(1);
			
			Cmd_T_GetCheckSum(page_idx);
			HAL_Delay(1);
			page_buf_full_idx = 0;
			for(i=0;i<512;i++)
			{
				checksum = checksum+page_buf[i];
			}	
			/*這邊寫完一個Page就要回ACK給PC*/
			memset(send_ack_buf,0x00,9);
			send_ack_buf[0]=FW_UPDATE_REPORT_ID;
			send_ack_buf[1]=0xFE;
			send_ack_buf[2]=0xAB;
			send_ack_buf[3]=checksum;
			
			Send_Ack_To_PC(send_ack_buf,9);
		//	printf(".");
			
		//	HAL_Delay(2); //must delay, 變數sync問題!!!!!!!!!!!!!!!!
			flag_start_fill_page_buf = 1;
			
			page_idx ++ ;
			
		}		
	}
	
	
	flag_start_fill_page_buf = 0;
	page_idx = 0;
}	
void ack_end_to_flash(void)
{
	memset(send_ack_buf,0xFF,9);
	send_ack_buf[0]=FW_UPDATE_REPORT_ID;
	send_ack_buf[1]=0xFE;
	send_ack_buf[2]=0xF0;
	
	Send_Ack_To_PC(send_ack_buf,9);
}	
void test_process(void)
{
	ack_end_to_flash();
}	




void Send_Ack_To_PC(uint8_t* data , uint16_t size) 
{
	//use uart to ack 
	//HAL_UART_Transmit(&huart6,data,size,1000);
	
	//use usb to ack
	USBD_CUSTOM_HID_SendReport_FS(data,size);
}	

void wait_end_to_flash ()
{
	while(1)
	{
		if(memcmp(aRxBuffer,CMD_END_FLASH,11)== 0)
		{
				
			//flag_start_fill_page_buf = 0;
			//USBD_CUSTOM_HID_SendReport_FS(msg,8) ;
			break;
		}
	}	
}
void  ISP_Process(void)
{
	flag_start_fill_page_buf = 0;
	page_idx = 0;
	page_buf_full_idx= 0;
	
	Enter_ISP() ;
	HAL_Delay(1);
	Cmd_T_Erase();
	HAL_Delay(1);
	MaxPages = Flash.Page;
	//printf("MaxPages=%d\n\r ",MaxPages);
	ack_ready_to_flash();
	
	set_bank0();
	flash_bank0_fw();
	
	set_bank1();
	flash_bank1_fw();
	
	set_bank2();
	flash_bank2_fw();
	
	set_bank3();
	flash_bank3_fw();
	printf("bank3 flash done!!!!!!");
	
	wait_end_to_flash();
	
	HAL_Delay(1);
			
	Cmd_T_BlockProtect(1); 
	//Exit ISP Mode
	T_ISPMode(0);
	
	HAL_Delay(1) ;
	
	ack_end_to_flash();
	
	printf("re-enter scaler app code!!!!!!");
	
}	
