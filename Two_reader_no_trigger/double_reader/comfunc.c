#include "comm.h"

/*************************************************************************
*     global event table
*************************************************************************/
volatile uint event_table = 0;

//*************************************************************************
//    global max_depth
//    ÿ�βɼ�ʱ����������Ч�����ȡ��������60�� <65535 ��Int ���ֽڴ洢����
//*************************************************************************/
uchar max_depth = 0;
uint cur_depth = 0;
/*************************************************************************
*     global variable
*************************************************************************/
uchar table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

//uchar  Title[]={"��-��-�� ʱ����  ���(m)"};//��ȡϵͳ 
//���ܹ���ı������ \r\n
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
//  DS1302    ��ǰʱ��
//***********************************************************************
uchar time_buf[8] = {0x20,0x10,0x01,0x14,0x09,0x55,0x00,0x02};
uint time_1s_count;
uint second;
uchar tmp = 0;
//***********************************************************************
//  display
//***********************************************************************

//***********************************************************************
//			�����������ʾ�Ķ����0��F
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
//  RFID reader ���������
//***********************************************************************

const uchar stRead[CMD_LEN] = {0xA0,0x03,0x65,0x00,0xF8};
const uchar edRead[CMD_LEN] = {0xA0,0x03,0xA8,0x00,0xB5};

const uchar setPara[PARA_LEN] = {0xA0,0x06,0x60,0x00,0x00,0x70,0x02,0x88};
const uchar stReadRsp[6] = {0xE4,0x04,0x65,0x00,0x00,0xB3}; //RESET READER  RSP OK 
const uchar edReadRsp[6] = {0xE0,0x04,0xA8,0X00,0x00,0x74};  //STOP READ RSP OK

//***********************************************************************
//  �ڵ繦�� ��һ��ʱ��û�������շ� ��û�д�׮�¼� ��LCD �ر� �ڵ�
//***********************************************************************
uchar save_mode = 0;
uchar multi_tag_flag = 0;
uchar multi_tag_flag_reader1 = 0;
uchar multi_tag_flag_reader2 = 0;
 void data_do(uint temp_d)
 {

     uint A2t,tp1;
     tp1=temp_d/100;                       //�ֳ��٣�ʮ���͸�λ
     A2t=temp_d%100;

     A1=tp1/10;
	 A2=tp1%10;
     A3=A2t/10;
     A4=A2t%10;

}
 void data_ascii(uchar dep)
 {
     uchar B2t;
     B1=dep/100;                       //�ֳ��٣�ʮ���͸�λ
     B2t=dep%100;
     B2=B2t/10;
     B3=B2t%10;

	 B1+='0';   //ת��ASCII�ַ�
	 B2+='0';  
	 B3+='0';
}
/*************************************************************************
*     beep
*************************************************************************/
void beep_ms(uchar utime)
{
	beep0;             //��������
	delay_ms(utime);    //������
	beep1;            //�ط����� 
	delay_ms(utime);    //
}

/*************************************************************************
*     display
*************************************************************************/

//***********************************************************************
//	��ʾ������д�뺯��
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
//	��ʾ������д�뺯��
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
//	��ʾ�������ʾ
//***********************************************************************

void LCD_clear(void) 
{
	LCD_write_com(0x01);
	delay_ms(5);
}
//***********************************************************************
//	��ʾ���ַ���д�뺯��
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
//	��ʾ�����ַ�д�뺯��
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
//	��ʾ����ʼ������
//***********************************************************************

void LCD_init(void) 
{

	LCD_write_com(0x38);				//��ʾģʽ����
	delay_ms(5);
	LCD_write_com(0x08);				//��ʾ�ر�
	delay_ms(5);
    LCD_write_com(0x01);				//��ʾ����
	delay_ms(5);
    LCD_write_com(0x06);				//��ʾ����ƶ�����
	delay_ms(5);
	LCD_write_com(0x0C);				//��ʾ�����������
    delay_ms(5);
}

void LCD_close(void)
{
	LCD_write_com(0x08);				//��ʾ�ر�
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
	    //not full ��Ӧ���ݻ�û��ȫ������--��û�н�����һ֡����
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

	    //���ݺϷ���Ч 	
	    LED8=~0x80;

        //ָʾ����һ�� ��ʾ������� ֻҪreader2������ǩ����  ����һ��
        LIGHT_OFF;
	    delay_ms(200);
        LIGHT_ON;
	    delay_ms(200);

        //�Ķ���2���յ�һ����Ч��ǩ �趨reader2�����Ч ��
        reader2_depth =  1;
        uart1_rcv_buf_reset();	
        //multi_tag_flag �洢��һ�����ĸ�reader������ multi_tag_flag =0 ��ʼֵ multi_tag_flag =1 reader1���� multi_tag_flag=2 reader2����
        if( multi_tag_flag != 2 )
        {
            LCD_write_str(0,1,"get depth");
            add_queue(2);    //1:reader1 2:reader

            if( is_lost_reader1())  //queue : 2-1-2 ������� ���п���reader1 û���� ��ʧһ�Ρ�
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
//	                USART1 ���ջ�������غ���
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
	    //not full ��Ӧ���ݻ�û��ȫ������--��û�н�����һ֡����
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
	    //������һ֡����
//	   if( uart0_rcv_buf[0]!=0x00 || uart0_rcv_buf[15] == GetCheckSum(uart0_rcv_buf[0],15))
//	   {
//			uart0_rcv_buf_reset();	
//			LED8=~0x40;
//	   }
//	   else
//	   {
			//���ݺϷ���Ч 	
			LED8=~0x80;



            //�Ķ���1���յ�һ����Ч��ǩ �趨reader1�����Ч ��
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
//	                USART0 ���ջ�������غ���
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
* ���� raw-data: DiskBuf encrypt-data: EncBuf
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
	s = CH376FileOpen( "/DATA.TXT" );  /* ���ļ�,���ļ��ڸ�Ŀ¼�� */
	if ( s == USB_INT_SUCCESS ) {  /* �ļ����ڲ����Ѿ�����,�ƶ��ļ�ָ�뵽β���Ա�������� */
//		printf( "File size = %ld\n", CH376GetFileSize( ) );  /* ��ȡ��ǰ�ļ����� */
//		printf( "Locate tail\n" );
		s = CH376ByteLocate( 0xFFFFFFFF );  /* �Ƶ��ļ���β�� */
		mStopIfError( s );
	}
	else if ( s == ERR_MISS_FILE ) {  /* û���ҵ��ļ�,�����½��ļ� */
//		printf( "Create\n" );
		s = CH376FileCreate( NULL );  /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½�,�������ṩ�ļ���,�ղ��Ѿ��ṩ��CH376FileOpen */
		mStopIfError( s );
	}
	else 
	{
//	    printf("open file failed\n");
	    mStopIfError( s );  /* ���ļ�ʱ���� */
	}
//	printf( "Write data begin\n" );

    if(encode == 0)
	{
	    s = CH376ByteWrite( DiskBuf, strlen(DiskBuf), NULL );  /* ���ֽ�Ϊ��λ���ļ�д������ */// strlen(DiskBuf) == 22 == DISKBUFLEN-1
	}
	else
	{
	    s = CH376ByteWrite( EncBuf, DISKBUFLEN-1, NULL );  /* ���ֽ�Ϊ��λ���ļ�д������ */
	}
	printf("raw_data:%s",DiskBuf);
/* ��ЩU�̿��ܻ�Ҫ����д���ݺ�ȴ�һ����ܼ�������,����,�����ĳЩU���з������ݶ�ʧ����,������ÿ��д�����ݺ�������ʱ�ټ��� */
	mStopIfError( s );

//		CH376ByteWrite( buf, 0, NULL );  /* ���ֽ�Ϊ��λ���ļ�д������,��Ϊ��0�ֽ�д��,����ֻ���ڸ����ļ��ĳ���,���׶���д�����ݺ�,���������ְ취�����ļ����� */
//	printf( "Write end\n" );
//	strcpy( (char *)buf, "end data\xd\xa" );
//	s = CH376ByteWrite( buf, strlen( (char *)buf ), NULL );  /* ���ֽ�Ϊ��λ���ļ�д������ */
	mStopIfError( s );
//	printf( "Close\n" );
	s = CH376FileClose( TRUE );  /* �ر��ļ�,�Զ������ļ�����,���ֽ�Ϊ��λд�ļ�,�����ó����ر��ļ��Ա��Զ������ļ����� */
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
  key_io=0xf0;                    //��IO���������ʽ��Ϊ���뷽ʽʱҪ�ӳ�һ������
  key_io=0xf0;                    //��ȡд���εķ�����ʱ
  key_checkin=key_ior;            //��ȡIO��״̬���ж��Ƿ��м�����

  if(key_checkin!=0xf0)           //IO��ֵ�����仯���ʾ�м�����
  {

    delay_ms(50);                 //������������ʱ20MS
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
	  statevalue=0xff;                      //�ް���ʱ����ֵ
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
//��ʼ����׼����
void init_ReaderPara()
{
    //��ȡEEPROM�д洢�Ĵ�׮��׼ ���û�У�����Ĭ�ϴ�׮��׼
	eprom_read(0x10, 0x01, &std_depth1);		//������¼std_depth1
    delay_ms(10);    
    if(std_depth1 > 99)
	{
	    std_depth1 = 10;    //���û�д����׮��׼ Ĭ��һ����׮��׼Ϊ10��
		eprom_write(0x10, 0x01, &std_depth1);
        delay_ms(10);   
	}
	eprom_read(0x11, 0x01, &std_depth2);		//������¼std_depth1
    delay_ms(10);    
    if(std_depth2 > 99)
	{
	    std_depth2 = 20;    //���û�д����׮��׼ Ĭ��һ����׮��׼Ϊ20��
	    eprom_write(0x11, 0x01, &std_depth2);
        delay_ms(10); 
	}
}


void Display_setting()
{
    LCD_write_str(0,1,"reader");
    eprom_read(0x11, 0x01, &std_depth2);		//������¼std_depth
    delay_ms(10); 
    data_dec(std_depth2);		
    LCD_write_char(13, 1, '0'+A2);
    LCD_write_char(14, 1, '0'+A3);
    LCD_write_char(15, 1, 'm');
}
//***********************************************************************
//                EEPROM��ȡ����*/
//            addr����ַ��number�����ȣ�p_buff���������ݴ��ָ��
//***********************************************************************
void eprom_read(unsigned int addr, unsigned char number, unsigned char *p_buff) {

	while(EECR & (1 << EEWE));               //�ȴ�ǰ���д���

	EEARH = 0x00;                           //д���ֽڵ�ַ
	
	while(number --) {
		EEARL = addr ++;                 	//д��ַ���ֽ�
		EECR |= (1 << EERE);            	//������λ��1
		*p_buff++ = EEDR;               	//����EEDR�е�����
		}
}
//***********************************************************************
//              EEPROMд�뺯��
//          addr����ַ��number�����ȣ�p_buff��д�����ݴ��ָ��
//***********************************************************************
void eprom_write(unsigned int addr, unsigned char number, unsigned char *p_buff) {

	EEARH = 0x00;
	
	while(number --) {
		while(EECR & (1 << EEWE));       //�ȴ�ǰ���д���
		EEARL = addr ++;                 //д��ַ
		EEDR = *p_buff ++;               //д���ݵ�EEDR
		EECR |= (1 << EEMWE);            //����д������λ
		EECR&=~(1<<EEWE);
		EECR |= (1 << EEWE);             //EEWEΪ1��ִ��д����
		}
}


void data_dec(uint temp_d)
{
     uint A2t;
     A1=temp_d/100;                       //�ֳ��٣�ʮ���͸�λ
     A2t=temp_d%100;
     A2=A2t/10;
     A3=A2t%10;
}



void parsekey()
{
		if(setkey_cnt !=0)
		{	
		    keyboardscan();

	        if(stepkey!=0xff)                        //����а�������
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
	//max_depth int 2 char ��������ַ���ʾ 
    data_ascii(max_depth);
    record2buf();
	//����
    encode(DiskBuf,DISKBUFLEN-1); //encrypt DiskBuf ==> EncBuf

	//�洢��SD����
    store2disk(1);

	//��LCD��дҪ��ʾ������ T: MMDD-HHMMSS
	LCD_clear();
	LCD_write_char(0, 0, 'T');
	LCD_write_char(1, 0, ':');
	LCD_write_char(2, 0, ' ');

	temp = (time_buf[2] >> 4) + '0';
	LCD_write_char(3, 0, temp);//��
	temp = (time_buf[2] & 0x0F) + '0';
	LCD_write_char(4, 0, temp);	
	temp = (time_buf[3] >> 4) + '0';
	LCD_write_char(5, 0, temp);//��
	temp = (time_buf[3] & 0x0F) + '0';
	LCD_write_char(6, 0, temp);	
	LCD_write_char(7, 0, '-');
	temp = (time_buf[4] >> 4) + '0';
	LCD_write_char(8, 0, temp);//ʱ
	temp = (time_buf[4] & 0x0F) + '0';
	LCD_write_char(9, 0, temp);	
	LCD_write_char(10, 0, ':');
	temp = (time_buf[5] >> 4) + '0';
	LCD_write_char(11, 0, temp);//��
	temp = (time_buf[5] & 0x0F) + '0';
	LCD_write_char(12, 0, temp);
	LCD_write_char(13, 0, ':');		
	temp = (time_buf[6] >> 4) + '0';
	LCD_write_char(14, 0, temp);//��
	temp = (time_buf[6] & 0x0F) + '0';
	LCD_write_char(15, 0, temp);
	
	//��ʾ�������
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

    //ʹ��USART1�Ľ��շ��͹���  reader2
    UCSR1B = 0x98;                                                          
    //rfid reader begin to read tags  A0 03 65 00 F8
    usart1_buf_send(stRead,CMD_LEN);	
	 
    //ʹ��USART0�Ľ��շ��͹���  reader1
    UCSR0B = 0x98;         
    //rfid reader begin to read tags  A0 03 65 00 F8
    usart0_buf_send(stRead,CMD_LEN);	

	//ϵͳ��ʼ�� �����ʱ��һ����Ч��� �ϴ������� д��U��
    if( is_one_cnt())
	{
	    Receive_OK_handler();
        queue_reset();
	}                    
//    queue_reset();
}

//
//	eprom_read(0x12, 0x01, &std_depth1);		//������¼std_depth1
//    delay_ms(10); 

//���г�ʼ�� EEPROM 0x12 0x13 0x14 ����������ַ��ʼ��Ϊ0xff
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
//2-1-2�������������  1-2-0xff
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

	eprom_read(0x12, 0x01, &data);		//������¼
    delay_ms(10); 
	data_dec(data);		
	LCD_write_char(13, 1, '0'+A3);

	eprom_read(0x13, 0x01, &data);		//������¼
    delay_ms(10); 
	data_dec(data);	
	LCD_write_char(14, 1, '0'+A3);

	eprom_read(0x14, 0x01, &data);		//������¼
    delay_ms(10); 
	data_dec(data);	
	LCD_write_char(15, 1, '0'+A3);

}
