
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/


#include	"config.h"
#include	"USART1.h"
#include	"delay.h"
#define dht11_io P33   //P2.0����Ϊ���ݶ�ȡ�ӿ�


/*************	����˵��	**************

˫����ȫ˫���жϷ�ʽ�շ�ͨѶ����

ͨ��PC��MCU��������, MCU�յ���ͨ�����ڰ��յ�������ԭ������.

******************************************/

/*************	���س�������	**************/


/*************	���ر�������	**************/
u16 U8FLAG;
u8  U8temp;
u8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
u8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
u8  U8comdata;
u8 datas=0;

/*************	���غ�������	**************/
void  read_byte(); //��ȡһ�ֽڵ�����,����ֵ��U8comdata
u8  read_temp();//��ȡ�¶ȣ������¶�ֵ
u8  read_humi();//��ȡʪ�ȣ�����ʪ��ֵ


void  Delay30us(); //22.1184M����ȷ��ʱ30΢��
void  Delay40us(); //22.1184M����ȷ��ʱ40΢��
void  Delay18ms(); //22.1184M����ȷ��ʱ30����
void Delay1000ms();//22.1184M����ȷ��ʱ1000����
void serial_send_byte(u8 dat);
void serial_send_string(u8 *dat);

/*************  �ⲿ�����ͱ������� *****************/


/*************  ����1��ʼ������ *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//�ṹ����
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//ģʽ,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//ʹ�ò�����,   BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//�ж�����,   ENABLE��DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//�ж����ȼ�, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//�л��˿�,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(����ʹ���ڲ�ʱ��)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//��ʼ������1 USART1,USART2

	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1����һ���ַ���
}


/**********************************************/
void main(void)
{
	Delay1000ms();
	//u8 i;

	UART_config();
	EA = 1;

	dht11_io = 1;
	//DHT11_delay_ms(1500);//DHT11�ϵ��Ҫ�ȴ�1S��Խ�����ȶ�״̬�ڴ��ڼ䲻�ܷ����κ�ָ��
	while (1)
	{
		datas=read_temp();
		//serial_send_string("�¶�:");
		serial_send_byte(datas%100/10+'0');//ʮλ
		serial_send_byte(datas%10+'0');//��λ
		serial_send_byte(',');
		datas=read_humi();
		//serial_send_string("ʪ��:");
		serial_send_byte(datas%100/10+'0');//ʮλ
		serial_send_byte(datas%10+'0');//��λ
		serial_send_string(":");
		Delay1000ms();
		
		/*delay_ms(1);
		if(COM1.RX_TimeOut > 0)		//��ʱ����
		{
			if(--COM1.RX_TimeOut == 0)
			{
				if(COM1.RX_Cnt > 0)
				{
					for(i=0; i<COM1.RX_Cnt; i++)	TX1_write2buff(RX1_Buffer[i]);	//�յ�������ԭ������
				}
				COM1.RX_Cnt = 0;
			}
		} */
	}
}

void read_byte()
{
u8 i;
for(i=0;i<8;i++)
{
U8FLAG=2;
while((!dht11_io)&&U8FLAG++);//��ʱ3��������
Delay30us();
U8temp=0;
if(dht11_io)U8temp=1;
U8FLAG=2;
while((dht11_io)&&U8FLAG++);//��ʱ3��������
if(U8FLAG==1)break;
U8comdata<<=1;
 U8comdata|=U8temp;
}
}


u8 read_temp()
{
dht11_io=0;
Delay18ms();
dht11_io=1;
Delay40us();
dht11_io=1;
if(!dht11_io)
{
U8FLAG=2;
while((!dht11_io)&&U8FLAG++);//��ʱ3��������
U8FLAG=2;
while((dht11_io)&&U8FLAG++);//��ʱ3��������
read_byte();
 U8RH_data_H_temp=U8comdata;
 read_byte();
 U8RH_data_L_temp=U8comdata;
 read_byte();
 U8T_data_H_temp=U8comdata;
 read_byte();
 U8T_data_L_temp=U8comdata;
 read_byte();
 U8checkdata_temp=U8comdata;
 dht11_io=1;

U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
 if(U8temp==U8checkdata_temp)
 {
  U8RH_data_H=U8RH_data_H_temp;
  U8RH_data_L=U8RH_data_L_temp;
U8T_data_H=U8T_data_H_temp;
  U8T_data_L=U8T_data_L_temp;
  U8checkdata=U8checkdata_temp;
 }
}
return U8T_data_H;
}


u8 read_humi()
{
dht11_io=0;
Delay18ms();
dht11_io=1;
Delay40us();
dht11_io=1;
if(!dht11_io)
{
U8FLAG=2;
while((!dht11_io)&&U8FLAG++);//��ʱ3��������
U8FLAG=2;
while((dht11_io)&&U8FLAG++);//��ʱ3��������
read_byte();
 U8RH_data_H_temp=U8comdata;
 read_byte();
 U8RH_data_L_temp=U8comdata;
 read_byte();
 U8T_data_H_temp=U8comdata;
 read_byte();
 U8T_data_L_temp=U8comdata;
 read_byte();
 U8checkdata_temp=U8comdata;
 dht11_io=1;

U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
 if(U8temp==U8checkdata_temp)
 {
  U8RH_data_H=U8RH_data_H_temp;
  U8RH_data_L=U8RH_data_L_temp;
U8T_data_H=U8T_data_H_temp;
  U8T_data_L=U8T_data_L_temp;
  U8checkdata=U8checkdata_temp;
 }
}
return U8RH_data_H;
}
void serial_send_byte(u8 dat)
{
SBUF=dat;
while(!TI);
TI=0;
}


void serial_send_string(u8 *dat)
{
while(*dat)
{
serial_send_byte(*dat++);
}
}
void Delay30us() //@22.1184MHz��ȷ��ʱ30΢��
{
unsigned char i, j;


i = 1;
j = 162;
do
{
while (--j);
} while (--i);
}


void Delay40us() //@22.1184MHz��ȷ��ʱ40΢��
{
unsigned char i, j;


_nop_();
i = 1;
j = 217;
do
{
while (--j);
} while (--i);
}


void Delay18ms() //@22.1184MHz��ȷ��ʱ18����
{
unsigned char i, j, k;


_nop_();
i = 2;
j = 132;
k = 67;
do
{
do
{
while (--k);
} while (--j);
} while (--i);
}


void Delay1000ms() //@22.1184MHz
{
unsigned char i, j, k;


_nop_();
_nop_();
i = 85;
j = 12;
k = 155;
do
{
do
{
while (--k);
} while (--j);
} while (--i);
}
