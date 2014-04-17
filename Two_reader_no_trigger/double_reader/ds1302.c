#include "comm.h"

//***********************************************************************
//��DS1302д��һ�ֽ�����
//***********************************************************************
void ds1302_write_byte(unsigned char addr, unsigned char d) 
{

	unsigned char i;
	RST_SET;					//����DS1302����
	
	                                                //д��Ŀ���ַ��addr
	IO_OUT;
	addr = addr & 0xFE;                            //���λ����
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO_SET;
			}
		else {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		addr = addr >> 1;
		}
	
	                                              //д�����ݣ�d
	IO_OUT;
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			IO_SET;
			}
		else {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		d = d >> 1;
		}
	RST_CLR;					//ֹͣDS1302����
}
//***********************************************************************
//                   ��DS1302����һ�ֽ�����
//***********************************************************************
unsigned char ds1302_read_byte(unsigned char addr)
 {

	unsigned char i;
	unsigned char temp=0;
	RST_SET;					//����DS1302����

	                                                //д��Ŀ���ַ��addr
	IO_OUT;
	addr = addr | 0x01;                             //���λ�ø�
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO_SET;
			}
		else {
			IO_CLR;
			}
		SCK_SET;
		SCK_CLR;
		addr = addr >> 1;
		}
	
	/*������ݣ�temp*/
	IO_IN;
	for (i = 0; i < 8; i ++) {
		temp = temp >> 1;
		if (IO_R) {
			temp |= 0x80;
			}
		else {
			temp &= 0x7F;
			}
		SCK_SET;
		SCK_CLR;
		}
	
	RST_CLR;					//ֹͣDS1302����
	return temp;
}

//***********************************************************************
//                   ��DS302д��ʱ������
//***********************************************************************

void ds1302_write_time(void)
 {

	ds1302_write_byte(ds1302_control_add,0x00);		//�ر�д���� 
	ds1302_write_byte(ds1302_sec_add,0x80);			//��ͣ 
	//ds1302_write_byte(ds1302_charger_add,0xa9);		//������ 
	ds1302_write_byte(ds1302_year_add,time_buf[1]);		//�� 
	ds1302_write_byte(ds1302_month_add,time_buf[2]);	//�� 
	ds1302_write_byte(ds1302_date_add,time_buf[3]);		//�� 
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//�� 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]);		//ʱ 
	ds1302_write_byte(ds1302_min_add,time_buf[5]);		//��
	ds1302_write_byte(ds1302_sec_add,time_buf[6]);		//��
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//�� 
	ds1302_write_byte(ds1302_control_add,0x80);		//��д���� 
}

//***********************************************************************
//                     ��DS302����ʱ������
//***********************************************************************

void ds1302_read_time(void)  
{ 

	time_buf[1]=ds1302_read_byte(ds1302_year_add);		//�� 
	time_buf[2]=ds1302_read_byte(ds1302_month_add);		//�� 
	time_buf[3]=ds1302_read_byte(ds1302_date_add);		//�� 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add);		//ʱ 
	time_buf[5]=ds1302_read_byte(ds1302_min_add);		//�� 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7F;//�� 
	time_buf[7]=ds1302_read_byte(ds1302_day_add);		//�� 
}

//***********************************************************************
//               DS302��ʼ������
//***********************************************************************

void ds1302_init(void) 
{
	
	RST_CLR;			//RST���õ�
	SCK_CLR;			//SCK���õ�
	RST_OUT;			//RST������Ϊ���
	SCK_OUT;			//SCK������Ϊ���
}