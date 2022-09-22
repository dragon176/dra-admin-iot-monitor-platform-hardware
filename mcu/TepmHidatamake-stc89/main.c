#include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器
#include <intrins.h>
typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;
// #define u8 unsigned char
// #define u16 unsigned int
sbit Data=P0^0;   //定义数据线
u8 rec_dat[5];   //用于显示的接收数据数组

void DHT11_delay_us(u8 n)
{
    while(--n);
}

void DHT11_delay_ms(u16 z)
{
   u16 i,j;
   for(i=z;i>0;i--)
      for(j=110;j>0;j--);
}

//串口初始化
void UsartInit()
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XFF;				//计数器初始值设置，注意波特率是57600的
	TL1=0XFF;
	ES=1;						//打开接收中断
	EA=1;						//打开总中断
	TR1=1;					//打开计数器
}
//DHT11初始化
void DHT11_start()
{
   Data=1;
   DHT11_delay_us(2);
   Data=0;
   DHT11_delay_ms(20);   //延时18ms以上
   Data=1;
   DHT11_delay_us(30);
}

u8 DHT11_rec_byte()      //接收一个字节
{
   u8 i,dat=0;
  for(i=0;i<8;i++)    //从高到低依次接收8位数据
   {          
      while(!Data);  // 等待50us低电平过去
      DHT11_delay_us(8);     //延时60us，如果还为高则数据为1，否则为0 
      dat<<=1;           //移位使正确接收8位数据，数据为0时直接移位
      if(Data==1)    //数据为1时，使dat加1来接收数据1
         dat+=1;
      while(Data);  //等待数据线拉低    
    }  
    return dat;
}

void DHT11_receive()      //接收40位的数据
{
    u8 R_H,R_L,T_H,T_L,RH,RL,TH,TL,revise; 
    DHT11_start();
    if(Data==0)
    {
        while(Data==0);   //等待拉高     
        DHT11_delay_us(40);  //拉高后延时80us
        R_H=DHT11_rec_byte();    //接收湿度高八位  
        R_L=DHT11_rec_byte();    //接收湿度低八位  
        T_H=DHT11_rec_byte();    //接收温度高八位  
        T_L=DHT11_rec_byte();    //接收温度低八位
        revise=DHT11_rec_byte(); //接收校正位

        DHT11_delay_us(25);    //结束

        if((R_H+R_L+T_H+T_L)==revise)      //校正
        {
            RH=R_H;
            RL=R_L;
            TH=T_H;
            TL=T_L;
        } 
        /*
		数据处理，方便显示 数据格式 湿度,温度
		*/
        rec_dat[0]='0'+(RH/10);
        rec_dat[1]='0'+(RH%10);
        rec_dat[2]=',';
        rec_dat[3]='0'+(TH/10);
        rec_dat[4]='0'+(TH%10);
    }
}

//串口发送一个字节数据
void UART_SendByte(u8 Byte){
	SBUF=Byte;
	//检测是否完成
	while(TI==0);
	TI=0;//TI复位
}

int main()
{
   	u8 i;   
   	UsartInit();
	DHT11_delay_ms(1500);//DHT11上电后要等待1S以越过不稳定状态在此期间不能发送任何指令
   	while(1)
   	{   
        UART_SendByte('2');  
		DHT11_receive();
		//    lcd_write_command(0x80);   //从lcd1602第一行第一个位置开始显示
		for(i=0;i<5;i++){
		//显示数据 
		UART_SendByte(rec_dat[i]);
	   	}
		UART_SendByte('\n');
		DHT11_delay_ms(1500);
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