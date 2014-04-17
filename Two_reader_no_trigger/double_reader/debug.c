/* ���� */
/* �ṩprintf�ӳ��� */
//#include <stdio.h>
//#include <string.h>
//#include <avr/io.h>
//#include <avr/iom128.h>
//#include "comm.h"

//#define uchar           unsigned char
//#define uint            unsigned int
//#define ulong           unsigned long

#include	"debug.h"

/* ������״̬,�����������ʾ������벢ͣ��,Ӧ���滻Ϊʵ�ʵĴ����ʩ,������ʾ������Ϣ,�ȴ��û�ȷ�Ϻ����Ե� */
void	mStopIfError( UINT8 iError )
{
	if ( iError == USB_INT_SUCCESS ) return;  /* �����ɹ� */
//	printf( "Error: %02X\n", (UINT16)iError );  /* ��ʾ���� */
	while ( 1 ) {
	    beep_ms(200);
	}
}

/* Ϊprintf��getkey���������ʼ������ */
void	mInitSTDIO( void )
{
     usart0_init();
}



//*************************************************************************
//	      USART0��ʼ���ӳ���
//*************************************************************************
int uart_putchar(char c, FILE *stream)
{
    if ( c == '\n')
        uart_putchar('\r', stream);
    while ( !(UCSR0A & (1<<UDRE0)) )   // UDRE, data register empty
        ;
    UDR0 = c;
    return 0;
}
//***********************************************************************
// ��������ı�׼��ʽ��������getcharͨ��UART1���뵽��Ƭ��������C�е�scanf
//***********************************************************************

int uart_getchar(FILE *stream)
{
    while ((UCSR0A & 0x80) == 0);
    return UDR0;
} 


