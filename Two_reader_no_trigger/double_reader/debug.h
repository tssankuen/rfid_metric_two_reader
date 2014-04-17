/* 调试 */
/* 提供printf子程序 */




#ifndef	__DEBUG_H__
#define __DEBUG_H__

#include    <stdarg.h>
#include    <stdlib.h>
#include	"comm.h"
#include	"hal.h"


/* 检查操作状态,如果错误则显示错误代码并停机,应该替换为实际的处理措施,例如显示错误信息,等待用户确认后重试等 */
void	mStopIfError( UINT8 iError );

/* 为printf和getkey输入输出初始化串口 */
void	mInitSTDIO( void );
void    usart0_init(void);
#endif
