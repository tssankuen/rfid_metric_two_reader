//**********************************************************************
//              相关头文件
//**********************************************************************
#include "comm.h"
#include "hal.h"
#include "debug.h"

//#define SPI_USEINT  
#define	CH376_SPI_SDO
volatile uint8 spiTransferComplete;  //传输完成标志

/***********************************************************************
函数功能：SPI采用中断的中断函数
操作内容：传输完成标志置1
***********************************************************************/


#ifdef SPI_USEINT
SIGNAL(SIG_SPI)
{
	spiTransferComplete = 1;

	LED8 =~0x40;
    delay_ms(200);
	LED8 |=0x40;
    delay_ms(200);



}
#endif

/***********************************************************************
函数功能：SPI初始化
操作内容：1、设置端口输入输出属性
		  2、设置SPI接口相关寄存器
          3、支持常用AVR单片机接口，兼容性增强，可扩展
***********************************************************************/
/* 由于使用SPI读写时序,所以进行初始化 */
void CH376_PORT_INIT(void)
{
   

	PORTB |= (1 << PB1);  //设置SCK为1
    DDRB |= (1<<DDB1);    //设置SCK为输出
	DDRB &= ~(1<<DDB3);   //设置MISO为输入
	DDRB |= (1<<DDB2);    //设置MOSI为输出
	DDRB |= (1<<DDB0);    //主机模式下，设置SS为输出，可以控制SPI外设的片选信号
    PORTB |= (1 << PB0);  //拉高SS信号

    SPCR = (1<<MSTR)| (1<<SPR1) | (1<<SPE);  //SPI允许，主机模式，数据MSB在前，16分频
//    SPCR = (1<<SPIE);  //SPI中断允许
	spiTransferComplete = 1;


}
void	mDelay0_5uS( void )  /* 至少延时0.5uS,根据单片机主频调整 */
{
	delay_us(1);
}


/***********************************************************************
函数功能：SPI字节传输函数
操作内容：数据从SPDR读出或者读入
***********************************************************************/

uint8 Spi376Exchange( uint8 d )  /* 硬件SPI输出且输入8个位数据 */
{
#ifdef SPI_USEINT                    //如果启用中断模式
	while(!spiTransferComplete);
	SPDR=d;                      //数据输出到SPDR
	spiTransferComplete = 0;
#else
    SPDR=d;                      //数据输出到SPDR
	while(!(SPSR & (1<<SPIF)));     //等待上次传输完成
#endif
    return SPDR;                     //返回SPDR值
	
}

void xWriteCH376Cmd( uint8 mCmd)/* 向CH376写命令 */
{

	PORTB |= (1 << PB0);  /* 防止之前未通过xEndCH376Cmd禁止SPI片选 */
/* 对于双向I/O引脚模拟SPI接口,那么必须确保已经设置SPI_SCS,SPI_SCK,SPI_SDI为输出方向,SPI_SDO为输入方向 */
	PORTB &= ~(1 << PB0);  /* SPI片选有效 */
	Spi376Exchange( mCmd );  /* 发出命令码 */

	mDelay0_5uS( ); 
	mDelay0_5uS( ); 
	mDelay0_5uS( );  /* 延时1.5uS确保读写周期大于1.5uS,或者用上面一行的状态查询代替 */

}
void xWriteCH376Data( uint8 mData )  /* 向CH376写数据 */
{
	Spi376Exchange( mData );

}
uint8 xReadCH376Data( void )  /* 从CH376读数据 */
{
	mDelay0_5uS( );  /* 确保读写周期大于0.6uS */
	return( Spi376Exchange( 0xFF ) );
}

/* SPI片选无效,结束CH376命令,仅用于SPI接口方式 */
void xEndCH376Cmd(void) 
{ 
    PORTB |= (1 << PB0);
}  



/* 查询CH376中断(INT#低电平) */
uint8 Query376Interrupt( void )
{
#ifdef	CH376_INT_WIRE
	return( CH376_INT_WIRE ? FALSE : TRUE );  /* 如果连接了CH376的中断引脚则直接查询中断引脚 */
#else
	return( (PINB &0x08) ? FALSE : TRUE );  /* 如果未连接CH376的中断引脚则查询兼做中断输出的SDO引脚状态 */
#endif
}



uint8 mInitCH376Host( void )  /* 初始化CH376 */
{
	UINT8	res;



	CH376_PORT_INIT( );  /* 接口硬件初始化 */



	xWriteCH376Cmd( CMD11_CHECK_EXIST );  /* 测试单片机与CH376之间的通讯接口 */


	xWriteCH376Data( 0x65 );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
	if ( res != 0x9A ) return( ERR_USB_UNKNOWN );  /* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),串口波特率,一直在复位,晶振不工作 */
	xWriteCH376Cmd( CMD11_SET_USB_MODE );  /* 设备USB工作模式 */
	xWriteCH376Data( 0x06 );
	mDelayuS( 20 );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
#ifndef	CH376_INT_WIRE
#ifdef	CH376_SPI_SDO
	xWriteCH376Cmd( CMD20_SET_SDO_INT );  /* 设置SPI的SDO引脚的中断方式 */
	xWriteCH376Data( 0x16 );
	xWriteCH376Data( 0x90 );  /* SDO引脚在SCS片选无效时兼做中断请求输出 */
	xEndCH376Cmd( );
#endif
#endif
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else return( ERR_USB_UNKNOWN );  /* 设置模式错误 */
}


/* 延时指定微秒时间,根据单片机主频调整,不精确 */
void mDelayuS( UINT8 us )
{
	delay_us(us);
}

/* 延时指定毫秒时间,根据单片机主频调整,不精确 */
void mDelaymS( UINT8 ms )
{
	delay_ms(ms);    
}
