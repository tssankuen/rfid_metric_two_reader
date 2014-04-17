
#include "comm.h"

int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);
FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar,_FDEV_SETUP_RW);



//*************************************************************************
//			遥控器解码子程序
//*************************************************************************


//红外遥控器解码检测



//*********************************************************************************


void port_init()
{
    Data_IO = 0xFF;				        //电平设置
    Data_DDR = 0xFF;				    //方向输出

    PORTD=0xFF;               		
    DDRD=0xFC;                //PD0 PD1 输入

    PORTB=0XFF;
    DDRB=0XFF;

    PORTF = 0xFF;                       //电平设置
    DDRF = 0xFF;                        //方向输出

    DDRC=0x0f;                        //PC口上拉电阻使能
    PORTC=0xf0;                       //PC高四位输入低四位输出

    D_LE1;                             //关掉数码管，以免显示乱码                   
    W_LE1;                     
    Data_IO=0xFF;                      //关数码管            
    W_LE0; 

    PORTE |=0x18;
    DDRE  |=0x18;
}

void system_init(void)
{
	port_init();
    ext_int0_init();
	usart1_init();
    usart0_init();
	LCD_init();                           //1602液晶初始化
	ds1302_init();  
	mInitSTDIO( );  /* 为了让计算机通过串口监控演示过程 */
	//stop read rfid tags 
	usart1_buf_send(edRead,CMD_LEN);
	usart0_buf_send(edRead,CMD_LEN);
	init_ReaderPara();


 }

void main(void)
{
	UINT8	s;
	unsigned char temp;

	system_init();
    //for debug
//    stdout=&mystdout;
//    stdin=&mystdout;

	delay_ms(200);

    ds1302_read_time();
 

    {
        temp = (time_buf[0] >> 4) + '0';   //在LCD上写要显示的数据
        LCD_write_char(0, 0, temp);/*年*/
        temp = (time_buf[0] & 0x0F) + '0';
        LCD_write_char(1, 0, temp);
        temp = (time_buf[1] >> 4) + '0';
        LCD_write_char(2, 0, temp);
        temp = (time_buf[1] & 0x0F) + '0';
        LCD_write_char(3, 0, temp);
        LCD_write_char(4, 0, '-');
	
        temp = (time_buf[2] >> 4) + '0';
        LCD_write_char(5, 0, temp);/*月*/
        temp = (time_buf[2] & 0x0F) + '0';
        LCD_write_char(6, 0, temp);
        LCD_write_char(7, 0, '-');
	
        temp = (time_buf[3] >> 4) + '0';
        LCD_write_char(8, 0, temp);/*日*/
        temp = (time_buf[3] & 0x0F) + '0';
        LCD_write_char(9, 0, temp);
	
        temp = (time_buf[4] >> 4) + '0';
        LCD_write_char(8, 1, temp);/*时*/
        temp = (time_buf[4] & 0x0F) + '0';
        LCD_write_char(9, 1, temp);
        LCD_write_char(10, 1, ':');
	
        temp = (time_buf[5] >> 4) + '0';
        LCD_write_char(11, 1, temp);/*分*/
        temp = (time_buf[5] & 0x0F) + '0';
        LCD_write_char(12, 1, temp);
        LCD_write_char(13, 1, ':');
	
       temp = (time_buf[6] >> 4) + '0';
        LCD_write_char(14, 1, temp);/*秒*/
        temp = (time_buf[6] & 0x0F) + '0';
        LCD_write_char(15, 1, temp);
    }
	beep_ms(200);	
    delay_ms(200);		
	LCD_clear(); 



    event_table = 0;
	SREG|=0x80;                                //开启全局中断

	s = mInitCH376Host( );  /* 初始化CH376 */
//    printf("s=%d",s);    
	mStopIfError( s );

    //delay 2s
	for(char i=0;i<10;i++)
	{ delay_ms(200);}

    //上电检查U盘是否接在系统上。
	LCD_clear();
	LCD_write_str(0,0,"wait-Udisk");


    //为了增加稳定性 增加看门狗电路
    wdt_enable(WDTO_1S);//使能看门狗定时器，溢出时间设置为1秒
    wdt_reset();        //喂狗

	while ( CH376DiskConnect( ) != USB_INT_SUCCESS ) {  /* 检查U盘是否连接,等待U盘插入,对于SD卡,可以由单片机直接查询SD卡座的插拔状态引脚 */
			mDelaymS( 100 );
			wdt_reset();//喂狗
		}
	mDelaymS( 200 );  /* 延时,可选操作,有的USB存储器需要几十毫秒的延时 */

    /* 对于检测到USB设备的,最多等待10*50mS */
	for ( s = 0; s < 10; s ++ ) {  /* 最长等待时间,10*50mS */
		mDelaymS( 50 );
		if ( CH376DiskMount( ) == USB_INT_SUCCESS ) break;  /* 初始化磁盘并测试磁盘是否就绪 */
		wdt_reset();//喂狗
	}

//	printf( "Udisk ON\n" );
	LCD_clear();
	LCD_write_str(0,0,"Udisk ON");
	LCD_clear();
	beep_ms(200);

    reader_init();

	while(1)
	{

        //查询判断有没有按键 设置默认打桩深度
        parsekey();
		if (event_table & EV_INT1_KEY)
		{
			Setkey_handler();
			if (event_table == 0)
				continue;
		}
		wdt_reset();//喂狗
	}
}
//**********************************************************************************************************************************


