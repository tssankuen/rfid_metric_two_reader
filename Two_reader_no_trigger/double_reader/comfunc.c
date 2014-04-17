#include "comm.h"

/*************************************************************************
*     global event table
*************************************************************************/
volatile uint event_table = 0;

//*************************************************************************
//    global max_depth
//    每次采集时测量到的有效最大深度。量程最大60米 <65535 用Int 两字节存储够了
//*************************************************************************/
uchar max_depth = 0;
uint cur_depth = 0;
/*************************************************************************
*     global variable
*************************************************************************/
uchar table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

//uchar  Title[]={"年-月-日 时：分  深度(m)"};//获取系统 
//加密过后的标题编码 \r\n
uchar  Title[]={0x9e,0xb0,0x77,0x8e,0x98,0x77,0x92,0x8f,0x7a,0x90,0xeb,0xf9,0xe0,0xed,0x8c,0x7a,0x7a,0x93,0xb4,0xec,0x92,0x72,0x37,0x73,0x0d,0x0a,0x00};

uchar DiskBuf[DISKBUFLEN] = {0x00};    //DISKBUFLEN=23 // YYYY-MM-DD HH:MM DATA \r\n\0
uchar EncBuf[DISKBUFLEN] = {0x00};
//***********************************************************************
//  USART1 RCV buff
//***********************************************************************
uchar uart1_rcv_buf[UARTRCVBUFLEN];
uchar r_ptr = 0;
uchar r_buf_full = 0;


//***********************************************************************
//  TAG buff
//***********************************************************************
uchar Tag_buf[TAGBUF_LEN][TAG_LEN];
uchar tag_ptr = 0;
uchar tag_buf_full = 0;
uchar (*pTagBuf)[TAG_LEN]=Tag_buf;
//***********************************************************************
//  DS1302    当前时间
//***********************************************************************
uchar time_buf[8] = {0x20,0x10,0x01,0x14,0x09,0x55,0x00,0x02};
uint time_1s_count;
uint second;
uchar tmp = 0;
//***********************************************************************
//  display
//***********************************************************************

//***********************************************************************
//			共阴数码管显示的断码表0～F
//***********************************************************************
 uchar  A1;
 uchar  A2;
 uchar  A3;
 uchar  A4;

 uchar  B1;
 uchar  B2;
 uchar  B3;

 uchar  enc_B1;
 uchar  enc_B2;
 uchar  enc_B3;


uchar usart0_rx_data;
uchar usart1_rx_data;

/***********************************************************************
* key config default depth
************************************************************************/
uchar setkey_cnt = 0;
unsigned char std_depth1;
unsigned char std_depth2;
uchar stepkey;

uchar reader1_depth;
uchar reader2_depth;

//***********************************************************************
//  USART0 RCV buff
//***********************************************************************
uchar uart0_rcv_buf[UARTRCVBUFLEN];
uchar r_ptr_u0 = 0;
uchar r_buf_full_u0 = 0;

//***********************************************************************
//  RFID reader 命令控制字
//***********************************************************************

const uchar stRead[CMD_LEN] = {0xA0,0x03,0x65,0x00,0xF8};
const uchar edRead[CMD_LEN] = {0xA0,0x03,0xA8,0x00,0xB5};

const uchar setPara[PARA_LEN] = {0xA0,0x06,0x60,0x00,0x00,0x70,0x02,0x88};
const uchar stReadRsp[6] = {0xE4,0x04,0x65,0x00,0x00,0xB3}; //RESET READER  RSP OK 
const uchar edReadRsp[6] = {0xE0,0x04,0xA8,0X00,0x00,0x74};  //STOP READ RSP OK

//***********************************************************************
//  节电功能 当一段时间没有数据收发 即没有打桩事件 则LCD 关闭 节电
//***********************************************************************
uchar save_mode = 0;
uchar multi_tag_flag = 0;
uchar multi_tag_flag_reader1 = 0;
uchar multi_tag_flag_reader2 = 0;
 void data_do(uint temp_d)
 {

     uint A2t,tp1;
     tp1=temp_d/100;                       //分出百，十，和个位
     A2t=temp_d%100;

     A1=tp1/10;
	 A2=tp1%10;
     A3=A2t/10;
     A4=A2t%10;

}
 void data_ascii(uchar dep)
 {
     uchar B2t;
     B1=dep/100;                       //分出百，十，和个位
     B2t=dep%100;
     B2=B2t/10;
     B3=B2t%10;

	 B1+='0';   //转成ASCII字符
	 B2+='0';  
	 B3+='0';
}
/*************************************************************************
*     beep
*************************************************************************/
void beep_ms(uchar utime)
{
	beep0;             //开蜂鸣器
	delay_ms(utime);    //让它响
	beep1;            //关蜂鸣器 
	delay_ms(utime);    //
}

/*************************************************************************
*     display
*************************************************************************/

//***********************************************************************
//	显示屏命令写入函数
//***********************************************************************


void LCD_write_com(unsigned char com) 
{	
	RS_CLR;
	RW_CLR;
	EN_SET;
	Data_IO = com;
	delay_ms(5);
	EN_CLR;
}
//***********************************************************************
//	显示屏数据写入函数
//***********************************************************************

void LCD_write_data(unsigned char data) 
{
	RS_SET;
	RW_CLR;
	EN_SET;
	Data_IO = data;
	delay_ms(5);
	EN_CLR;
}
//***********************************************************************
//	显示屏清空显示
//***********************************************************************

void LCD_clear(void) 
{
	LCD_write_com(0x01);
	delay_ms(5);
}
//***********************************************************************
//	显示屏字符串写入函数
//***********************************************************************

void LCD_write_str(unsigned char x,unsigned char y,unsigned char *s) 
{
	
    if (y == 0) 
    {
    	LCD_write_com(0x80 + x);
    }
    else 
    {
    	LCD_write_com(0xC0 + x);
    }
    
    while (*s) 
    {
    	LCD_write_data( *s);
    	s ++;
    }
}
//***********************************************************************
//	显示屏单字符写入函数
//***********************************************************************

void LCD_write_char(unsigned char x,unsigned char y,unsigned char data) 
{
	
    if (y == 0) 
    {
    	LCD_write_com(0x80 + x);
    }
    else 
    {
    	LCD_write_com(0xC0 + x);
    }
    
    LCD_write_data( data);  
}
//***********************************************************************
//	显示屏初始化函数
//***********************************************************************

void LCD_init(void) 
{

	LCD_write_com(0x38);				//显示模式设置
	delay_ms(5);
	LCD_write_com(0x08);				//显示关闭
	delay_ms(5);
    LCD_write_com(0x01);				//显示清屏
	delay_ms(5);
    LCD_write_com(0x06);				//显示光标移动设置
	delay_ms(5);
	LCD_write_com(0x0C);				//显示开及光标设置
    delay_ms(5);
}

void LCD_close(void)
{
	LCD_write_com(0x08);				//显示关闭
	delay_ms(5);
}
//***********************************************************************
//	RFID READER checksum
//***********************************************************************
uchar GetCheckSum(uchar* buf, uchar len)
{
	uchar sum = 0;
	for(uint i = 0; i < len; i++)
		sum = sum + buf[i];
	sum = (~sum) + 1;
	return sum;
}


void usart1_recv_handler()
{

	//todo
    if(!r_buf_full)
    {
	    //not full 响应数据还没有全部接收--还没有接收完一帧数据
	  	uart1_rcv_buf_additem(usart1_rx_data);	
	    LED8 =~0x20;
        if(r_ptr == 6)
        {
            if( memcmp((void *)uart1_rcv_buf,(void *)stReadRsp,6) == 0 || memcmp((void *)uart1_rcv_buf,(void *)edReadRsp,6) == 0)  //rsp 6 bytes
            {
                 uart1_rcv_buf_reset();
            }
        }
         printf("1");
    }
	else
	{

	    //数据合法有效 	
	    LED8=~0x80;

        //指示灯闪一下 表示到深度了 只要reader2读到标签就算  灯闪一下
        LIGHT_OFF;
	    delay_ms(200);
        LIGHT_ON;
	    delay_ms(200);

        //阅读器2接收到一个有效标签 设定reader2深度有效 。
        reader2_depth =  1;
        uart1_rcv_buf_reset();	
        //multi_tag_flag 存储上一次是哪个reader读到的 multi_tag_flag =0 初始值 multi_tag_flag =1 reader1读到 multi_tag_flag=2 reader2读到
        if( multi_tag_flag != 2 )
        {
            LCD_write_str(0,1,"get depth");
            add_queue(2);    //1:reader1 2:reader

            if( is_lost_reader1())  //queue : 2-1-2 这种情况 即有可能reader1 没读到 丢失一次。
			{
		        Receive_OK_handler();
                queue_shift();
				multi_tag_flag=0;
				return;
			}
            multi_tag_flag = 2;
         }

	}

}

//***********************************************************************
//	                USART1 接收缓冲区相关函数
//
//***********************************************************************


void uart1_rcv_buf_init(void)
{
  r_ptr = 0;
  r_buf_full = 0;
  for(unsigned char i=0; i<UARTRCVBUFLEN; i++)
  {
      uart1_rcv_buf[i] = 0;
  }    
}

void uart1_rcv_buf_reset(void)
{
    uart1_rcv_buf_init();
}

uchar uart1_rcv_buf_isfull(void)
{
    if( r_ptr == UARTRCVBUFLEN-1)
	{
	    r_buf_full = 1;
	}
	else
	{
	    r_buf_full = 0;
	}
	return r_buf_full;
}


void uart1_rcv_buf_additem(uchar data)
{

    if (r_buf_full == 1)
    {
	    //full  flush buffer and add one item
		uart1_rcv_buf_reset();
	}

	//add one item 
	uart1_rcv_buf[r_ptr] = data;
	r_ptr++;
	if(r_ptr == UARTRCVBUFLEN-1)
	{
	    r_buf_full = 1;
	}

}
//*************************************************************************************************************************
void usart0_recv_handler()
{
    CLR_EVT(EV_USART0_RECV); 

	//todo
    if(!r_buf_full_u0)
	{
	    //not full 响应数据还没有全部接收--还没有接收完一帧数据
	  	uart0_rcv_buf_additem(usart0_rx_data);	
	    LED8 =~0x20;
        if(r_ptr_u0 == 6)
        {
           if( memcmp((void *)uart0_rcv_buf,(void *)stReadRsp,6) == 0 || memcmp((void *)uart0_rcv_buf,(void *)edReadRsp,6) == 0)  //rsp 6 bytes
           {

                uart0_rcv_buf_reset();

           }
        }

	}
	else
	{
	    //接收完一帧数据
//	   if( uart0_rcv_buf[0]!=0x00 || uart0_rcv_buf[15] == GetCheckSum(uart0_rcv_buf[0],15))
//	   {
//			uart0_rcv_buf_reset();	
//			LED8=~0x40;
//	   }
//	   else
//	   {
			//数据合法有效 	
			LED8=~0x80;



            //阅读器1接收到一个有效标签 设定reader1深度有效 。
			reader1_depth =  1;
			uart0_rcv_buf_reset();	

            if( multi_tag_flag != 1 )
			{
	            LCD_clear();
                LCD_write_str(0,0,"start metric");

                add_queue(1);    //1:reader1 2:reader
                if( is_one_cnt())
				{
			        Receive_OK_handler();
                    queue_reset();
					multi_tag_flag=0;
					return;
				}
				multi_tag_flag = 1;

			}
	}

}

//***********************************************************************
//	                USART0 接收缓冲区相关函数
//
//***********************************************************************


void uart0_rcv_buf_init(void)
{
  r_ptr_u0 = 0;
  r_buf_full_u0 = 0;
  for(unsigned char i=0; i<UARTRCVBUFLEN; i++)
  {
      uart0_rcv_buf[i] = 0;
  }    
}

void uart0_rcv_buf_reset(void)
{
    uart0_rcv_buf_init();
}

uchar uart0_rcv_buf_isfull(void)
{
    if( r_ptr_u0 == UARTRCVBUFLEN-1)
	{
	    r_buf_full_u0 = 1;
	}
	else
	{
	    r_buf_full_u0 = 0;
	}
	return r_buf_full_u0;
}


void uart0_rcv_buf_additem(uchar data)
{

    if (r_buf_full_u0 == 1)
    {
	    //full  flush buffer and add one item
		uart0_rcv_buf_reset();
	}

	//add one item 
	uart0_rcv_buf[r_ptr_u0] = data;
	r_ptr_u0++;
	if(r_ptr_u0 == UARTRCVBUFLEN-1)
	{
	    r_buf_full_u0 = 1;
	}

}

//***********************************************************************
//  TAG buff
//uchar Tag_buf[TAGBUF_LEN][TAG_LEN];
//uchar tag_ptr = 0;
//uchar tag_buf_full = 0;
//***********************************************************************

void tagbuf_init(void)
{
  tag_ptr = 0;
  tag_buf_full = 0;
  (*pTagBuf)[TAG_LEN]=Tag_buf;
  for(unsigned char i=0; i<TAGBUF_LEN; i++)
  {
      for(unsigned char j=0; j<TAG_LEN; j++)
	   {
	       Tag_buf[i][j] = 0;
	   }
  }    
}
void tagbuf_reset(void)
{
    tagbuf_init();
}

uchar tagbuf_isfull(void)
{
    if( tag_ptr == TAGBUF_LEN-1)
	{
	    tag_buf_full = 1;
	}
	else
	{
	    tag_buf_full = 0;
	}
	return tag_buf_full;
}

uchar tagbuf_additem(uchar *CurTag)
{


    if (tag_buf_full == 1)
    {
	    //full  flush buffer and add one item
		tagbuf_reset();
	}

	//add one item copy CurTag to Tag_buf
    memcpy(*(pTagBuf+tag_ptr), CurTag,TAG_LEN);
	tag_ptr++;
	if(tag_ptr == TAGBUF_LEN-1)
	{
	    tag_buf_full = 1;
	}
	return tag_ptr;

}

/**************************************
* 加密 raw-data: DiskBuf encrypt-data: EncBuf
***************************************/
void encode(uchar * ucpSrcbuf,uchar ucLen)
{
    uchar i;
    memset(EncBuf,0x00,64*sizeof(uchar));

    for( i=0; i <ucLen; i++)
    {
        EncBuf[i] = *(ucpSrcbuf+i)^ENCRIPT_KEY;
	}

}


/****************************************
*
******************************************/
uchar buf[64];
void store2disk(uchar encode)
{
	UINT8	s;
//	printf( "Open\n" );
	s = CH376FileOpen( "/DATA.TXT" );  /* 打开文件,该文件在根目录下 */
	if ( s == USB_INT_SUCCESS ) {  /* 文件存在并且已经被打开,移动文件指针到尾部以便添加数据 */
//		printf( "File size = %ld\n", CH376GetFileSize( ) );  /* 读取当前文件长度 */
//		printf( "Locate tail\n" );
		s = CH376ByteLocate( 0xFFFFFFFF );  /* 移到文件的尾部 */
		mStopIfError( s );
	}
	else if ( s == ERR_MISS_FILE ) {  /* 没有找到文件,必须新建文件 */
//		printf( "Create\n" );
		s = CH376FileCreate( NULL );  /* 新建文件并打开,如果文件已经存在则先删除后再新建,不必再提供文件名,刚才已经提供给CH376FileOpen */
		mStopIfError( s );
	}
	else 
	{
//	    printf("open file failed\n");
	    mStopIfError( s );  /* 打开文件时出错 */
	}
//	printf( "Write data begin\n" );

    if(encode == 0)
	{
	    s = CH376ByteWrite( DiskBuf, strlen(DiskBuf), NULL );  /* 以字节为单位向文件写入数据 */// strlen(DiskBuf) == 22 == DISKBUFLEN-1
	}
	else
	{
	    s = CH376ByteWrite( EncBuf, DISKBUFLEN-1, NULL );  /* 以字节为单位向文件写入数据 */
	}
	printf("raw_data:%s",DiskBuf);
/* 有些U盘可能会要求在写数据后等待一会才能继续操作,所以,如果在某些U盘中发生数据丢失现象,建议在每次写入数据后稍作延时再继续 */
	mStopIfError( s );

//		CH376ByteWrite( buf, 0, NULL );  /* 以字节为单位向文件写入数据,因为是0字节写入,所以只用于更新文件的长度,当阶段性写入数据后,可以用这种办法更新文件长度 */
//	printf( "Write end\n" );
//	strcpy( (char *)buf, "end data\xd\xa" );
//	s = CH376ByteWrite( buf, strlen( (char *)buf ), NULL );  /* 以字节为单位向文件写入数据 */
	mStopIfError( s );
//	printf( "Close\n" );
	s = CH376FileClose( TRUE );  /* 关闭文件,自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度 */
	mStopIfError( s );

}

/*************************************************************
*   YYYY-MM-DD HH:MM data
*    copy time-data ==>DiskBuf
*    2013-07-30 12:00 030
**************************************************************/
void record2buf(void)
{
    uchar temp =0;
	temp = (time_buf[0] >> 4) + '0';
	DiskBuf[0] = temp;	
	temp = (time_buf[0] & 0x0F) + '0';
	DiskBuf[1] = temp;	
	temp = (time_buf[1] >> 4) + '0';
	DiskBuf[2] = temp;	
	temp = (time_buf[1] & 0x0F) + '0';
	DiskBuf[3] = temp;	
	DiskBuf[4] = '-';	
	temp = (time_buf[2] >> 4) + '0';
	DiskBuf[5] = temp;	
	temp = (time_buf[2] & 0x0F) + '0';
	DiskBuf[6] = temp;	
	DiskBuf[7] = '-';
	temp = (time_buf[3] >> 4) + '0';
	DiskBuf[8] = temp;	
	temp = (time_buf[3] & 0x0F) + '0';
	DiskBuf[9] = temp;
	DiskBuf[10] = ' '; // backspace		
	temp = (time_buf[4] >> 4) + '0';
	DiskBuf[11] = temp;	
	temp = (time_buf[4] & 0x0F) + '0';
	DiskBuf[12] = temp;
	DiskBuf[13] = ':';
	temp = (time_buf[5] >> 4) + '0';
	DiskBuf[14] = temp;	
	temp = (time_buf[5] & 0x0F) + '0';
	DiskBuf[15] = temp;
	DiskBuf[16] = ' ';	//backspace	
	DiskBuf[17] = B1;
	DiskBuf[18] = B2;
	DiskBuf[19] = B3;	
	DiskBuf[20] = '\r';;
	DiskBuf[21] = '\n';
	DiskBuf[22] = '\0';								
}

/*****************************************************************************************************************
* 
******************************************************************************************************************/
void keyboardscan() 
{
  uchar statevalue,key_checkin,key_check;
  key_io=0xf0;                    //在IO口由输出方式变为输入方式时要延迟一个周期
  key_io=0xf0;                    //采取写两次的方法延时
  key_checkin=key_ior;            //读取IO口状态，判断是否有键按下

  if(key_checkin!=0xf0)           //IO口值发生变化则表示有键按下
  {

    delay_ms(50);                 //键盘消抖，延时20MS
    key_checkin=key_ior;
    if(key_checkin!=0xf0)
    {
	     key_check=key_ior;
	     switch (key_check)
	     {
	         case 0xe0:stepkey=1;break;
	         case 0xd0:stepkey=2;break;
	     }          
    }
  }
  else
  {
	  statevalue=0xff;                      //无按键时返回值
	  stepkey=statevalue;
	  return ;
  }
}

void Setkey_handler()
{

	//clear event
        CLR_EVT(EV_INT1_KEY); 
        beep_ms(50);

        LCD_clear(); 
        //status 0 1 2; -------0:no not set 1:set reader1 2:set reader2
	if(setkey_cnt == 1)
	{
//		    printf("%d\n",setkey_cnt);
            Display_setting();
	}
	else if(setkey_cnt == 2) //display queue
	{
            Display_Queue();
	}
	else LCD_clear(); 
}
//初始化标准参数
void init_ReaderPara()
{
    //读取EEPROM中存储的打桩标准 如果没有，设置默认打桩标准
	eprom_read(0x10, 0x01, &std_depth1);		//读出记录std_depth1
    delay_ms(10);    
    if(std_depth1 > 99)
	{
	    std_depth1 = 10;    //如果没有存过打桩标准 默认一个打桩标准为10米
		eprom_write(0x10, 0x01, &std_depth1);
        delay_ms(10);   
	}
	eprom_read(0x11, 0x01, &std_depth2);		//读出记录std_depth1
    delay_ms(10);    
    if(std_depth2 > 99)
	{
	    std_depth2 = 20;    //如果没有存过打桩标准 默认一个打桩标准为20米
	    eprom_write(0x11, 0x01, &std_depth2);
        delay_ms(10); 
	}
}


void Display_setting()
{
    LCD_write_str(0,1,"reader");
    eprom_read(0x11, 0x01, &std_depth2);		//读出记录std_depth
    delay_ms(10); 
    data_dec(std_depth2);		
    LCD_write_char(13, 1, '0'+A2);
    LCD_write_char(14, 1, '0'+A3);
    LCD_write_char(15, 1, 'm');
}
//***********************************************************************
//                EEPROM读取函数*/
//            addr：地址；number：长度；p_buff：读出数据存放指针
//***********************************************************************
void eprom_read(unsigned int addr, unsigned char number, unsigned char *p_buff) {

	while(EECR & (1 << EEWE));               //等待前面的写完成

	EEARH = 0x00;                           //写高字节地址
	
	while(number --) {
		EEARL = addr ++;                 	//写地址低字节
		EECR |= (1 << EERE);            	//读允许位置1
		*p_buff++ = EEDR;               	//读出EEDR中的数据
		}
}
//***********************************************************************
//              EEPROM写入函数
//          addr：地址；number：长度；p_buff：写入数据存放指针
//***********************************************************************
void eprom_write(unsigned int addr, unsigned char number, unsigned char *p_buff) {

	EEARH = 0x00;
	
	while(number --) {
		while(EECR & (1 << EEWE));       //等待前面的写完成
		EEARL = addr ++;                 //写地址
		EEDR = *p_buff ++;               //写数据到EEDR
		EECR |= (1 << EEMWE);            //主机写入允许位
		EECR&=~(1<<EEWE);
		EECR |= (1 << EEWE);             //EEWE为1，执行写操作
		}
}


void data_dec(uint temp_d)
{
     uint A2t;
     A1=temp_d/100;                       //分出百，十，和个位
     A2t=temp_d%100;
     A2=A2t/10;
     A3=A2t%10;
}



void parsekey()
{
		if(setkey_cnt !=0)
		{	
		    keyboardscan();

	        if(stepkey!=0xff)                        //如果有按键按下
	        {
		       if(stepkey == 1)
			   { 
				   if(setkey_cnt == 1)
				   {
					   std_depth2++;
					   if(std_depth2 > 99)
					   {
					       std_depth2 =0;
					   } 
				       eprom_write(0x11, 0x01, &std_depth2);		
					}
                   delay_ms(10); 
				   Display_setting();
	            }

		       if(stepkey == 2)
			   { 
				   if(setkey_cnt == 1)
				   {
					   if(std_depth2 > 0)
					   {
					       std_depth2--;
					   } 
				       eprom_write(0x11, 0x01, &std_depth2);		
					}
                   delay_ms(10); 
				   Display_setting();
	            }

		    }	
       }
}



void Receive_OK_handler()
{
	unsigned char temp;

    ds1302_read_time();

	eprom_read(0x11, 0x01, &max_depth); 
	delay_ms(10);
	if(max_depth == 0)
	{
	    LCD_write_str(0, 0, "max_depth = 0");
	    return;
	}
	//max_depth int 2 char 深度三个字符显示 
    data_ascii(max_depth);
    record2buf();
	//加密
    encode(DiskBuf,DISKBUFLEN-1); //encrypt DiskBuf ==> EncBuf

	//存储到SD卡中
    store2disk(1);

	//在LCD上写要显示的数据 T: MMDD-HHMMSS
	LCD_clear();
	LCD_write_char(0, 0, 'T');
	LCD_write_char(1, 0, ':');
	LCD_write_char(2, 0, ' ');

	temp = (time_buf[2] >> 4) + '0';
	LCD_write_char(3, 0, temp);//月
	temp = (time_buf[2] & 0x0F) + '0';
	LCD_write_char(4, 0, temp);	
	temp = (time_buf[3] >> 4) + '0';
	LCD_write_char(5, 0, temp);//日
	temp = (time_buf[3] & 0x0F) + '0';
	LCD_write_char(6, 0, temp);	
	LCD_write_char(7, 0, '-');
	temp = (time_buf[4] >> 4) + '0';
	LCD_write_char(8, 0, temp);//时
	temp = (time_buf[4] & 0x0F) + '0';
	LCD_write_char(9, 0, temp);	
	LCD_write_char(10, 0, ':');
	temp = (time_buf[5] >> 4) + '0';
	LCD_write_char(11, 0, temp);//分
	temp = (time_buf[5] & 0x0F) + '0';
	LCD_write_char(12, 0, temp);
	LCD_write_char(13, 0, ':');		
	temp = (time_buf[6] >> 4) + '0';
	LCD_write_char(14, 0, temp);//秒
	temp = (time_buf[6] & 0x0F) + '0';
	LCD_write_char(15, 0, temp);
	
	//显示深度数据
	LCD_write_str(0, 1,"depth:");
    data_dec(max_depth);
	LCD_write_char(13, 1, '0'+A2);
	LCD_write_char(14, 1, '0'+A3);
	LCD_write_char(15, 1, 'm');

//    printf("max_depth = %d\n",max_depth);
    reader1_depth =0;
    reader2_depth =0;


}

void reader_init()
{

    max_depth = 0;
    uart1_rcv_buf_reset();	
    uart0_rcv_buf_reset();	
    tagbuf_reset();
    memset(DiskBuf,0x00,sizeof(uchar)*DISKBUFLEN);
    memset(EncBuf,0x00,sizeof(uchar)*DISKBUFLEN);

    //使能USART1的接收发送功能  reader2
    UCSR1B = 0x98;                                                          
    //rfid reader begin to read tags  A0 03 65 00 F8
    usart1_buf_send(stRead,CMD_LEN);	
	 
    //使能USART0的接收发送功能  reader1
    UCSR0B = 0x98;         
    //rfid reader begin to read tags  A0 03 65 00 F8
    usart0_buf_send(stRead,CMD_LEN);	

	//系统初始化 如果此时有一条有效深度 上次遗留的 写入U盘
    if( is_one_cnt())
	{
	    Receive_OK_handler();
        queue_reset();
	}                    
//    queue_reset();
}

//
//	eprom_read(0x12, 0x01, &std_depth1);		//读出记录std_depth1
//    delay_ms(10); 

//队列初始化 EEPROM 0x12 0x13 0x14 三个连续地址初始化为0xff
void queue_init()
{        
    unsigned char data =0xff;
	eprom_write(0x12, 0x01, &data);
    delay_ms(10);  

	eprom_write(0x13, 0x01, &data);
    delay_ms(10);  

	eprom_write(0x14, 0x01, &data);
    delay_ms(10);  
}
void queue_reset()
{
    queue_init();
}
//2-1-2这种情况的修正  1-2-0xff
void queue_shift()  
{        
    unsigned char data =0x01;
	eprom_write(0x12, 0x01, &data);
    delay_ms(10);  

    data =0x02;
	eprom_write(0x13, 0x01, &data);
    delay_ms(10); 
	 
    data =0xff;
	eprom_write(0x14, 0x01, &data);
    delay_ms(10);  
}
/*
void add_queue(unsigned char tag)
{
    unsigned char Tmp = 0;
	unsigned char i;
	for(i = 0; i<3; i++)
	{
	    eprom_read(0x12+i, 0x01, &Tmp); 
        delay_ms(10);  
		if(Tmp == 0xFF)
		{
	        eprom_write(0x12+i, 0x01, &tag);
            delay_ms(10);  		    
			return;
		}
    }
}
*/
/*
void add_queue(unsigned char tag)
{
    unsigned char Tmp = 0;
    unsigned char Tmp_pre = 0;
	unsigned char i;
    if(is_empty())
	{
	    eprom_read(0x12, 0x01, &Tmp); 
        delay_ms(10);  
	}
	else
	{
	    for(i = 0; i<2; i++)
	    {
	        eprom_read(0x12+i+1, 0x01, &Tmp); 
            delay_ms(10);  
	        eprom_read(0x12+i, 0x01, &Tmp_pre); 
            delay_ms(10); 
		    if((Tmp == 0xff) && (Tmp_pre!=tag) )
		    {
	            eprom_write(0x12+i+1, 0x01, &tag);
                delay_ms(10);  		    
			    return;
		    }
        }
	}
}
*/
void add_queue(unsigned char tag)
{
    unsigned char Tmp1 = 0;
    unsigned char Tmp2 = 0;
    unsigned char Tmp3 = 0;

    unsigned char Tmp = 0;
	unsigned char i;

    for(i = 0; i<3; i++)
    {
        eprom_read(0x12+i, 0x01, &Tmp); 
        delay_ms(10);  
	    if(Tmp == 0xFF )
	    {
            if(i > 0)
			{
                eprom_read(0x12+i-1, 0x01, &Tmp1); 
                delay_ms(10); 				    
				if(tag == Tmp1)
				    continue;
			}
			eprom_write(0x12+i, 0x01, &tag);
            delay_ms(10);  		    
		return;
	    }
    }

}

unsigned char is_one_cnt()
{
    unsigned char arTmp[3] ={0,0,0};
	eprom_read(0x12, 0x03, &arTmp); 
    delay_ms(10);  
	if(arTmp[0] == 1 && arTmp[1] == 2 && arTmp[2] == 1)
	{
	    return TRUE;
	}
	return FALSE;
}
unsigned char  is_lost_reader1()
{
    unsigned char arTmp[3] ={0,0,0};
	eprom_read(0x12, 0x03, &arTmp); 
    delay_ms(10);  
	if(arTmp[0] == 2 && arTmp[1] == 1 && arTmp[2] == 2)
	{
	    return TRUE;
	}
	return FALSE;
}
unsigned char is_empty()
{
    unsigned char Tmp = 0;
	unsigned char i;
	for(i = 0; i<3; i++)
	{
	    eprom_read(0x12+i, 0x01, &Tmp); 
        delay_ms(10);  
		if(Tmp != 0xff)
		{		    
			return FALSE;
		}
    }
	return TRUE;
}
void Display_Queue()
{
    unsigned char data=0;
	LCD_clear();
    LCD_write_str(0,1,"queue");

	eprom_read(0x12, 0x01, &data);		//读出记录
    delay_ms(10); 
	data_dec(data);		
	LCD_write_char(13, 1, '0'+A3);

	eprom_read(0x13, 0x01, &data);		//读出记录
    delay_ms(10); 
	data_dec(data);	
	LCD_write_char(14, 1, '0'+A3);

	eprom_read(0x14, 0x01, &data);		//读出记录
    delay_ms(10); 
	data_dec(data);	
	LCD_write_char(15, 1, '0'+A3);

}
