/* CH376芯片 硬件抽象层 V1.0 */
/* 提供I/O接口子程序 */

#ifndef	__HAL_H__
#define __HAL_H__

#include "usbdef.h"

/* 延时指定微秒时间,根据单片机主频调整,不精确 */
extern void	mDelayuS(UINT8 us);
/* 延时指定毫秒时间,根据单片机主频调整,不精确 */
extern void mDelaymS(UINT8 ms);

extern void CH376_PORT_INIT(void);  		/* CH376通讯接口初始化 */

extern void xEndCH376Cmd( void );			/* 结束CH376命令,仅用于SPI接口方式 */

extern void xWriteCH376Cmd( uint8 mCmd);	/* 向CH376写命令 */

extern void xWriteCH376Data( uint8 mData );	/* 向CH376写数据 */

extern uint8 xReadCH376Data( void );			/* 从CH376读数据 */

extern uint8 Query376Interrupt( void );		/* 查询CH376中断(INT#引脚为低电平) */

extern uint8 mInitCH376Host( void );			/* 初始化CH376 */

#endif
