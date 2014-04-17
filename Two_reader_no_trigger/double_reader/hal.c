//**********************************************************************
//              ���ͷ�ļ�
//**********************************************************************
#include "comm.h"
#include "hal.h"
#include "debug.h"

//#define SPI_USEINT  
#define	CH376_SPI_SDO
volatile uint8 spiTransferComplete;  //������ɱ�־

/***********************************************************************
�������ܣ�SPI�����жϵ��жϺ���
�������ݣ�������ɱ�־��1
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
�������ܣ�SPI��ʼ��
�������ݣ�1�����ö˿������������
		  2������SPI�ӿ���ؼĴ���
          3��֧�ֳ���AVR��Ƭ���ӿڣ���������ǿ������չ
***********************************************************************/
/* ����ʹ��SPI��дʱ��,���Խ��г�ʼ�� */
void CH376_PORT_INIT(void)
{
   

	PORTB |= (1 << PB1);  //����SCKΪ1
    DDRB |= (1<<DDB1);    //����SCKΪ���
	DDRB &= ~(1<<DDB3);   //����MISOΪ����
	DDRB |= (1<<DDB2);    //����MOSIΪ���
	DDRB |= (1<<DDB0);    //����ģʽ�£�����SSΪ��������Կ���SPI�����Ƭѡ�ź�
    PORTB |= (1 << PB0);  //����SS�ź�

    SPCR = (1<<MSTR)| (1<<SPR1) | (1<<SPE);  //SPI��������ģʽ������MSB��ǰ��16��Ƶ
//    SPCR = (1<<SPIE);  //SPI�ж�����
	spiTransferComplete = 1;


}
void	mDelay0_5uS( void )  /* ������ʱ0.5uS,���ݵ�Ƭ����Ƶ���� */
{
	delay_us(1);
}


/***********************************************************************
�������ܣ�SPI�ֽڴ��亯��
�������ݣ����ݴ�SPDR�������߶���
***********************************************************************/

uint8 Spi376Exchange( uint8 d )  /* Ӳ��SPI���������8��λ���� */
{
#ifdef SPI_USEINT                    //��������ж�ģʽ
	while(!spiTransferComplete);
	SPDR=d;                      //���������SPDR
	spiTransferComplete = 0;
#else
    SPDR=d;                      //���������SPDR
	while(!(SPSR & (1<<SPIF)));     //�ȴ��ϴδ������
#endif
    return SPDR;                     //����SPDRֵ
	
}

void xWriteCH376Cmd( uint8 mCmd)/* ��CH376д���� */
{

	PORTB |= (1 << PB0);  /* ��ֹ֮ǰδͨ��xEndCH376Cmd��ֹSPIƬѡ */
/* ����˫��I/O����ģ��SPI�ӿ�,��ô����ȷ���Ѿ�����SPI_SCS,SPI_SCK,SPI_SDIΪ�������,SPI_SDOΪ���뷽�� */
	PORTB &= ~(1 << PB0);  /* SPIƬѡ��Ч */
	Spi376Exchange( mCmd );  /* ���������� */

	mDelay0_5uS( ); 
	mDelay0_5uS( ); 
	mDelay0_5uS( );  /* ��ʱ1.5uSȷ����д���ڴ���1.5uS,����������һ�е�״̬��ѯ���� */

}
void xWriteCH376Data( uint8 mData )  /* ��CH376д���� */
{
	Spi376Exchange( mData );

}
uint8 xReadCH376Data( void )  /* ��CH376������ */
{
	mDelay0_5uS( );  /* ȷ����д���ڴ���0.6uS */
	return( Spi376Exchange( 0xFF ) );
}

/* SPIƬѡ��Ч,����CH376����,������SPI�ӿڷ�ʽ */
void xEndCH376Cmd(void) 
{ 
    PORTB |= (1 << PB0);
}  



/* ��ѯCH376�ж�(INT#�͵�ƽ) */
uint8 Query376Interrupt( void )
{
#ifdef	CH376_INT_WIRE
	return( CH376_INT_WIRE ? FALSE : TRUE );  /* ���������CH376���ж�������ֱ�Ӳ�ѯ�ж����� */
#else
	return( (PINB &0x08) ? FALSE : TRUE );  /* ���δ����CH376���ж��������ѯ�����ж������SDO����״̬ */
#endif
}



uint8 mInitCH376Host( void )  /* ��ʼ��CH376 */
{
	UINT8	res;



	CH376_PORT_INIT( );  /* �ӿ�Ӳ����ʼ�� */



	xWriteCH376Cmd( CMD11_CHECK_EXIST );  /* ���Ե�Ƭ����CH376֮���ͨѶ�ӿ� */


	xWriteCH376Data( 0x65 );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
	if ( res != 0x9A ) return( ERR_USB_UNKNOWN );  /* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),���ڲ�����,һֱ�ڸ�λ,���񲻹��� */
	xWriteCH376Cmd( CMD11_SET_USB_MODE );  /* �豸USB����ģʽ */
	xWriteCH376Data( 0x06 );
	mDelayuS( 20 );
	res = xReadCH376Data( );
	xEndCH376Cmd( );
#ifndef	CH376_INT_WIRE
#ifdef	CH376_SPI_SDO
	xWriteCH376Cmd( CMD20_SET_SDO_INT );  /* ����SPI��SDO���ŵ��жϷ�ʽ */
	xWriteCH376Data( 0x16 );
	xWriteCH376Data( 0x90 );  /* SDO������SCSƬѡ��Чʱ�����ж�������� */
	xEndCH376Cmd( );
#endif
#endif
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else return( ERR_USB_UNKNOWN );  /* ����ģʽ���� */
}


/* ��ʱָ��΢��ʱ��,���ݵ�Ƭ����Ƶ����,����ȷ */
void mDelayuS( UINT8 us )
{
	delay_us(us);
}

/* ��ʱָ������ʱ��,���ݵ�Ƭ����Ƶ����,����ȷ */
void mDelaymS( UINT8 ms )
{
	delay_ms(ms);    
}
