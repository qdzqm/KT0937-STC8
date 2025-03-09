
#ifndef	__STC8G_H_I2C_H
#define	__STC8G_H_I2C_H

#include	"config.h"


#define		I2C_Function(n)	(n==0?(I2CCFG &= ~0x80):(I2CCFG |= 0x80))
#define		I2C_ENABLE()	I2CCFG |= 0x80	
#define		I2C_DISABLE()	I2CCFG &= ~0x80	
#define		I2C_Master()	I2CCFG |=  0x40	
#define		I2C_Slave()		I2CCFG &= ~0x40	
#define		I2C_SetSpeed(n)	I2CCFG = (I2CCFG & ~0x3f) | (n & 0x3f)	

#define		I2C_WDTA_EN()		I2CMSAUX |= 0x01	
#define		I2C_WDTA_DIS()	I2CMSAUX &= ~0x01		

#define		I2C_ESTAI_EN(n)		I2CSLCR = (I2CSLCR & ~0x40) | (n << 6)	
#define		I2C_ERXI_EN(n)		I2CSLCR = (I2CSLCR & ~0x20) | (n << 5)		
#define		I2C_ETXI_EN(n)		I2CSLCR = (I2CSLCR & ~0x10) | (n << 4)		
#define		I2C_ESTOI_EN(n)		I2CSLCR = (I2CSLCR & ~0x08) | (n << 3)		
#define		I2C_SLRET()				I2CSLCR |= 0x01		

#define		I2C_Address(n)	I2CSLADR = (I2CSLADR & 0x01) | (n << 1)	
#define		I2C_MATCH_EN()	I2CSLADR &= ~0x01	
#define		I2C_MATCH_DIS()	I2CSLADR |= 0x01	



#define DEV_ADDR    0xA0   

#define	I2C_BUF_LENTH	8

#define		I2C_ESTAI					0x40	
#define		I2C_ERXI					0x20	
#define		I2C_ETXI					0x10	
#define		I2C_ESTOI					0x08	

typedef struct
{
	u8	I2C_Speed;		
	u8	I2C_Enable;	
	u8	I2C_Mode;		
	u8	I2C_MS_WDTA;	

	u8	I2C_SL_ADR;				
	u8	I2C_SL_MA;				
} I2C_InitTypeDef;

typedef struct
{
	u8	isma;				
	u8	isda;				
	u8	addr;				
} I2C_IsrTypeDef;


extern u8 xdata I2C_Buffer[I2C_BUF_LENTH];
extern bit DisplayFlag;

void I2C_Init(I2C_InitTypeDef *I2Cx);
void I2C_Writebyte(u8 dev_addr, u8 mem_addr, u8 *p);
void I2C_Readbyte(u8 dev_addr, u8 mem_addr, u8 *p);
u8 Get_MSBusy_Status(void);
void SendCmdData(u8 cmd, u8 dat);
void Oled_Write_Cmd(u8 Comd);  
void Oled_Write_Data(u8 Data);  
void OLED_Set_Points(u8 x, u8 y); 
void OLED_Clear(void);  
void OLED_ShowPx(u8 x,u8 y);
void OLED_ShowChar(u8 x,u8 y,u8 car,u8 Char_Size);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size);
u16 oled_pow(u8 m,u8 n);
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 len,u8  num_size);
void OLED_ShowSignal(u8 x, u8 y, u8 signal, u8 *l);
void OLED_Init(void);

#endif

