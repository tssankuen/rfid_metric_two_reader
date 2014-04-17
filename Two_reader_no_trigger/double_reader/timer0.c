#include "comm.h"

//*************************************************************************
//			定时器0初始化子程序
//*************************************************************************
//ucint time_1s_count=0;

void timer0_init()                                 //定时器初始化
{
  TCCR0=0x07;                                      //普通模式，OC0不输出，1024分频
  TCNT0=f_count;                                    //初值，定时为10ms
  TIFR=0x01;                                       //清中断标志位
  TIMSK=0x01;                                      //使能定时器0溢出中断
}

//*************************************************************************
//		定时器0中断服务子程序
//*************************************************************************
ISR(SIG_OVERFLOW0)                           	//中断服务程序
{
    
	TCNT0=f_count;                              //定时器赋初值，非自动重装模式
   if(time_1s_count!=100)
      {
		  time_1s_count++;                          //定时器定时10ms，计数100次为3s

	  }
   else
      {
	      //未满10s
          if(second != 10)
		  {
               second++;
		  }
		  else
		  {
		       //秒清零
	           second=0; 
               printf("end timer\n");
               //flag标志清空 表示在读到的上个标签的10秒内没有多标签
               multi_tag_flag_reader1 = 0;
               multi_tag_flag_reader2 = 0;
			   //关闭定时器0
              TCCR0=0x00;
		  }

		time_1s_count=0;
	  }
}
