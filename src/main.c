#include	"include/config.h"
#include	"include/STC8G_H_GPIO.h"
#include	"include/STC8G_H_I2C.h"
#include	"include/STC8G_H_Timer.h"
// #include	"include/STC8G_H_UART.h"
#include	"include/STC8G_H_NVIC.h"
#include	"include/STC8G_H_Switch.h"
#include	"include/STC8G_H_Delay.h"

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/
bit B_1ms,fm,am,stereo,set=FALSE,tmp1=FALSE;         //1ms标志

u8  x,y,snr,rssi,last,signal;     //通用数组
u16 msecond;
u32 freq;

/*************	本地函数声明	**************/

//void DisplayScan(void);

/*************  外部函数和变量声明 *****************/


/******************** IO口配置 ********************/
void	GPIO_config(void)
{
	P3_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3);		//P3.3,P3.2 设置为准双向口
	P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);		//P3.0,P3.1 设置为准双向口
	P1_MODE_IN_HIZ(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 );	//P1.0,P1.1 设置为输入悬空口
	P1_MODE_IO_PU(GPIO_Pin_7);
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
	I2C_InitStructure.I2C_Speed	 = 58;
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;						//主机模式下,是否使能自动发送功能,   ENABLE, DISABLE
	//I2C_InitStructure.I2C_SL_MA     = ENABLE;						//使能从机地址比较功能,   ENABLE, DISABLE
	//I2C_InitStructure.I2C_SL_ADR    = 0x2d;							//从机设备地址,  0~127  (0x2d<<1 = 0x5a)
	I2C_Init(&I2C_InitStructure);
	NVIC_I2C_Init(I2C_Mode_Master,I2C_ESTAI|I2C_ERXI|I2C_ETXI|I2C_ESTOI,Priority_0);	//主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, I2C_ESTAI/I2C_ERXI/I2C_ETXI/I2C_ESTOI/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	I2C_SW(I2C_P33_P32);					//I2C_P14_P15,I2C_P24_P25,I2C_P33_P32
	//I2C_SW(I2C_P24_P25);					//I2C_P14_P15,I2C_P24_P25,I2C_P33_P32
}

/****************  UART初始化函数 *****************/
// void	UART_config(void)
// {
// 	COMx_InitDefine		COMx_InitStructure;					//结构定义

// 	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
// 	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
// 	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
// 	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
// 	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
// 	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4
// 	NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

// 	UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
// }

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
void SetFM()
{
	WriteRegister(0x04, 0x80);//Clock initialization completed.
	WriteRegister(0x51, 0x02);//Dial controlled channel increase/decrease
	WriteRegister(0x4e,0x13);//DEPOP_TC=500ms AUDV_DCLVL=1.15V
	WriteRegister(0x2f,0x22);//ANT_CALI_SWITCH_BAND=1 SMUTE_FILTER_EN = 1
	// WriteRegister(0x51, 0x01);//Key controlled channel increase/decrease
	WriteRegister(0x71, 0x80);//Channel ADC Disable, Channel ADC Start set to None.
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
	WriteRegister(0x2b, 0x20);//Stereo, 75us De-emphasis, Blend enable
  }

/**************** 配置MW *****************/
  void SetAM()
  {
	WriteRegister(0x04, 0x80);//Clock initialization completed.
	WriteRegister(0xa0, 0x03);//CH_GUARD=3
	WriteRegister(0x51, 0x02);//Dial controlled channel increase/decrease
	WriteRegister(0x71, 0x80);//Channel ADC Enable, Channel ADC Start set to None.
	WriteRegister(0x9b, 0x7a);//通道间隔(1620-522)/9=122
	WriteRegister(0x9a, 0x00);//通道间隔(1620-522)/9=122
	WriteRegister(0x88, 0xc8);//Change band, AM mode, High edge frequency of FM band set to 108MHz
	WriteRegister(0x99, 0x0a);//MW最低频率522KHz
	WriteRegister(0x98, 0x02);//MW最低频率522KHz
	WriteRegister(0x8c, 0x06);//MW最高频率1620KHz
	WriteRegister(0x8d, 0x54);//MW最高频率1620KHz
	WriteRegister(0x18, 0x59);//FM Band Space 100KHz, MW Band Space 9KHz
	WriteRegister(0x71, 0x04);//Enable Channel ADC, Channel ADC Start
	WriteRegister(0x22, 0xd5);//Enable Tune Interrupt, INT is edge triggered, FM_SMUTE_START_SNR=default
	WriteRegister(0x1F, 0xd3);//INT interrupt is active high, FM_SMUTE_START_RSSI=default, FM_SMUTE_SLOPE_RSSI=default
  }
  /**************** 配置MW *****************/
  void SetSW()
  {
	WriteRegister(0x04, 0x80);//Clock initialization completed.
	WriteRegister(0xa0, 0x03);//CH_GUARD=3
	WriteRegister(0x51, 0x02);//Dial controlled channel increase/decrease
	WriteRegister(0x71, 0x80);//Channel ADC Enable, Channel ADC Start set to None.
	WriteRegister(0x9b, 0x20);//通道间隔(8000-4000)/5=800
	WriteRegister(0x9a, 0x03);//通道间隔(8000-4000)/5=800
	WriteRegister(0x99, 0xa0);//MW最低频率4000KHz
	WriteRegister(0x98, 0x0f);//MW最低频率4000KHz
	WriteRegister(0x8c, 0x1f);//MW最高频率8000KHz
	WriteRegister(0x8d, 0x40);//MW最高频率8000KHz
	WriteRegister(0x19, 0x31);//SW Band Space 5KHz, keep others default.
	WriteRegister(0x62, 0x40);//AM Channel Filter Bandwidth Selection = 1.2kHz
	WriteRegister(0x16, 0x9a);//SW Enable
	WriteRegister(0x71, 0x04);//Enable Channel ADC, Channel ADC Start
	WriteRegister(0x88, 0xc8);//Change band, AM mode, High edge frequency of FM band set to 108MHz
	WriteRegister(0x22, 0xd5);//Enable Tune Interrupt, INT is edge triggered, FM_SMUTE_START_SNR=default
	WriteRegister(0x1F, 0xd3);//INT interrupt is active high, FM_SMUTE_START_RSSI=default, FM_SMUTE_SLOPE_RSSI=default
  }
/**************** 读取当前频率 *****************/
  void GetFrequency(u32 *frequency) 
  {
	u8 high_byte, low_byte;
	ReadRegister(0xe4, &high_byte);
	ReadRegister(0xe5, &low_byte);
	*frequency = (high_byte << 8) | low_byte;

  }
/**************** 读取FM RSSI *****************/
  void GetFMrssi(u8 *rssi)
  {
	u8 fm_rssi;
	ReadRegister(0xe6, &fm_rssi);
	*rssi = fm_rssi -3;
  }
/**************** 读取FM SNR *****************/  
  void GetFMsnr(u8 *snr)
  {
	ReadRegister(0xe2, snr);
  }
/**************** 读取MW RSSI *****************/ 
  void GetMWrssi(u8 *rssi)
  {
	u8 mw_rssi;
	ReadRegister(0xea, &mw_rssi);
	*rssi = mw_rssi -3;
  }
/**************** 读取MW SNR *****************/ 
  void GetMWsnr(u8 *snr)
  {
	ReadRegister(0xec, snr);
  }
/**************** 检查是否是立体声 *****************/ 
  BOOL CheckStereo()
  {
	u8 i=0;
	ReadRegister(0xde, &i);
	i = i & 0x01;
	return i;
  }
  /**************** 更新屏幕 *****************/
  void UpdateScreen()
  {
	u8 xx=32;
	GetFrequency(&freq);
	if(fm)
	{
		GetFMrssi(&rssi);
		GetFMsnr(&snr);
		signal = (rssi * snr)/20;
		OLED_ShowSignal(0,0,signal,&last);
		stereo = CheckStereo();
		OLED_ShowNum(xx,3, freq/20,3,16);
		OLED_ShowChar(xx+24,3,'.',16);
		OLED_ShowNum(xx+32,3,(freq/2)%10,1,16);
		OLED_ShowString(xx+44,4,"MHz",8);
		OLED_ShowString(0,7,"FM",8);
		if(stereo)
		{
			OLED_ShowString(80,7,"STEREO",8);
		}
		else
		{
			OLED_ShowString(80,7,"  MONO",8);
		}
	}
	else
	{
		GetMWrssi(&rssi);
		GetMWsnr(&snr);
		signal = (rssi * snr)/50;
		OLED_ShowSignal(0,0,signal,&last);
		OLED_ShowNum(xx,3, freq,5,16);
		OLED_ShowString(xx+44,4,"KHz",8);
		OLED_ShowString(0,7,"MW",8);
	}
	
  }

/******************** Main() **************************/
void main(void)
{
	EAXSFR();		/* 扩展寄存器访问使能 */
	GPIO_config();
	Timer_config();
	I2C_config();
	// UART_config();
	OLED_Clear();
	OLED_Init();

	EA = 1;
	fm = FALSE;am=FALSE;
	while (1)
	{
		if(B_1ms)
		{
			B_1ms = 0;
            if(++msecond >= 1000 && set == FALSE)   //1秒到
            {
				msecond = 0;
				GetFrequency(&freq);
				if(freq == 1720)
				{
					SetFM();
					fm = TRUE;
					am = FALSE;
				}
				else
				{
					set = TRUE;
				}
            }
		}

		if(P10 == 1)
		{
			UpdateScreen();
		}
		if(P11 == 0 && !fm)
		{
			SetFM();OLED_Clear();
			fm = TRUE;
			am = FALSE;
			UpdateScreen();
		}
		if(P11 == 1 && !am)
		{
			SetSW();OLED_Clear();
			fm = FALSE;
			am = TRUE;
			UpdateScreen();
		}
		// if(P16 == 1 && tmp1 == FALSE)
		// {
		// 	tmp1 = TRUE;
		// 	WriteRegister(0x0e, 0x00);
		// 	WriteRegister(0x51, 0x01);			WriteRegister(0x88, 0x88);
		// 	WriteRegister(0xe4, 0x08);
		// 	WriteRegister(0xe5, 0x6a);
		// 	delay_ms(20);P17=0;delay_ms(20);P17=1;

		// }
		// if(P16 == 0 && tmp1 == TRUE)
		// {
		// 	tmp1 = FALSE;
		// 	WriteRegister(0x51, 0x01);			WriteRegister(0x88, 0x88);
		// 	WriteRegister(0xe4, 0x06);
		// 	WriteRegister(0xe5, 0xde);
		// 	delay_ms(20);P17=0;delay_ms(20);P17=1;
		// }
	}
	
}
