/* ���� */
/* �ṩprintf�ӳ��� */




#ifndef	__DEBUG_H__
#define __DEBUG_H__

#include    <stdarg.h>
#include    <stdlib.h>
#include	"comm.h"
#include	"hal.h"


/* ������״̬,�����������ʾ������벢ͣ��,Ӧ���滻Ϊʵ�ʵĴ����ʩ,������ʾ������Ϣ,�ȴ��û�ȷ�Ϻ����Ե� */
void	mStopIfError( UINT8 iError );

/* Ϊprintf��getkey���������ʼ������ */
void	mInitSTDIO( void );
void    usart0_init(void);
#endif
