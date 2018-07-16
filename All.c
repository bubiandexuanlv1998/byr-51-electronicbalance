#include <reg52.h>
#include <intrins.h>
#include <string.h>



#define LCD1602_PORT P0
sbit LCD1602_RS = P2^6;
sbit LCD1602_RW = P2^5;
sbit LCD1602_EN = P2^7;
//函数或者变量声明
void LCD1602_delay_ms(unsigned int n);
void LCD1602_write_com(unsigned char com);
void LCD1602_write_data(unsigned char dat);
void LCD1602_write_word(unsigned char *s);
void Init_LCD1602();
//****************************************************
//MS延时函数(12M晶振下测试)
//****************************************************
void LCD1602_delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}
//****************************************************
//写指令
//****************************************************
void LCD1602_write_com(unsigned char com)
{
	LCD1602_RS = 0;
	LCD1602_delay_ms(1);
	LCD1602_EN = 1;
	LCD1602_PORT = com;
	LCD1602_delay_ms(1);
	LCD1602_EN = 0;
}
//****************************************************
//写数据
//****************************************************
void LCD1602_write_data(unsigned char dat)
{
	LCD1602_RS = 1;
	LCD1602_delay_ms(1);	
	LCD1602_PORT = dat;
	LCD1602_EN = 1;
	LCD1602_delay_ms(1);
	LCD1602_EN = 0;
}
//****************************************************
//连续写字符
//****************************************************
void LCD1602_write_word(unsigned char *s)
{
	while(*s>0)
	{
		LCD1602_write_data(*s);
		s++;
	}
}
void Init_LCD1602()
{
	LCD1602_EN = 0;
	LCD1602_RW = 0;						//设置为写状态
	LCD1602_write_com(0x38);			//显示模式设定
	LCD1602_write_com(0x0c);			//开关显示、光标有无设置、光标闪烁设置
	LCD1602_write_com(0x06);			//写一个字符后指针加一
	LCD1602_write_com(0x01);			//清屏指令
}



//IO设置
sbit HX711_DOUT=P3^2; 
sbit HX711_SCK=P3^3; 
//函数或者变量声明
void Delay__hx711_us(void);
unsigned long HX711_Read(void);
//****************************************************
//延时函数
//****************************************************
void Delay__hx711_us(void)
{
	_nop_();
	_nop_();
}
//****************************************************
//读取HX711
//****************************************************
unsigned long HX711_Read(void)	//增益128
{
	unsigned long count; 
	unsigned char i; 
  	HX711_DOUT=1; 
	Delay__hx711_us();
  	HX711_SCK=0; 
  	count=0; 
  	while(HX711_DOUT); 
  	for(i=0;i<24;i++)
	{ 
	  	HX711_SCK=1; 
	  	count=count<<1; 
		HX711_SCK=0; 
	  	if(HX711_DOUT)
			count++; 
	} 
 	HX711_SCK=1; 
    count=count^0x800000;//第25个脉冲下降沿来时，转换数据
	Delay__hx711_us();
	HX711_SCK=0;  
	return(count);
}



sbit ROW1=P1^7;
sbit ROW2=P1^6;
sbit ROW3=P1^5;
sbit ROW4=P1^4;
sbit COL1=P1^3;
sbit COL2=P1^2;
sbit COL3=P1^1;
sbit COL4=P1^0;
unsigned char keycode;
bit bdata flag_key;
unsigned char Getkeyboard(void)
{
    unsigned char number = 0;
    unsigned char i;
	//行输入低电平
    ROW1=ROW2=ROW3=ROW4=0;
	//如果列输出不全为高，则说明有键按下
	if (((COL1!=1)||(COL2!=1)||(COL3!=1)||(COL4!=1))&&flag_key==1)
	{
	   //输入0111
	   flag_key=0;
	   ROW1=0;
	   ROW2=ROW3=ROW4=1;
	   for (i=0;i<20;i++);
	   if (COL1==0) return 1;	   // 7
	   else if (COL2==0) return 2;	//	 8
	   else if (COL3==0) return 3;	 //	9
	   else if (COL4==0) return 10;	 //	 10
	   //输入1011
	   ROW2=0;
	   ROW1=ROW3=ROW4=1;
	   for (i=0;i<20;i++);
	   if (COL1==0) return 4;	  //4
	   else if (COL2==0) return 5; //  5
	   else if (COL3==0) return 6;	// 6
	   else if (COL4==0) return 11;	 //	11
	   //输入1101
	   ROW3=0;
	   ROW1=ROW2=ROW4=1;
	   for (i=0;i<20;i++);
	   if (COL1==0) return 7;  //1
	   else if (COL2==0) return 8;	  // 2
	   else if (COL3==0) return 9;	  //   3
	   else if (COL4==0) return 12;	  //   12
	   //输入0111
	   ROW4=0;
	   ROW1=ROW2=ROW3=1;
	   for (i=0;i<20;i++);
	   if (COL1==0) return 14;
	   else if (COL2==0) return 0;
	   else if (COL3==0) return 15;
	   else if (COL4==0) return 13;
	   //没有检测到列低电平
	   return 99;
	}
	else if(COL1==1&&COL2==1&&COL3==1&&COL4==1)
	flag_key=1;
	return 99;
}



//函数或者变量声明
void Delay_ms(unsigned int n);
void Get_Weight();
unsigned int qupi=0;
unsigned long Weight_Shiwu = 0;	 
unsigned char DotPos;				   //小数点标志及位置
unsigned int idata price;     //单价，长整型值，单位为分   
unsigned int idata money;     //总价，长整型值，单位为分
unsigned int Gapvalue0=9273;

//数据初始化
void Data_Init()
{
   price = 0;
   DotPos = 0;
}
//显示单价，单位为元，四位整数，两位小数
void Display_Price()
{
            LCD1602_write_com(0x8c);
			LCD1602_write_data(price/100 + 0x30);
			LCD1602_write_data(price%100/10 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(price%10 + 0x30);
}
//显示重量，单位kg，两位整数，三位小数
void Display_Weight()
{
            LCD1602_write_com(0x83);
			LCD1602_write_data(Weight_Shiwu/1000 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(Weight_Shiwu%1000/100 + 0x30);
			LCD1602_write_data(Weight_Shiwu%100/10 + 0x30);
			LCD1602_write_data(Weight_Shiwu%10 + 0x30);
}
//显示总价，单位为元，四位整数，两位小数
void Display_Money()
{
  // unsigned int i,j;

   if (money>9999) 	//超出显示量程
   {
     LCD1602_write_com(0x80+0x40+6);
     LCD1602_write_word("---.-");
      return;       
   } 
   if (money>=1000)
   {
       LCD1602_write_com(0x80+0x40+6);
	   LCD1602_write_data(money/1000 + 0x30);
	   LCD1602_write_data(money%1000/100 + 0x30);
	   LCD1602_write_data(money%100/10 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10 + 0x30);
   }
   else if (money>=100)
   {
       LCD1602_write_com(0x80+0x40+6);
	   LCD1602_write_data(0x20);
	   LCD1602_write_data(money%1000/100 + 0x30);
	   LCD1602_write_data(money%100/10 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10 + 0x30);
   }
    else if(money>=10)
   {
       LCD1602_write_com(0x80+0x40+6);
	  LCD1602_write_data(0x20);
	    LCD1602_write_com(0x80+0x40+7);
	   LCD1602_write_data(0x20);
	   LCD1602_write_data(money%100/10 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10+ 0x30);
   }   
   else 
     {
       LCD1602_write_com(0x80+0x40+6);
	   LCD1602_write_data(0x20);
	   LCD1602_write_com(0x80+0x40+7);
	   LCD1602_write_data(0x20);
	   LCD1602_write_com(0x80+0x40+8);
	   LCD1602_write_data(0 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10 + 0x30);
   }         
}
//按键处理
//按键响应程序，参数是键值
//返回键值：
//         1    2    3    10	   //10:清零重量，兼去皮功能	
//         4    5    6    11	   //11:清除单价
//         7    8    9    12	   //12:校准增加重量 
//         14   0    15   13	   //14:无功能（用于扩展）15：小数点   13：校准减少重量 
void KeyPress(unsigned char keycode)
{
	switch (keycode)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:	//目前在设置整数位，要注意price是整型，存储单位为分
			if (DotPos == 0)
			{			//最多只能设置到千位
				if (price<100)
				{
					price=price*10+keycode*10;
				}
			}//目前在设置小数位
			else if (DotPos==1)  //小数点后第一位
			{
				price=price+keycode;
				DotPos=2;
			}
			Display_Price();
			break;
		case 10:   //去皮键
			if(qupi==0)
				qupi=Weight_Shiwu;
			else
				qupi=0;
			Display_Price();
			//		 FlagSetPrice = 0;
			DotPos = 0;
			break;
		case 11:	//删除键
			price=0;
			DotPos=0;
			Display_Price();
			break;
		case 12:	   //加
			Gapvalue0--;	
			break;
		case 13:   //减
			Gapvalue0++;
			break;
		case 15:   //小数点按下 
			DotPos = 1;      //小数点后第一位
			break;
   }
}
//****************************************************
//称重
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read()*10/9100-Gapvalue0-qupi;
	if(Weight_Shiwu<1)	 Weight_Shiwu=0;															
	if(Weight_Shiwu > 2000)		//超重
	{
		LCD1602_write_com(0x83);
	   LCD1602_write_word("-.---");
	}
	else
	{
		Display_Weight();
		money = Weight_Shiwu*price/1000;  //money单位为分
	       //显示总金额
	       Display_Money();
	}
}



//****************************************************
//主函数
//****************************************************
void main()
{
	Init_LCD1602();									//初始化LCD1602
   EA = 0;
   Data_Init();
   EA = 1;
   LCD1602_write_com(0x80);						//指针设置
   LCD1602_write_word("WE:0.000 PR:00.0");
   LCD1602_write_com(0x80+0x40);				//指针设置
   LCD1602_write_word("MONEY:  0.0     ");
   Display_Price();
	while(1)
	{
	 Get_Weight();
   	LCD1602_delay_ms(100);
	keycode = Getkeyboard();
	if (keycode<16)
	  KeyPress(keycode);	  	
	}
}
