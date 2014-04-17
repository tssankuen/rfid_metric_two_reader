/* 调试 */
/* 提供printf子程序 */
//#include <stdio.h>
//#include <string.h>
//#include <avr/io.h>
//#include <avr/iom128.h>
//#include "comm.h"

//#define uchar           unsigned char
//#define uint            unsigned int
//#define ulong           unsigned long

#include	"debug.h"

/* 检查操作状态,如果错误则显示错误代码并停机,应该替换为实际的处理措施,例如显示错误信息,等待用户确认后重试等 */
void	mStopIfError( UINT8 iError )
{
	if ( iError == USB_INT_SUCCESS ) return;  /* 操作成功 */
//	printf( "Error: %02X\n", (UINT16)iError );  /* 显示错误 */
	while ( 1 ) {
	    beep_ms(200);
	}
}

/* 为printf和getkey输入输出初始化串口 */
void	mInitSTDIO( void )
{
     usart0_init();
}



//*************************************************************************
//	      USART0初始化子程序
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
// 用于输入的标准格式函数，用getchar通过UART1输入到单片机，代替C中的scanf
//***********************************************************************

int uart_getchar(FILE *stream)
{
    while ((UCSR0A & 0x80) == 0);
    return UDR0;
} 


