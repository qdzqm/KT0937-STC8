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
/*这个函数被修改过了，增加了OLED相关的函数*/
#include	"include/STC8G_H_I2C.h"
#include 	"include/OLEDFont.h"
#include 	"string.h"

u8 xdata I2C_Buffer[I2C_BUF_LENTH];

#define SLAW    0x6A
#define SLAR    0x6B

//========================================================================
// 函数: void I2C_Init(I2C_InitTypeDef *I2Cx)
// 描述: I2C初始化程序.
// 参数: I2Cx: 结构参数,请参考I2C.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void I2C_Init(I2C_InitTypeDef *I2Cx)
{
	if(I2Cx->I2C_Mode == I2C_Mode_Master)
	{
		I2C_Master();			//设为主机	
		I2CMSST = 0x00;		//清除I2C主机状态寄存器
		I2C_SetSpeed(I2Cx->I2C_Speed);
		if(I2Cx->I2C_MS_WDTA == ENABLE)		I2C_WDTA_EN();	//使能自动发送
		else									I2C_WDTA_DIS();	//禁止自动发送
	}
	else
	{
		I2C_Slave();	//设为从机
		I2CSLST = 0x00;		//清除I2C从机状态寄存器
		I2C_Address(I2Cx->I2C_SL_ADR);
		if(I2Cx->I2C_SL_MA == ENABLE)		I2C_MATCH_EN();	//从机地址比较功能，只接受相匹配地址
		else									I2C_MATCH_DIS();	//禁止从机地址比较功能，接受所有设备地址
	}
	
	I2C_Function(I2Cx->I2C_Enable);
}

//========================================================================
// 函数: u8	Get_MSBusy_Status (void)
// 描述: 获取主机忙碌状态.
// 参数: none.
// 返回: 主机忙碌状态.
// 版本: V1.0, 2012-11-22
//========================================================================
u8 Get_MSBusy_Status(void)
{
	return (I2CMSST & 0x80);
}

//========================================================================
// 函数: void	Wait (void)
// 描述: 等待主机模式I2C控制器执行完成I2CMSCR.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void Wait()
{
	while (!(I2CMSST & 0x40));
	I2CMSST &= ~0x40;
}

//========================================================================
// 函数: void Start (void)
// 描述: I2C总线起始函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void Start()
{
	I2CMSCR = 0x01;                         //发送START命令
	Wait();
}

//========================================================================
// 函数: void SendData (u8 dat)
// 描述: I2C发送一个字节数据函数.
// 参数: 发送的数据.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void SendData(u8 dat)
{
	I2CTXD = dat;                           //写数据到数据缓冲区
	I2CMSCR = 0x02;                         //发送SEND命令
	Wait();
}

//========================================================================
// 函数: void RecvACK (void)
// 描述: I2C获取ACK函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void RecvACK()
{
	I2CMSCR = 0x03;                         //发送读ACK命令
	Wait();
}

//========================================================================
// 函数: char RecvData (void)
// 描述: I2C读取一个字节数据函数.
// 参数: none.
// 返回: 读取数据.
// 版本: V1.0, 2020-09-15
//========================================================================
char RecvData()
{
	I2CMSCR = 0x04;                         //发送RECV命令
	Wait();
	return I2CRXD;
}

//========================================================================
// 函数: void SendACK (void)
// 描述: I2C发送ACK函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void SendACK()
{
	I2CMSST = 0x00;                         //设置ACK信号
	I2CMSCR = 0x05;                         //发送ACK命令
	Wait();
}

//========================================================================
// 函数: void SendNAK (void)
// 描述: I2C发送NAK函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void SendNAK()
{
	I2CMSST = 0x01;                         //设置NAK信号
	I2CMSCR = 0x05;                         //发送ACK命令
	Wait();
}

//========================================================================
// 函数: void Stop (void)
// 描述: I2C总线停止函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void Stop()
{
	I2CMSCR = 0x06;                         //发送STOP命令
	Wait();
}

//========================================================================
// 函数: void SendCmdData (u8 cmd, u8 dat)
// 描述: I2C发送一个字节数据函数.
// 参数: 命令/数据.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void SendCmdData(u8 cmd, u8 dat)
{
	I2CTXD = dat;                           //写数据到数据缓冲区
	I2CMSCR = cmd;                          //设置命令
	Wait();
}

//========================================================================
// 函数: void I2C_WriteNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)
// 描述: I2C写入数据函数.
// 参数: dev_addr: 设备地址, mem_addr: 存储地址, *p写入数据存储位置, number写入数据个数.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void I2C_Writebyte(u8 dev_addr, u8 mem_addr, u8 *p)  /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //发送起始命令
	SendData(dev_addr);                     //发送设备地址+写命令
	RecvACK();
	SendData(mem_addr);                     //发送存储地址
	RecvACK();
	SendData(*p);
	RecvACK();
	Stop();                                 //发送停止命令
}

//========================================================================
// 函数: void I2C_ReadNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)
// 描述: I2C读取数据函数.
// 参数: dev_addr: 设备地址, mem_addr: 存储地址, *p读取数据存储位置, number读取数据个数.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void I2C_Readbyte(u8 dev_addr, u8 mem_addr, u8 *p)   /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //发送起始命令
	SendData(dev_addr);                     //发送设备地址+写命令
	RecvACK();
	SendData(mem_addr);                     //发送存储地址
	RecvACK();
	Start();
	SendData(dev_addr|1);                   //发送设备地址+读命令
	RecvACK();
	*p = RecvData();
	SendNAK();                              //send no ACK	
	Stop();                                 //发送停止命令
}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
void Oled_Write_Cmd(u8 Comd)  /* 向OLED 写入命令 */
{
	Start();	SendData(0x78); //驱动地址 0x78
	RecvACK();	SendData(0x00); // 发送的只为字节D/C=000 0000为命令
	RecvACK();	SendData(Comd);
	RecvACK();
	Stop();
}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//======================================================================== 
void Oled_Write_Data(u8 Data)   /* 向OLED 写入数据 */
{
	Start();  	SendData(0x78); //驱动地址 0x78
	RecvACK();	SendData(0x40); // 发送的只为字节D/C=100 0000为数据
	RecvACK();	SendData(Data);
	RecvACK();
	Stop();
}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
/* 坐标设置函数 */ 
void OLED_Set_Points(u8 x, u8 y) 
{ 	
	Oled_Write_Cmd(0xb0+y);
	Oled_Write_Cmd(((x&0xf0)>>4)|0x10);
	Oled_Write_Cmd((x&0x0f)); 
}   	  
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//======================================================================== 
/* 清屏函数 */  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		Oled_Write_Cmd(0xb0+i);    //设置页地址（0~3）
		Oled_Write_Cmd(0x00);      //设置显示位置—列低地址
		Oled_Write_Cmd(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)    Oled_Write_Data(0);
	} 
}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
void OLED_ShowPx(u8 x,u8 y)
{      	
	u8 c=0,i=0;	
//	c = car-' ';   //偏移后的值			
	if(x>128-1)  {x=0;y=y+2;}

		OLED_Set_Points(x,y);
		Oled_Write_Data(0x55);


}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
   /* 显示字符          列     行  字符内容       哪个数组 */
void OLED_ShowChar(u8 x,u8 y,u8 car,u8 Char_Size)
{   
	u8 c=0,i=0;	
	c = car-' ';   //偏移后的值			
	if(x>128-1)  {x=0;y=y+2;}
	if(Char_Size ==16)
		{
			OLED_Set_Points(x,y);	
			for(i=0;i<8;i++)
			  { Oled_Write_Data(F8X16[c*16+i]); }
			OLED_Set_Points(x,y+1);
			for(i=0;i<8;i++)
			  { Oled_Write_Data(F8X16[c*16+i+8]); }
		}
	else 
		{	
			OLED_Set_Points(x,y);
			for(i=0;i<6;i++)
				{ Oled_Write_Data(F6x8[c][i]);	}
		}
}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
  /* 字符串形式显示字符   列      行  指针-字符串    哪个数组 */
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	u8 j;
	for(j=0;j<strlen(chr);j++)
	 {		
		 OLED_ShowChar(x,y,chr[j],Char_Size);
		  x+=6;
		if(x>120) {x=0;y+=2;}
	 }
}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//======================================================================== 
u16 oled_pow(u8 m,u8 n)
{
	u16 result=1;	 
	while(n--)result*=m;    
	return result;
}			
 //========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
  /* 显示变量数字      列    行   变量内容  字符长度   字符大小16或8  */
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 len,u8 num_size)
{         	
	u8 t,temp,enshow=0;
  for(t=0;t<len;t++)
	 {
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		 {
			if(temp==0)
			{
				if(num_size == 8) OLED_ShowChar(x+(num_size/2+2)*t,y,' ',num_size); 	
				else if(num_size == 16) OLED_ShowChar(x+(num_size/2)*t,y,' ',num_size);  
				continue;
			}else enshow=1; 
 		}
	 if(num_size == 8) 	OLED_ShowChar(x+(num_size/2+2)*t,y,temp+'0',num_size); 	 		 
	 else if(num_size == 16) 	OLED_ShowChar(x+(num_size/2)*t,y,temp+'0',num_size);  
	}
} 
 //========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
  /* 显示变量数字      列    行   变量内容  字符长度   字符大小16或8  */
void OLED_ShowSignal(u8 x, u8 y, u8 signal, u8 *l)
{
    u8 i=0;
	OLED_Set_Points(x, y); // 设置起点
	for(i=0;i<*l;i++) 
	{
		Oled_Write_Data(0x00); 
	}
	OLED_Set_Points(x, y); // 设置起点
    for(i=0;i<signal;i++) 
	{
		Oled_Write_Data(0x18); 
	}
	*l = signal;
}
//========================================================================
// 函数: 
// 描述: 
// 参数: 
// 返回: 
// 版本: 
//========================================================================
/* OLED12864 屏幕初始化 */

void OLED_Init(void)
{ 
	Oled_Write_Cmd(0xAE); //关闭显示display off
	Oled_Write_Cmd(0x20);	//Set Memory Addressing Mode	
	Oled_Write_Cmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET,OLED_CMD);11,Invalid
	Oled_Write_Cmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	Oled_Write_Cmd(0xc8);	//Set COM Output Scan Direction
	Oled_Write_Cmd(0x02); //-设置低位列地址set low column address 注意：0.96屏幕从0x00开始写数据，1.3寸屏幕从0x02开始写数据
	Oled_Write_Cmd(0x10); //-设置高位列地址set high column address
	Oled_Write_Cmd(0x40); //--set start line address
	Oled_Write_Cmd(0x81); //--set contrast control register
	Oled_Write_Cmd(0xff); //亮度调节 0x00~0xff
	Oled_Write_Cmd(0xa1); //--set segment re-map 0 to 127
	Oled_Write_Cmd(0xa6); //--set normal display
	Oled_Write_Cmd(0xa8); //--set multiplex ratio(1 to 64,OLED_CMD)
	Oled_Write_Cmd(0x3F); //
	Oled_Write_Cmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Oled_Write_Cmd(0xd3); //-设置显示偏移set display offset
	Oled_Write_Cmd(0x00); //-没有偏移not offset
	Oled_Write_Cmd(0xd5); //-设置时钟分频因子,震荡频率set display clock divide ratio/oscillator frequency
	Oled_Write_Cmd(0xf0); //--set divide ratio
	Oled_Write_Cmd(0xd9); //--set pre-charge period
	Oled_Write_Cmd(0x22); //
	Oled_Write_Cmd(0xda); //--设置COM硬件引脚配置set com pins hardware configuration
	Oled_Write_Cmd(0x12);//[5:4]配置
	Oled_Write_Cmd(0xdb); //--set vcomh
	Oled_Write_Cmd(0x20); //0x20,0.77xVcc
	Oled_Write_Cmd(0x8d); //-电荷泵设置set DC-DC enable
	Oled_Write_Cmd(0x14); //bit2，开启/关闭
	Oled_Write_Cmd(0xaf); //-开启显示 turn on oled panel
}

// void OLED_Init(void)
// { 
// 	Oled_Write_Cmd(0xAE);//关闭显示
// 	Oled_Write_Cmd(0x00);//set memory addressing Mode
// 	Oled_Write_Cmd(0X10);
// 	Oled_Write_Cmd(0X40);
// 	Oled_Write_Cmd(0X81);
// 	Oled_Write_Cmd(0XCF);//分频因子
// 	Oled_Write_Cmd(0xA1);//设置驱动路数
// 	Oled_Write_Cmd(0xC8);//默认0X3f(1/64) 0x1f(1/32)
// 	Oled_Write_Cmd(0xA6); //设置显示偏移
// 	Oled_Write_Cmd(0xA8);//默认值00
		
// 	Oled_Write_Cmd(0x3F);//设置开始行 【5:0】，行数
	
// 	Oled_Write_Cmd(0xD3);//电荷泵设置
// 	Oled_Write_Cmd(0x00);//bit2,开启/关闭
	
// 	Oled_Write_Cmd(0xD5);//设置内存地址模式
// 	Oled_Write_Cmd(0x80);//[[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
// 	Oled_Write_Cmd(0xD9);//段重定义设置,bit0:0,0->0;1,0->127;
// 	Oled_Write_Cmd(0xF1);//设置COM扫描方向
	
// 	Oled_Write_Cmd(0xDA);//设置COM硬件引脚配置
// 	Oled_Write_Cmd(0x12);//0.91英寸128*32分辨率
	
// 	Oled_Write_Cmd(0xDB);//对比度设置
// 	Oled_Write_Cmd(0x40);//1~255(亮度设置,越大越亮)
	
// 	Oled_Write_Cmd(0x20);//设置预充电周期
// 	Oled_Write_Cmd(0x02);//[3:0],PHASE 1;[7:4],PHASE 2;
// 	Oled_Write_Cmd(0x8D);//设置VCOMH 电压倍率
// 	Oled_Write_Cmd(0x14);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
 
// 	Oled_Write_Cmd(0xA4);//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
// 	Oled_Write_Cmd(0xA6);//设置显示方式;bit0:1,反相显示;0,正常显示
	
// 	Oled_Write_Cmd(0x2E);//停用滚动条 
// 	Oled_Write_Cmd(0xAF);//开启显示

// }
