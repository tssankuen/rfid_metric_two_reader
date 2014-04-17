
#include "comm.h"

int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);
FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, uart_getchar,_FDEV_SETUP_RW);



//*************************************************************************
//			ң���������ӳ���
//*************************************************************************


//����ң����������



//*********************************************************************************


void port_init()
{
    Data_IO = 0xFF;				        //��ƽ����
    Data_DDR = 0xFF;				    //�������

    PORTD=0xFF;               		
    DDRD=0xFC;                //PD0 PD1 ����

    PORTB=0XFF;
    DDRB=0XFF;

    PORTF = 0xFF;                       //��ƽ����
    DDRF = 0xFF;                        //�������

    DDRC=0x0f;                        //PC����������ʹ��
    PORTC=0xf0;                       //PC����λ�������λ���

    D_LE1;                             //�ص�����ܣ�������ʾ����                   
    W_LE1;                     
    Data_IO=0xFF;                      //�������            
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
	LCD_init();                           //1602Һ����ʼ��
	ds1302_init();  
	mInitSTDIO( );  /* Ϊ���ü����ͨ�����ڼ����ʾ���� */
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
        temp = (time_buf[0] >> 4) + '0';   //��LCD��дҪ��ʾ������
        LCD_write_char(0, 0, temp);/*��*/
        temp = (time_buf[0] & 0x0F) + '0';
        LCD_write_char(1, 0, temp);
        temp = (time_buf[1] >> 4) + '0';
        LCD_write_char(2, 0, temp);
        temp = (time_buf[1] & 0x0F) + '0';
        LCD_write_char(3, 0, temp);
        LCD_write_char(4, 0, '-');
	
        temp = (time_buf[2] >> 4) + '0';
        LCD_write_char(5, 0, temp);/*��*/
        temp = (time_buf[2] & 0x0F) + '0';
        LCD_write_char(6, 0, temp);
        LCD_write_char(7, 0, '-');
	
        temp = (time_buf[3] >> 4) + '0';
        LCD_write_char(8, 0, temp);/*��*/
        temp = (time_buf[3] & 0x0F) + '0';
        LCD_write_char(9, 0, temp);
	
        temp = (time_buf[4] >> 4) + '0';
        LCD_write_char(8, 1, temp);/*ʱ*/
        temp = (time_buf[4] & 0x0F) + '0';
        LCD_write_char(9, 1, temp);
        LCD_write_char(10, 1, ':');
	
        temp = (time_buf[5] >> 4) + '0';
        LCD_write_char(11, 1, temp);/*��*/
        temp = (time_buf[5] & 0x0F) + '0';
        LCD_write_char(12, 1, temp);
        LCD_write_char(13, 1, ':');
	
       temp = (time_buf[6] >> 4) + '0';
        LCD_write_char(14, 1, temp);/*��*/
        temp = (time_buf[6] & 0x0F) + '0';
        LCD_write_char(15, 1, temp);
    }
	beep_ms(200);	
    delay_ms(200);		
	LCD_clear(); 



    event_table = 0;
	SREG|=0x80;                                //����ȫ���ж�

	s = mInitCH376Host( );  /* ��ʼ��CH376 */
//    printf("s=%d",s);    
	mStopIfError( s );

    //delay 2s
	for(char i=0;i<10;i++)
	{ delay_ms(200);}

    //�ϵ���U���Ƿ����ϵͳ�ϡ�
	LCD_clear();
	LCD_write_str(0,0,"wait-Udisk");


    //Ϊ�������ȶ��� ���ӿ��Ź���·
    wdt_enable(WDTO_1S);//ʹ�ܿ��Ź���ʱ�������ʱ������Ϊ1��
    wdt_reset();        //ι��

	while ( CH376DiskConnect( ) != USB_INT_SUCCESS ) {  /* ���U���Ƿ�����,�ȴ�U�̲���,����SD��,�����ɵ�Ƭ��ֱ�Ӳ�ѯSD�����Ĳ��״̬���� */
			mDelaymS( 100 );
			wdt_reset();//ι��
		}
	mDelaymS( 200 );  /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */

    /* ���ڼ�⵽USB�豸��,���ȴ�10*50mS */
	for ( s = 0; s < 10; s ++ ) {  /* ��ȴ�ʱ��,10*50mS */
		mDelaymS( 50 );
		if ( CH376DiskMount( ) == USB_INT_SUCCESS ) break;  /* ��ʼ�����̲����Դ����Ƿ���� */
		wdt_reset();//ι��
	}

//	printf( "Udisk ON\n" );
	LCD_clear();
	LCD_write_str(0,0,"Udisk ON");
	LCD_clear();
	beep_ms(200);

    reader_init();

	while(1)
	{

        //��ѯ�ж���û�а��� ����Ĭ�ϴ�׮���
        parsekey();
		if (event_table & EV_INT1_KEY)
		{
			Setkey_handler();
			if (event_table == 0)
				continue;
		}
		wdt_reset();//ι��
	}
}
//**********************************************************************************************************************************


