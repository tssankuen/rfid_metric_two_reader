/* CH376оƬ Ӳ������� V1.0 */
/* �ṩI/O�ӿ��ӳ��� */

#ifndef	__HAL_H__
#define __HAL_H__

#include "usbdef.h"

/* ��ʱָ��΢��ʱ��,���ݵ�Ƭ����Ƶ����,����ȷ */
extern void	mDelayuS(UINT8 us);
/* ��ʱָ������ʱ��,���ݵ�Ƭ����Ƶ����,����ȷ */
extern void mDelaymS(UINT8 ms);

extern void CH376_PORT_INIT(void);  		/* CH376ͨѶ�ӿڳ�ʼ�� */

extern void xEndCH376Cmd( void );			/* ����CH376����,������SPI�ӿڷ�ʽ */

extern void xWriteCH376Cmd( uint8 mCmd);	/* ��CH376д���� */

extern void xWriteCH376Data( uint8 mData );	/* ��CH376д���� */

extern uint8 xReadCH376Data( void );			/* ��CH376������ */

extern uint8 Query376Interrupt( void );		/* ��ѯCH376�ж�(INT#����Ϊ�͵�ƽ) */

extern uint8 mInitCH376Host( void );			/* ��ʼ��CH376 */

#endif
