#ifndef _COMDEF_H_
#define _COMDEF_H_

#define uchar           unsigned char
#define uint            unsigned int
#define ulong           unsigned long

#define	FCPU	7372800		// ��Ƭ����ƵΪ7.3728MHz,������ʱ�ӳ��� 
#define f_count         184              	//��ʱ����ֵ����ʱ����һ����
#define timer_clk       0x07				//ʱ��1024��Ƶ

#define delay_us(x)     _delay_us(x)    	//AVR GCC��ʱ���� x(us)
#define delay_ms(x)     _delay_ms(x)    	//AVR GCC��ʱ���� x(ms)

#define Data_IO	        PORTA                //��������ݿ�
#define Data_DDR        DDRA                 //��������ݿڷ���Ĵ���
#define D_LE0	        PORTD &= ~(1 << PD4) //����ܶο���λΪ0������˿�����
#define D_LE1           PORTD |= (1 << PD4)  //����ܶο���λΪ1�������������˿�һ��
#define W_LE0	        PORTD &= ~(1 << PD5) //�����λ����λΪ0
#define W_LE1           PORTD |= (1 << PD5)  //�����λ����λΪ1

#define baud           9600                                     //���ò����ʵĴ�С
#define baud_setting   (uint)((ulong)FCPU/(16*(ulong)baud)-1)  //�����ʼ��㹫ʽ
#define baud_h         (uchar)(baud_setting>>8)                 //��ȡ��λ
#define baud_l         (uchar)(baud_setting)                    //��λ


//***********************************************************************
//  sound   beep
//***********************************************************************
#define beep0             PORTD &= ~(1 << PD6)    //����������IO��PD6
#define beep1             PORTD |= (1 << PD6)
#define LED8              PORTB                  //LED���ƿ�PB

#define delay_nms(X)        _delay_ms(X)     
 
#define SoundON             PORTD &= ~(1 << PD6)    		//����������IO��PD6
#define SoundOFF            PORTD |= (1 << PD6)
/*******************************************************************************
* DS1302���
********************************************************************************/
/*��λ��*/
#define RST_CLR	PORTE &= ~(1 << PE7)              //��ƽ�õ�
#define RST_SET	PORTE |= (1 << PE7)               //��ƽ�ø�
#define RST_IN	DDRE &= ~(1 << PE7)               //��������
#define RST_OUT	DDRE |= (1 << PE7)                //�������

/*˫������*/
#define IO_CLR	PORTE &= ~(1 << PE6)              //��ƽ�õ�
#define IO_SET	PORTE |= (1 << PE6)               //��ƽ�ø�
#define IO_R	PINE & (1 << PE6)                 //��ƽ��ȡ
#define IO_IN	DDRE &= ~(1 << PE6)               //��������
#define IO_OUT	DDRE |= (1 << PE6)                //�������

/*ʱ���ź�*/
#define SCK_CLR	PORTE &= ~(1 << PE5)              //ʱ���ź�
#define SCK_SET	PORTE |= (1 << PE5)               //��ƽ�ø�
#define SCK_IN	DDRE &= ~(1 << PE5)               //��������
#define SCK_OUT	DDRE |= (1 << PE5)                //�������

/*ds1302��������ص�ַ*/
#define ds1302_sec_add			0x80		//�����ݵ�ַ
#define ds1302_min_add			0x82		//�����ݵ�ַ
#define ds1302_hr_add			0x84		//ʱ���ݵ�ַ
#define ds1302_date_add			0x86		//�����ݵ�ַ
#define ds1302_month_add		0x88		//�����ݵ�ַ
#define ds1302_day_add			0x8a		//�������ݵ�ַ
#define ds1302_year_add			0x8c		//�����ݵ�ַ
#define ds1302_control_add		0x8e		//�������ݵ�ַ
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe

#define RS_CLR	PORTF &= ~(1 << PF1)       //RS�õ�
#define RS_SET	PORTF |= (1 << PF1)        //RS�ø�

#define RW_CLR	PORTF &= ~(1 << PF2)       //RW�õ�
#define RW_SET	PORTF |= (1 << PF2)        //RW�ø�

#define EN_CLR	PORTF &= ~(1 << PF3)       //E�õ�
#define EN_SET	PORTF |= (1 << PF3)        //E�ø�


#define LIGHT_OFF	PORTE &= ~(1 << PE3)       //PE3�õ�
#define LIGHT_ON	PORTE |= (1 << PE3)        //PE3�ø�
/*******************************************************************************
                      event_table ȫ���¼�״̬��־
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
                      ��Ȳ��� ���������
********************************************************************************/

/*******************************************************************************
                     USART1���ջ���������
********************************************************************************/
#define UARTRCVBUFLEN	        17

#define CMD_LEN                  5
#define PARA_LEN                 8


#define TAGBUF_LEN              30              //����30����ǩ
#define TAG_LEN                 12              //tag 
#define INTEVAL                  3              //ÿ����ǩ����ļ������ m



#define ENCRIPT_KEY             0x5A            //�ӽ�����Կ
#define DISKBUFLEN                23             // YYYY-MM-DD HH:MM DATA \r\n\0



#define key_io          PORTC               //����������PC4��PC5����
#define key_ior         PINC                //��ȡIO��ֵΪPINC��ֵ

#define KEY_UP     0x01
#define KEY_DOWN   0x09
#define KEY_SET    0x05
#define KEY_LEFT   0x04
#define KEY_RIGHT  0x06
#define KEY_QUIT   0x03
#endif
