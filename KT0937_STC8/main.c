/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序            */
/*---------------------------------------------------------------------*/

#include	"config.h"
#include	"STC8G_H_GPIO.h"
#include	"STC8G_H_I2C.h"
#include	"STC8G_H_Soft_I2C.h"
#include	"STC8G_H_Timer.h"
#include	"STC8G_H_UART.h"
#include	"STC8G_H_NVIC.h"
#include	"STC8G_H_Switch.h"

/*************	功能说明	**************

本例程基于STC8H8K64U为主控芯片的开天斧核心板进行编写测试，STC8G、STC8H系列芯片可通用参考.

内部集成I2C串行总线控制器做从机模式，SCL->P2.5, SDA->P2.4;
IO口模拟I2C做主机模式，SCL->P0.0, SDA->P0.1;
通过外部飞线连接 P0.0->P2.5, P0.1->P2.4，实现I2C自发自收功能。

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下,用户修改MCU主时钟频率时,自动定时于1ms.
计数器每秒钟加1, 计数范围为0~199.

上电后主机每秒钟发送一次计数数据，串口打印发送内容；从机接收到数据后通过串口打印接收结果。

需要在"STC8G_H_UART.h"里设置： #define	PRINTF_SELECT  UART1

下载时, 选择时钟 24MHz (可以在配置文件"config.h"中修改).

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/
bit B_1ms,fm;          //1ms标志

u8  temp[4];     //通用数组
u16 msecond;
u8  second;
u32 freq;

/*************	本地函数声明	**************/

//void DisplayScan(void);

/*************  外部函数和变量声明 *****************/


/******************** IO口配置 ********************/
void	GPIO_config(void)
{
	P0_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);		//P0.0,P0.1 设置为准双向口
	P3_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3);		//P3.3,P3.2 设置为准双向口
	P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);		//P3.0,P3.1 设置为准双向口
	P1_MODE_IN_HIZ(GPIO_Pin_0);
}

/************************ 定时器配置 ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;						//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;         //指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;              //是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000UL);   //初值,
	TIM_InitStructure.TIM_Run       = ENABLE;               //是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);               //初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
	NVIC_Timer0_Init(ENABLE,Priority_0);    //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/****************  I2C初始化函数 *****************/
void	I2C_config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;

	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;		//主从选择   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;						//I2C功能使能,   ENABLE, DISABLE
	I2C_InitStructure.I2C_Speed	 = 32;
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;						//主机模式下,是否使能自动发送功能,   ENABLE, DISABLE
	//I2C_InitStructure.I2C_SL_MA     = ENABLE;						//使能从机地址比较功能,   ENABLE, DISABLE
	//I2C_InitStructure.I2C_SL_ADR    = 0x2d;							//从机设备地址,  0~127  (0x2d<<1 = 0x5a)
	I2C_Init(&I2C_InitStructure);
	//NVIC_I2C_Init(I2C_Mode_Master,I2C_ESTAI|I2C_ERXI|I2C_ETXI|I2C_ESTOI,Priority_0);	//主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, I2C_ESTAI/I2C_ERXI/I2C_ETXI/I2C_ESTOI/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	//I2C_SW(I2C_P33_P32);					//I2C_P14_P15,I2C_P24_P25,I2C_P33_P32
	I2C_SW(I2C_P14_P15);					//I2C_P14_P15,I2C_P24_P25,I2C_P33_P32
}

/****************  UART初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

/**************** 写入KT0937寄存器 *****************/
void WriteRegister(u8 reg, u8 value) 
{
	I2C_Writebyte(0x6a, reg, &value);
}

/**************** 读取KT0937寄存器 *****************/
void ReadRegister(u8 reg, u8 *value)
{
	I2C_Readbyte(0x6a, reg, value);
}

/**************** 配置FM *****************/
void SetFM(){
	WriteRegister(0x04, 0x80);//Clock initialization completed.
	WriteRegister(0x51, 0x02);//Dial controlled channel increase/decrease
	WriteRegister(0x9b, 0xcd);//设置87.5MHz~108MHz之间分为205个通道，间隔0.1MHz
	WriteRegister(0x9a, 0x00);//设置87.5MHz~108MHz之间分为205个通道，间隔0.1MHz
	WriteRegister(0x88, 0x88);//可以改变band，FM模式，最高频率是108MHz
	WriteRegister(0x89, 0x70);//最高频率是108MHz
	WriteRegister(0x99, 0xd6);//最低频率是87.5MHz
	WriteRegister(0x98, 0x06);//最低频率是87.5MHz
	WriteRegister(0x18, 0x59);//FM Band Space 100KHz, MW Band Space 9KHz
	WriteRegister(0x71, 0x04);//Enable Channel ADC, Channel ADC Start
	WriteRegister(0x22, 0xd5);//Enable Tune Interrupt, INT is edge triggered, FM_SMUTE_START_SNR=default
	WriteRegister(0x1F, 0xd3);//INT interrupt is active high, FM_SMUTE_START_RSSI=default, FM_SMUTE_SLOPE_RSSI=default
	WriteRegister(0x2b, 0x20);//Force mono, 75us De-emphasis, Blend enable
  }

  /**************** 读取当前频率 *****************/
  void GetFrequency(u32 *frequency) {
	u8 high_byte, low_byte;
	ReadRegister(0xe4, &high_byte);
	ReadRegister(0xe5, &low_byte);
	*frequency = (high_byte << 8) | low_byte;

  }
/******************** task A **************************/
void main(void)
{
	EAXSFR();		/* 扩展寄存器访问使能 */
	GPIO_config();
	Timer_config();
	I2C_config();
	UART_config();
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(0, 0, "FM Radio", 16);
	EA = 1;
	fm = FALSE;
    SetFM();
	while (1)
	{
		if(B_1ms)
		{
			B_1ms = 0;
          
            if(++msecond >= 1000)   //1秒到
            {
				if(!fm)
				{
					SetFM();
					GetFrequency(&freq);
					if(freq == 1720)
					{
						SetFM();
						printf("Trying to set FM\r\n");
					}
					else
					{
						fm = TRUE;
						printf("Set FM\r\n");
					}
				}
            }
		}

		if(P10 == 1)
		{
			GetFrequency(&freq);
			printf("Frequency: %5.1f\r\n", (float)freq/20);

		}
	}
	
}
