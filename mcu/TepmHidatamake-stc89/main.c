#include "reg52.h"			 //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include <intrins.h>
typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;
// #define u8 unsigned char
// #define u16 unsigned int
sbit Data=P0^0;   //����������
u8 rec_dat[5];   //������ʾ�Ľ�����������

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

//���ڳ�ʼ��
void UsartInit()
{
	SCON=0X50;			//����Ϊ������ʽ1
	TMOD=0X20;			//���ü�����������ʽ2
	PCON=0X80;			//�����ʼӱ�
	TH1=0XFF;				//��������ʼֵ���ã�ע�Ⲩ������57600��
	TL1=0XFF;
	ES=1;						//�򿪽����ж�
	EA=1;						//�����ж�
	TR1=1;					//�򿪼�����
}
//DHT11��ʼ��
void DHT11_start()
{
   Data=1;
   DHT11_delay_us(2);
   Data=0;
   DHT11_delay_ms(20);   //��ʱ18ms����
   Data=1;
   DHT11_delay_us(30);
}

u8 DHT11_rec_byte()      //����һ���ֽ�
{
   u8 i,dat=0;
  for(i=0;i<8;i++)    //�Ӹߵ������ν���8λ����
   {          
      while(!Data);  // �ȴ�50us�͵�ƽ��ȥ
      DHT11_delay_us(8);     //��ʱ60us�������Ϊ��������Ϊ1������Ϊ0 
      dat<<=1;           //��λʹ��ȷ����8λ���ݣ�����Ϊ0ʱֱ����λ
      if(Data==1)    //����Ϊ1ʱ��ʹdat��1����������1
         dat+=1;
      while(Data);  //�ȴ�����������    
    }  
    return dat;
}

void DHT11_receive()      //����40λ������
{
    u8 R_H,R_L,T_H,T_L,RH,RL,TH,TL,revise; 
    DHT11_start();
    if(Data==0)
    {
        while(Data==0);   //�ȴ�����     
        DHT11_delay_us(40);  //���ߺ���ʱ80us
        R_H=DHT11_rec_byte();    //����ʪ�ȸ߰�λ  
        R_L=DHT11_rec_byte();    //����ʪ�ȵͰ�λ  
        T_H=DHT11_rec_byte();    //�����¶ȸ߰�λ  
        T_L=DHT11_rec_byte();    //�����¶ȵͰ�λ
        revise=DHT11_rec_byte(); //����У��λ

        DHT11_delay_us(25);    //����

        if((R_H+R_L+T_H+T_L)==revise)      //У��
        {
            RH=R_H;
            RL=R_L;
            TH=T_H;
            TL=T_L;
        } 
        /*
		���ݴ���������ʾ ���ݸ�ʽ ʪ��,�¶�
		*/
        rec_dat[0]='0'+(RH/10);
        rec_dat[1]='0'+(RH%10);
        rec_dat[2]=',';
        rec_dat[3]='0'+(TH/10);
        rec_dat[4]='0'+(TH%10);
    }
}

//���ڷ���һ���ֽ�����
void UART_SendByte(u8 Byte){
	SBUF=Byte;
	//����Ƿ����
	while(TI==0);
	TI=0;//TI��λ
}

int main()
{
   	u8 i;   
   	UsartInit();
	DHT11_delay_ms(1500);//DHT11�ϵ��Ҫ�ȴ�1S��Խ�����ȶ�״̬�ڴ��ڼ䲻�ܷ����κ�ָ��
   	while(1)
   	{   
        UART_SendByte('2');  
		DHT11_receive();
		//    lcd_write_command(0x80);   //��lcd1602��һ�е�һ��λ�ÿ�ʼ��ʾ
		for(i=0;i<5;i++){
		//��ʾ���� 
		UART_SendByte(rec_dat[i]);
	   	}
		UART_SendByte('\n');
		DHT11_delay_ms(1500);
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