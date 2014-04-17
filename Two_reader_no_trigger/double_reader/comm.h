#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>   //中断信号头文件
#include "comdef.h"
#include "debug.h"
#include "hal.h"
#include "file_sys.h"
//***********************************************************************
//  global 全局事件表
//***********************************************************************
extern volatile uint event_table;
//***********************************************************************
//  DS1302    当前时间
//***********************************************************************
extern unsigned char time_buf[8];

//***********************************************************************
//			共阴数码管显示的断码表0～F
//***********************************************************************
extern uchar table[];

extern uint time_1s_count;
extern uint second;
//***********************************************************************
//  display
//***********************************************************************
extern  uchar  A1;
extern  uchar  A2;
extern  uchar  A3;

extern uchar usart0_rx_data;
extern uchar usart1_rx_data;

extern const uchar edRead[CMD_LEN];

extern uchar setkey_cnt;
unsigned char std_depth1;
unsigned char std_depth2;
extern uchar stepkey;

extern uchar reader1_depth;
extern uchar reader2_depth;

extern uchar save_mode;
extern uchar multi_tag_flag;
extern uchar multi_tag_flag_reader1;
extern uchar multi_tag_flag_reader2;
//***********************************************************************
//  函数声明
//***********************************************************************
extern void beep_ms(uchar utime);

extern void ds1302_init(void);
extern void ds1302_read_time(void);
extern void ds1302_write_time(void);



extern void ext_int0_init();
extern void usart1_str_send(char *s);
extern void usart1_char_send(uchar i);
extern void usart1_init();
extern void usart1_buf_send(uchar *pBuf,uchar ucBufLen);
extern void usart0_str_send(char *s);
extern void usart0_char_send(uchar i);
extern void usart0_init();
extern void usart0_buf_send(uchar *pBuf,uchar ucBufLen);
extern void system_init(void);


void data_do(uint temp_d);
void beep_ms(uchar utime);
void LCD_write_com(unsigned char com); 
void LCD_write_data(unsigned char data); 
void LCD_clear(void); 
void LCD_write_str(unsigned char x,unsigned char y,unsigned char *s); 
void LCD_write_char(unsigned char x,unsigned char y,unsigned char data); 
void LCD_init(void); 
void LCD_close(void);
uchar GetCheckSum(uchar* buf, uchar len);
void Start_metric_handler();
void End_metric_handler();
void usart1_recv_handler();
void result_handler();

//***********************************************************************
//  uart1_rcv_buf
//***********************************************************************
void uart1_rcv_buf_init(void);
void uart1_rcv_buf_reset(void);
uchar uart1_rcv_buf_isfull(void);
void uart1_rcv_buf_additem(uchar data);
//***********************************************************************
//  uart1_rcv_buf
//***********************************************************************
void uart0_rcv_buf_init(void);
void uart0_rcv_buf_reset(void);
uchar uart0_rcv_buf_isfull(void);
void uart0_rcv_buf_additem(uchar data);
//***********************************************************************
//  tagbuf
//***********************************************************************
void tagbuf_init(void);
void tagbuf_reset(void);
uchar tagbuf_isfull(void);
uchar tagbuf_additem(uchar *CurTag);

void Receive_OK_handler();
extern void reader_init();

void queue_init();
void queue_reset();
void add_queue(unsigned char tag);
unsigned char is_one_cnt();
unsigned char is_empty();
unsigned char  is_lost_reader1();
void Display_Queue();
#endif
