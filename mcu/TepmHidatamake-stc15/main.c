
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
#define dht11_io P33   //P2.0口作为数据读取接口


/*************	功能说明	**************

双串口全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/
u16 U8FLAG;
u8  U8temp;
u8  U8T_data_H,U8T_data_L,U8RH_data_H,U8RH_data_L,U8checkdata;
u8  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
u8  U8comdata;
u8 datas=0;

/*************	本地函数声明	**************/
void  read_byte(); //读取一字节的数据,并赋值给U8comdata
u8  read_temp();//读取温度，返回温度值
u8  read_humi();//读取湿度，返回湿度值


void  Delay30us(); //22.1184M晶振精确延时30微秒
void  Delay40us(); //22.1184M晶振精确延时40微秒
void  Delay18ms(); //22.1184M晶振精确延时30毫秒
void Delay1000ms();//22.1184M晶振精确延时1000毫秒
void serial_send_byte(u8 dat);
void serial_send_string(u8 *dat);

/*************  外部函数和变量声明 *****************/


/*************  串口1初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	//PrintString1("STC15F2K60S2 UART1 Test Prgramme!\r\n");	//SUART1发送一个字符串
}


/**********************************************/
void main(void)
{
	Delay1000ms();
	//u8 i;

	UART_config();
	EA = 1;

	dht11_io = 1;
	//DHT11_delay_ms(1500);//DHT11上电后要等待1S以越过不稳定状态在此期间不能发送任何指令
	while (1)
	{
		datas=read_temp();
		//serial_send_string("温度:");
		serial_send_byte(datas%100/10+'0');//十位
		serial_send_byte(datas%10+'0');//个位
		serial_send_byte(',');
		datas=read_humi();
		//serial_send_string("湿度:");
		serial_send_byte(datas%100/10+'0');//十位
		serial_send_byte(datas%10+'0');//个位
		serial_send_string(":");
		Delay1000ms();
		
		/*delay_ms(1);
		if(COM1.RX_TimeOut > 0)		//超时计数
		{
			if(--COM1.RX_TimeOut == 0)
			{
				if(COM1.RX_Cnt > 0)
				{
					for(i=0; i<COM1.RX_Cnt; i++)	TX1_write2buff(RX1_Buffer[i]);	//收到的数据原样返回
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
while((!dht11_io)&&U8FLAG++);//超时3毫秒跳出
Delay30us();
U8temp=0;
if(dht11_io)U8temp=1;
U8FLAG=2;
while((dht11_io)&&U8FLAG++);//超时3毫秒跳出
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
while((!dht11_io)&&U8FLAG++);//超时3毫秒跳出
U8FLAG=2;
while((dht11_io)&&U8FLAG++);//超时3毫秒跳出
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
while((!dht11_io)&&U8FLAG++);//超时3毫秒跳出
U8FLAG=2;
while((dht11_io)&&U8FLAG++);//超时3毫秒跳出
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
void Delay30us() //@22.1184MHz精确延时30微秒
{
unsigned char i, j;


i = 1;
j = 162;
do
{
while (--j);
} while (--i);
}


void Delay40us() //@22.1184MHz精确延时40微秒
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


void Delay18ms() //@22.1184MHz精确延时18毫秒
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
