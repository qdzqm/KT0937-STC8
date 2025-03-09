/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* --- BBS: www.STCAIMCU.com  -----------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ����STC�����ϼ�����            */
/*---------------------------------------------------------------------*/
/*����������޸Ĺ��ˣ�������OLED��صĺ���*/
#include	"include/STC8G_H_I2C.h"
#include 	"include/OLEDFont.h"
#include 	"string.h"

u8 xdata I2C_Buffer[I2C_BUF_LENTH];

#define SLAW    0x6A
#define SLAR    0x6B

//========================================================================
// ����: void I2C_Init(I2C_InitTypeDef *I2Cx)
// ����: I2C��ʼ������.
// ����: I2Cx: �ṹ����,��ο�I2C.h��Ķ���.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void I2C_Init(I2C_InitTypeDef *I2Cx)
{
	if(I2Cx->I2C_Mode == I2C_Mode_Master)
	{
		I2C_Master();			//��Ϊ����	
		I2CMSST = 0x00;		//���I2C����״̬�Ĵ���
		I2C_SetSpeed(I2Cx->I2C_Speed);
		if(I2Cx->I2C_MS_WDTA == ENABLE)		I2C_WDTA_EN();	//ʹ���Զ�����
		else									I2C_WDTA_DIS();	//��ֹ�Զ�����
	}
	else
	{
		I2C_Slave();	//��Ϊ�ӻ�
		I2CSLST = 0x00;		//���I2C�ӻ�״̬�Ĵ���
		I2C_Address(I2Cx->I2C_SL_ADR);
		if(I2Cx->I2C_SL_MA == ENABLE)		I2C_MATCH_EN();	//�ӻ���ַ�ȽϹ��ܣ�ֻ������ƥ���ַ
		else									I2C_MATCH_DIS();	//��ֹ�ӻ���ַ�ȽϹ��ܣ����������豸��ַ
	}
	
	I2C_Function(I2Cx->I2C_Enable);
}

//========================================================================
// ����: u8	Get_MSBusy_Status (void)
// ����: ��ȡ����æµ״̬.
// ����: none.
// ����: ����æµ״̬.
// �汾: V1.0, 2012-11-22
//========================================================================
u8 Get_MSBusy_Status(void)
{
	return (I2CMSST & 0x80);
}

//========================================================================
// ����: void	Wait (void)
// ����: �ȴ�����ģʽI2C������ִ�����I2CMSCR.
// ����: none.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void Wait()
{
	while (!(I2CMSST & 0x40));
	I2CMSST &= ~0x40;
}

//========================================================================
// ����: void Start (void)
// ����: I2C������ʼ����.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void Start()
{
	I2CMSCR = 0x01;                         //����START����
	Wait();
}

//========================================================================
// ����: void SendData (u8 dat)
// ����: I2C����һ���ֽ����ݺ���.
// ����: ���͵�����.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void SendData(u8 dat)
{
	I2CTXD = dat;                           //д���ݵ����ݻ�����
	I2CMSCR = 0x02;                         //����SEND����
	Wait();
}

//========================================================================
// ����: void RecvACK (void)
// ����: I2C��ȡACK����.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void RecvACK()
{
	I2CMSCR = 0x03;                         //���Ͷ�ACK����
	Wait();
}

//========================================================================
// ����: char RecvData (void)
// ����: I2C��ȡһ���ֽ����ݺ���.
// ����: none.
// ����: ��ȡ����.
// �汾: V1.0, 2020-09-15
//========================================================================
char RecvData()
{
	I2CMSCR = 0x04;                         //����RECV����
	Wait();
	return I2CRXD;
}

//========================================================================
// ����: void SendACK (void)
// ����: I2C����ACK����.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void SendACK()
{
	I2CMSST = 0x00;                         //����ACK�ź�
	I2CMSCR = 0x05;                         //����ACK����
	Wait();
}

//========================================================================
// ����: void SendNAK (void)
// ����: I2C����NAK����.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void SendNAK()
{
	I2CMSST = 0x01;                         //����NAK�ź�
	I2CMSCR = 0x05;                         //����ACK����
	Wait();
}

//========================================================================
// ����: void Stop (void)
// ����: I2C����ֹͣ����.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void Stop()
{
	I2CMSCR = 0x06;                         //����STOP����
	Wait();
}

//========================================================================
// ����: void SendCmdData (u8 cmd, u8 dat)
// ����: I2C����һ���ֽ����ݺ���.
// ����: ����/����.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void SendCmdData(u8 cmd, u8 dat)
{
	I2CTXD = dat;                           //д���ݵ����ݻ�����
	I2CMSCR = cmd;                          //��������
	Wait();
}

//========================================================================
// ����: void I2C_WriteNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)
// ����: I2Cд�����ݺ���.
// ����: dev_addr: �豸��ַ, mem_addr: �洢��ַ, *pд�����ݴ洢λ��, numberд�����ݸ���.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void I2C_Writebyte(u8 dev_addr, u8 mem_addr, u8 *p)  /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //������ʼ����
	SendData(dev_addr);                     //�����豸��ַ+д����
	RecvACK();
	SendData(mem_addr);                     //���ʹ洢��ַ
	RecvACK();
	SendData(*p);
	RecvACK();
	Stop();                                 //����ֹͣ����
}

//========================================================================
// ����: void I2C_ReadNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)
// ����: I2C��ȡ���ݺ���.
// ����: dev_addr: �豸��ַ, mem_addr: �洢��ַ, *p��ȡ���ݴ洢λ��, number��ȡ���ݸ���.
// ����: none.
// �汾: V1.0, 2020-09-15
//========================================================================
void I2C_Readbyte(u8 dev_addr, u8 mem_addr, u8 *p)   /*  DeviceAddress,WordAddress,First Data Address,Byte lenth   */
{
	Start();                                //������ʼ����
	SendData(dev_addr);                     //�����豸��ַ+д����
	RecvACK();
	SendData(mem_addr);                     //���ʹ洢��ַ
	RecvACK();
	Start();
	SendData(dev_addr|1);                   //�����豸��ַ+������
	RecvACK();
	*p = RecvData();
	SendNAK();                              //send no ACK	
	Stop();                                 //����ֹͣ����
}
//========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
void Oled_Write_Cmd(u8 Comd)  /* ��OLED д������ */
{
	Start();	SendData(0x78); //������ַ 0x78
	RecvACK();	SendData(0x00); // ���͵�ֻΪ�ֽ�D/C=000 0000Ϊ����
	RecvACK();	SendData(Comd);
	RecvACK();
	Stop();
}
//========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//======================================================================== 
void Oled_Write_Data(u8 Data)   /* ��OLED д������ */
{
	Start();  	SendData(0x78); //������ַ 0x78
	RecvACK();	SendData(0x40); // ���͵�ֻΪ�ֽ�D/C=100 0000Ϊ����
	RecvACK();	SendData(Data);
	RecvACK();
	Stop();
}
//========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
/* �������ú��� */ 
void OLED_Set_Points(u8 x, u8 y) 
{ 	
	Oled_Write_Cmd(0xb0+y);
	Oled_Write_Cmd(((x&0xf0)>>4)|0x10);
	Oled_Write_Cmd((x&0x0f)); 
}   	  
//========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//======================================================================== 
/* �������� */  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		Oled_Write_Cmd(0xb0+i);    //����ҳ��ַ��0~3��
		Oled_Write_Cmd(0x00);      //������ʾλ�á��е͵�ַ
		Oled_Write_Cmd(0x10);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)    Oled_Write_Data(0);
	} 
}
//========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
void OLED_ShowPx(u8 x,u8 y)
{      	
	u8 c=0,i=0;	
//	c = car-' ';   //ƫ�ƺ��ֵ			
	if(x>128-1)  {x=0;y=y+2;}

		OLED_Set_Points(x,y);
		Oled_Write_Data(0x55);


}
//========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
   /* ��ʾ�ַ�          ��     ��  �ַ�����       �ĸ����� */
void OLED_ShowChar(u8 x,u8 y,u8 car,u8 Char_Size)
{   
	u8 c=0,i=0;	
	c = car-' ';   //ƫ�ƺ��ֵ			
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
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
  /* �ַ�����ʽ��ʾ�ַ�   ��      ��  ָ��-�ַ���    �ĸ����� */
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
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//======================================================================== 
u16 oled_pow(u8 m,u8 n)
{
	u16 result=1;	 
	while(n--)result*=m;    
	return result;
}			
 //========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
  /* ��ʾ��������      ��    ��   ��������  �ַ�����   �ַ���С16��8  */
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
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
  /* ��ʾ��������      ��    ��   ��������  �ַ�����   �ַ���С16��8  */
void OLED_ShowSignal(u8 x, u8 y, u8 signal, u8 *l)
{
    u8 i=0;
	OLED_Set_Points(x, y); // �������
	for(i=0;i<*l;i++) 
	{
		Oled_Write_Data(0x00); 
	}
	OLED_Set_Points(x, y); // �������
    for(i=0;i<signal;i++) 
	{
		Oled_Write_Data(0x18); 
	}
	*l = signal;
}
//========================================================================
// ����: 
// ����: 
// ����: 
// ����: 
// �汾: 
//========================================================================
/* OLED12864 ��Ļ��ʼ�� */

void OLED_Init(void)
{ 
	Oled_Write_Cmd(0xAE); //�ر���ʾdisplay off
	Oled_Write_Cmd(0x20);	//Set Memory Addressing Mode	
	Oled_Write_Cmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET,OLED_CMD);11,Invalid
	Oled_Write_Cmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	Oled_Write_Cmd(0xc8);	//Set COM Output Scan Direction
	Oled_Write_Cmd(0x02); //-���õ�λ�е�ַset low column address ע�⣺0.96��Ļ��0x00��ʼд���ݣ�1.3����Ļ��0x02��ʼд����
	Oled_Write_Cmd(0x10); //-���ø�λ�е�ַset high column address
	Oled_Write_Cmd(0x40); //--set start line address
	Oled_Write_Cmd(0x81); //--set contrast control register
	Oled_Write_Cmd(0xff); //���ȵ��� 0x00~0xff
	Oled_Write_Cmd(0xa1); //--set segment re-map 0 to 127
	Oled_Write_Cmd(0xa6); //--set normal display
	Oled_Write_Cmd(0xa8); //--set multiplex ratio(1 to 64,OLED_CMD)
	Oled_Write_Cmd(0x3F); //
	Oled_Write_Cmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Oled_Write_Cmd(0xd3); //-������ʾƫ��set display offset
	Oled_Write_Cmd(0x00); //-û��ƫ��not offset
	Oled_Write_Cmd(0xd5); //-����ʱ�ӷ�Ƶ����,��Ƶ��set display clock divide ratio/oscillator frequency
	Oled_Write_Cmd(0xf0); //--set divide ratio
	Oled_Write_Cmd(0xd9); //--set pre-charge period
	Oled_Write_Cmd(0x22); //
	Oled_Write_Cmd(0xda); //--����COMӲ����������set com pins hardware configuration
	Oled_Write_Cmd(0x12);//[5:4]����
	Oled_Write_Cmd(0xdb); //--set vcomh
	Oled_Write_Cmd(0x20); //0x20,0.77xVcc
	Oled_Write_Cmd(0x8d); //-��ɱ�����set DC-DC enable
	Oled_Write_Cmd(0x14); //bit2������/�ر�
	Oled_Write_Cmd(0xaf); //-������ʾ turn on oled panel
}

// void OLED_Init(void)
// { 
// 	Oled_Write_Cmd(0xAE);//�ر���ʾ
// 	Oled_Write_Cmd(0x00);//set memory addressing Mode
// 	Oled_Write_Cmd(0X10);
// 	Oled_Write_Cmd(0X40);
// 	Oled_Write_Cmd(0X81);
// 	Oled_Write_Cmd(0XCF);//��Ƶ����
// 	Oled_Write_Cmd(0xA1);//��������·��
// 	Oled_Write_Cmd(0xC8);//Ĭ��0X3f(1/64) 0x1f(1/32)
// 	Oled_Write_Cmd(0xA6); //������ʾƫ��
// 	Oled_Write_Cmd(0xA8);//Ĭ��ֵ00
		
// 	Oled_Write_Cmd(0x3F);//���ÿ�ʼ�� ��5:0��������
	
// 	Oled_Write_Cmd(0xD3);//��ɱ�����
// 	Oled_Write_Cmd(0x00);//bit2,����/�ر�
	
// 	Oled_Write_Cmd(0xD5);//�����ڴ��ַģʽ
// 	Oled_Write_Cmd(0x80);//[[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
// 	Oled_Write_Cmd(0xD9);//���ض�������,bit0:0,0->0;1,0->127;
// 	Oled_Write_Cmd(0xF1);//����COMɨ�跽��
	
// 	Oled_Write_Cmd(0xDA);//����COMӲ����������
// 	Oled_Write_Cmd(0x12);//0.91Ӣ��128*32�ֱ���
	
// 	Oled_Write_Cmd(0xDB);//�Աȶ�����
// 	Oled_Write_Cmd(0x40);//1~255(��������,Խ��Խ��)
	
// 	Oled_Write_Cmd(0x20);//����Ԥ�������
// 	Oled_Write_Cmd(0x02);//[3:0],PHASE 1;[7:4],PHASE 2;
// 	Oled_Write_Cmd(0x8D);//����VCOMH ��ѹ����
// 	Oled_Write_Cmd(0x14);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
 
// 	Oled_Write_Cmd(0xA4);//ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
// 	Oled_Write_Cmd(0xA6);//������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
	
// 	Oled_Write_Cmd(0x2E);//ͣ�ù����� 
// 	Oled_Write_Cmd(0xAF);//������ʾ

// }
