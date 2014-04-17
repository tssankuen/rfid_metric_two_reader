#ifndef _COMDEF_H_
#define _COMDEF_H_

#define uchar           unsigned char
#define uint            unsigned int
#define ulong           unsigned long

#define	FCPU	7372800		// 单片机主频为7.3728MHz,用于延时子程序 
#define f_count         184              	//定时器初值，定时器加一计数
#define timer_clk       0x07				//时钟1024分频

#define delay_us(x)     _delay_us(x)    	//AVR GCC延时函数 x(us)
#define delay_ms(x)     _delay_ms(x)    	//AVR GCC延时函数 x(ms)

#define Data_IO	        PORTA                //数码管数据口
#define Data_DDR        DDRA                 //数码管数据口方向寄存器
#define D_LE0	        PORTD &= ~(1 << PD4) //数码管段控制位为0，锁存端口数据
#define D_LE1           PORTD |= (1 << PD4)  //数码管段控制位为1，锁存器输出与端口一致
#define W_LE0	        PORTD &= ~(1 << PD5) //数码管位控制位为0
#define W_LE1           PORTD |= (1 << PD5)  //数码管位控制位为1

#define baud           9600                                     //设置波特率的大小
#define baud_setting   (uint)((ulong)FCPU/(16*(ulong)baud)-1)  //波特率计算公式
#define baud_h         (uchar)(baud_setting>>8)                 //提取高位
#define baud_l         (uchar)(baud_setting)                    //低位


//***********************************************************************
//  sound   beep
//***********************************************************************
#define beep0             PORTD &= ~(1 << PD6)    //蜂鸣器所在IO口PD6
#define beep1             PORTD |= (1 << PD6)
#define LED8              PORTB                  //LED控制口PB

#define delay_nms(X)        _delay_ms(X)     
 
#define SoundON             PORTD &= ~(1 << PD6)    		//蜂鸣器所在IO口PD6
#define SoundOFF            PORTD |= (1 << PD6)
/*******************************************************************************
* DS1302相关
********************************************************************************/
/*复位脚*/
#define RST_CLR	PORTE &= ~(1 << PE7)              //电平置低
#define RST_SET	PORTE |= (1 << PE7)               //电平置高
#define RST_IN	DDRE &= ~(1 << PE7)               //方向输入
#define RST_OUT	DDRE |= (1 << PE7)                //方向输出

/*双向数据*/
#define IO_CLR	PORTE &= ~(1 << PE6)              //电平置低
#define IO_SET	PORTE |= (1 << PE6)               //电平置高
#define IO_R	PINE & (1 << PE6)                 //电平读取
#define IO_IN	DDRE &= ~(1 << PE6)               //方向输入
#define IO_OUT	DDRE |= (1 << PE6)                //方向输出

/*时钟信号*/
#define SCK_CLR	PORTE &= ~(1 << PE5)              //时钟信号
#define SCK_SET	PORTE |= (1 << PE5)               //电平置高
#define SCK_IN	DDRE &= ~(1 << PE5)               //方向输入
#define SCK_OUT	DDRE |= (1 << PE5)                //方向输出

/*ds1302命令与相关地址*/
#define ds1302_sec_add			0x80		//秒数据地址
#define ds1302_min_add			0x82		//分数据地址
#define ds1302_hr_add			0x84		//时数据地址
#define ds1302_date_add			0x86		//日数据地址
#define ds1302_month_add		0x88		//月数据地址
#define ds1302_day_add			0x8a		//星期数据地址
#define ds1302_year_add			0x8c		//年数据地址
#define ds1302_control_add		0x8e		//控制数据地址
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe

#define RS_CLR	PORTF &= ~(1 << PF1)       //RS置低
#define RS_SET	PORTF |= (1 << PF1)        //RS置高

#define RW_CLR	PORTF &= ~(1 << PF2)       //RW置低
#define RW_SET	PORTF |= (1 << PF2)        //RW置高

#define EN_CLR	PORTF &= ~(1 << PF3)       //E置低
#define EN_SET	PORTF |= (1 << PF3)        //E置高


#define LIGHT_OFF	PORTE &= ~(1 << PE3)       //PE3置低
#define LIGHT_ON	PORTE |= (1 << PE3)        //PE3置高
/*******************************************************************************
                      event_table 全局事件状态标志
********************************************************************************/

#define EV_START_METRIC			0x01			//
#define EV_END_METRIC			0x02			//
#define EV_GET_DEPTH			0x04			//
#define EV_USART1_RECV			0x08			//
#define EV_INT1_KEY  			0x10			//
#define EV_USART0_RECV  		0x20			//
#define SET_EVT(X) event_table |= X;
#define CLR_EVT(X)	event_table &= ~X; 

/*******************************************************************************
                      深度测量 命令控制字
********************************************************************************/

/*******************************************************************************
                     USART1接收缓冲区长度
********************************************************************************/
#define UARTRCVBUFLEN	        17

#define CMD_LEN                  5
#define PARA_LEN                 8


#define TAGBUF_LEN              30              //最多存30个标签
#define TAG_LEN                 12              //tag 
#define INTEVAL                  3              //每个标签代表的间隔距离 m



#define ENCRIPT_KEY             0x5A            //加解密密钥
#define DISKBUFLEN                23             // YYYY-MM-DD HH:MM DATA \r\n\0



#define key_io          PORTC               //独立键盘与PC4和PC5连接
#define key_ior         PINC                //读取IO口值为PINC的值

#define KEY_UP     0x01
#define KEY_DOWN   0x09
#define KEY_SET    0x05
#define KEY_LEFT   0x04
#define KEY_RIGHT  0x06
#define KEY_QUIT   0x03
#endif
