/* CH376оƬ �ļ�ϵͳ�� V1.0 */
/* �ṩ�ļ�ϵͳ�����ӳ���,�ṩ������ */
/* ��ʹ�õ��ӳ������ע�͵�,�Ӷ���Լ��Ƭ���ĳ���ROM�ռ������RAM�ռ� */
/* ������ӳ�����ͨ�������еı������ݲ���,��������϶�,Ϊ�˽�ԼRAM,Ҳ���Բο�CH375�ӳ����ĳ�ͨ��ͬһȫ�ֱ���/���ϽṹCH376_CMD_DATA���� */

/* name ������ָ���ļ���, ���԰�����Ŀ¼��, ��������·���ָ���, �ܳ��Ȳ�����1+8+1+3+1�ֽ� */
/* PathName ������ָȫ·���Ķ��ļ���, ������Ŀ¼�����༶��Ŀ¼��·���ָ������ļ���/Ŀ¼�� */
/* LongName ������ָ���ļ���, ��UNICODEС��˳�����, ������0�ֽڽ���, ʹ�ó��ļ����ӳ�������ȶ���ȫ�ֻ�����GlobalBuf, ���Ȳ�С��64�ֽ�, �����������ӳ����� */

/* ���� NO_DEFAULT_CH376_INT ���ڽ�ֹĬ�ϵ�Wait376Interrupt�ӳ���,��ֹ��,Ӧ�ó���������ж���һ��ͬ���ӳ��� */
/* ���� DEF_INT_TIMEOUT ��������Ĭ�ϵ�Wait376Interrupt�ӳ����еĵȴ��жϵĳ�ʱʱ��/ѭ������ֵ, 0�򲻼�鳬ʱ��һֱ�ȴ� */
/* ���� EN_DIR_CREATE �����ṩ�½��༶��Ŀ¼���ӳ���,Ĭ���ǲ��ṩ */
/* ���� EN_DISK_QUERY �����ṩ����������ѯ��ʣ��ռ��ѯ���ӳ���,Ĭ���ǲ��ṩ */
/* ���� EN_SECTOR_ACCESS �����ṩ������Ϊ��λ��д�ļ����ӳ���,Ĭ���ǲ��ṩ */
/* ���� EN_LONG_NAME �����ṩ֧�ֳ��ļ������ӳ���,Ĭ���ǲ��ṩ */

#include "hal.h"
#include "FILE_SYS.h"

UINT8	CH376ReadBlock( PUINT8 buf )  /* �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ�,���س��� */
{
	UINT8	s, l;
	xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
	s = l = xReadCH376Data( );  /* ���� */
	if ( l ) {
		do {
			*buf = xReadCH376Data( );
			buf ++;
		} while ( -- l );
	}
	xEndCH376Cmd( );
	return( s );
}

UINT8	CH376WriteReqBlock( PUINT8 buf )  /* ���ڲ�ָ��������д����������ݿ�,���س��� */
{
	UINT8	s, l;
	xWriteCH376Cmd( CMD01_WR_REQ_DATA );
	s = l = xReadCH376Data( );  /* ���� */
	if ( l ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- l );
	}
	xEndCH376Cmd( );
	return( s );
}

void	CH376WriteHostBlock( PUINT8 buf, UINT8 len )  /* ��USB�����˵�ķ��ͻ�����д�����ݿ� */
{
	xWriteCH376Cmd( CMD10_WR_HOST_DATA );
	xWriteCH376Data( len );  /* ���� */
	if ( len ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- len );
	}
	xEndCH376Cmd( );
}

void	CH376WriteOfsBlock( PUINT8 buf, UINT8 ofs, UINT8 len )  /* ���ڲ�������ָ��ƫ�Ƶ�ַд�����ݿ� */
{
	xWriteCH376Cmd( CMD20_WR_OFS_DATA );
	xWriteCH376Data( ofs );  /* ƫ�Ƶ�ַ */
	xWriteCH376Data( len );  /* ���� */
	if ( len ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- len );
	}
	xEndCH376Cmd( );
}

void	CH376SetFileName( PUINT8 name )  /* ���ý�Ҫ�������ļ����ļ��� */
{
/*	UINT8	i;*/
	UINT8	c;
	xWriteCH376Cmd( CMD10_SET_FILE_NAME );
/*	for ( i = MAX_FILE_NAME_LEN; i != 0; -- i ) {
		c = *name;
		xWriteCH376Data( c );
		if ( c == 0 ) break;
		name ++;
	}*/
	c = *name;
	xWriteCH376Data( c );
	while ( c ) {
		name ++;
		c = *name;
		if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;  /* ǿ�н��ļ�����ֹ */
		xWriteCH376Data( c );
	}
	xEndCH376Cmd( );
}

UINT32	CH376Read32bitDat( void )  /* ��CH376оƬ��ȡ32λ�����ݲ��������� */
{
	UINT8	c0, c1, c2, c3;
	c0 = xReadCH376Data( );
	c1 = xReadCH376Data( );
	c2 = xReadCH376Data( );
	c3 = xReadCH376Data( );
	xEndCH376Cmd( );
	return( c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24 );
}

UINT8	CH376ReadVar8( UINT8 var )  /* ��CH376оƬ�ڲ���8λ���� */
{
	UINT8	c0;
	xWriteCH376Cmd( CMD11_READ_VAR8 );
	xWriteCH376Data( var );
	c0 = xReadCH376Data( );
	xEndCH376Cmd( );
	return( c0 );
}

void	CH376WriteVar8( UINT8 var, UINT8 dat )  /* дCH376оƬ�ڲ���8λ���� */
{
	xWriteCH376Cmd( CMD20_WRITE_VAR8 );
	xWriteCH376Data( var );
	xWriteCH376Data( dat );
	xEndCH376Cmd( );
}

UINT32	CH376ReadVar32( UINT8 var )  /* ��CH376оƬ�ڲ���32λ���� */
{
	xWriteCH376Cmd( CMD14_READ_VAR32 );
	xWriteCH376Data( var );
	return( CH376Read32bitDat( ) );  /* ��CH376оƬ��ȡ32λ�����ݲ��������� */
}

void	CH376WriteVar32( UINT8 var, UINT32 dat )  /* дCH376оƬ�ڲ���32λ���� */
{
    xWriteCH376Cmd( CMD50_WRITE_VAR32 );
	xWriteCH376Data( var );
	xWriteCH376Data( (UINT8)dat );
	xWriteCH376Data( (UINT8)( (UINT16)dat >> 8 ) );
	xWriteCH376Data( (UINT8)( dat >> 16 ) );
	xWriteCH376Data( (UINT8)( dat >> 24 ) );
	xEndCH376Cmd( );
}

void	CH376EndDirInfo( void )  /* �ڵ���CH376DirInfoRead��ȡFAT_DIR_INFO�ṹ֮��Ӧ��֪ͨCH376���� */
{
	CH376WriteVar8( 0x0D, 0x00 );
}

UINT32	CH376GetFileSize( void )  /* ��ȡ��ǰ�ļ����� */
{
	return( CH376ReadVar32( VAR_FILE_SIZE ) );
}

UINT8	CH376GetDiskStatus( void )  /* ��ȡ���̺��ļ�ϵͳ�Ĺ���״̬ */
{
	return( CH376ReadVar8( VAR_DISK_STATUS ) );
}

UINT8	CH376GetIntStatus( void )  /* ��ȡ�ж�״̬��ȡ���ж����� */
{
	UINT8	s;
	xWriteCH376Cmd( CMD01_GET_STATUS );
	s = xReadCH376Data( );
	xEndCH376Cmd( );
	return( s );
}

#ifndef	NO_DEFAULT_CH376_INT
UINT8	Wait376Interrupt( void )  /* �ȴ�CH376�ж�(INT#�͵�ƽ)�������ж�״̬��, ��ʱ�򷵻�ERR_USB_UNKNOWN */
{
#ifdef	DEF_INT_TIMEOUT
#if		DEF_INT_TIMEOUT < 1
	while ( Query376Interrupt( ) == FALSE );  /* һֱ���ж� */
	return( CH376GetIntStatus( ) );  /* ��⵽�ж� */
#else
	UINT32	i;
	for ( i = 0; i < DEF_INT_TIMEOUT; i ++ ) {  /* ������ֹ��ʱ */
		if ( Query376Interrupt( ) ) return( CH376GetIntStatus( ) );  /* ��⵽�ж� */
/* �ڵȴ�CH376�жϵĹ�����,������Щ��Ҫ��ʱ�������������� */
	}
	return( ERR_USB_UNKNOWN );  /* ��Ӧ�÷�������� */
#endif
#else
	UINT32	i;
	for ( i = 0; i < 5000000; i ++ ) {  /* ������ֹ��ʱ,Ĭ�ϵĳ�ʱʱ��,�뵥Ƭ����Ƶ�й� */
		if ( Query376Interrupt( ) ) return( CH376GetIntStatus( ) );  /* ��⵽�ж� */
/* �ڵȴ�CH376�жϵĹ�����,������Щ��Ҫ��ʱ�������������� */
	}
	return( ERR_USB_UNKNOWN );  /* ��Ӧ�÷�������� */
#endif
}
#endif

UINT8	CH376SendCmdWaitInt( UINT8 mCmd )  /* �����������,�ȴ��ж� */
{
	xWriteCH376Cmd( mCmd );
	xEndCH376Cmd( );
	return( Wait376Interrupt( ) );
}

UINT8	CH376SendCmdDatWaitInt( UINT8 mCmd, UINT8 mDat )  /* �����������һ�ֽ����ݺ�,�ȴ��ж� */
{
	xWriteCH376Cmd( mCmd );
	xWriteCH376Data( mDat );
	xEndCH376Cmd( );
	return( Wait376Interrupt( ) );
}

UINT8	CH376DiskReqSense( void )  /* ���USB�洢������ */
{
	UINT8	s;
	mDelaymS( 5 );
	s = CH376SendCmdWaitInt( CMD0H_DISK_R_SENSE );
	mDelaymS( 5 );
	return( s );
}

UINT8	CH376DiskConnect( void )  /* ���U���Ƿ�����,��֧��SD�� */
{
	if ( Query376Interrupt( ) ) CH376GetIntStatus( );  /* ��⵽�ж� */
	return( CH376SendCmdWaitInt( CMD0H_DISK_CONNECT ) );
}

UINT8	CH376DiskMount( void )  /* ��ʼ�����̲����Դ����Ƿ���� */
{
	return( CH376SendCmdWaitInt( CMD0H_DISK_MOUNT ) );
}

UINT8	CH376FileOpen( PUINT8 name )  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼�´��ļ�����Ŀ¼(�ļ���) */
{
	CH376SetFileName( name );  /* ���ý�Ҫ�������ļ����ļ��� */
	if ( name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2 ) CH376WriteVar32( VAR_CURRENT_CLUST, 0 );
	return( CH376SendCmdWaitInt( CMD0H_FILE_OPEN ) );
}

UINT8	CH376FileCreate( PUINT8 name )  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼���½��ļ�,����ļ��Ѿ�������ô��ɾ�� */
{
	if ( name ) CH376SetFileName( name );  /* ���ý�Ҫ�������ļ����ļ��� */
	return( CH376SendCmdWaitInt( CMD0H_FILE_CREATE ) );
}

UINT8	CH376DirCreate( PUINT8 name )  /* �ڸ�Ŀ¼���½�Ŀ¼(�ļ���)����,���Ŀ¼�Ѿ�������ôֱ�Ӵ� */
{
	CH376SetFileName( name );  /* ���ý�Ҫ�������ļ����ļ��� */
	if ( name[0] == DEF_SEPAR_CHAR1 || name[0] == DEF_SEPAR_CHAR2 ) CH376WriteVar32( VAR_CURRENT_CLUST, 0 );
	return( CH376SendCmdWaitInt( CMD0H_DIR_CREATE ) );
}

UINT8	CH376SeparatePath( PUINT8 path )  /* ��·���з�������һ���ļ�������Ŀ¼(�ļ���)��,�������һ���ļ�������Ŀ¼�����ֽ�ƫ�� */
{
	PUINT8	pName;
	for ( pName = path; *pName != 0; ++ pName );  /* ���ļ����ַ�������λ�� */
	while ( *pName != DEF_SEPAR_CHAR1 && *pName != DEF_SEPAR_CHAR2 && pName != path ) pName --;  /*  ����������һ��·���ָ��� */
	if ( pName != path ) pName ++;  /* �ҵ���·���ָ���,���޸�ָ��Ŀ���ļ������һ���ļ���,����ǰ��Ķ༶Ŀ¼����·���ָ��� */
	return( pName - path );
}

UINT8	CH376FileOpenDir( PUINT8 PathName, UINT8 StopName )  /* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼���ϼ�Ŀ¼,֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
/* StopName ָ�����һ���ļ�������Ŀ¼�� */
{
	UINT8	i, s;
	s = 0;
	i = 1;  /* �����п��ܵĸ�Ŀ¼�� */
	while ( 1 ) {
		while ( PathName[i] != DEF_SEPAR_CHAR1 && PathName[i] != DEF_SEPAR_CHAR2 && PathName[i] != 0 ) ++ i;  /* ������һ��·���ָ�������·�������� */
		if ( PathName[i] ) i ++;  /* �ҵ���·���ָ���,�޸�ָ��Ŀ���ļ������һ���ļ��� */
		else i = 0;  /* ·������ */
		s = CH376FileOpen( &PathName[s] );  /* ���ļ�����Ŀ¼ */
		if ( i && i != StopName ) {  /* ·����δ���� */
			if ( s != ERR_OPEN_DIR ) {  /* ��Ϊ���𼶴�,��δ��·������,����,������ǳɹ�����Ŀ¼,��ô˵�������� */
				if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );  /* �м�·��������Ŀ¼��,������ļ�������� */
				else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );  /* �м�·����ĳ����Ŀ¼û���ҵ�,������Ŀ¼���ƴ��� */
				else return( s );  /* �������� */
			}
			s = i;  /* ����һ��Ŀ¼��ʼ���� */
		}
		else return( s );  /* ·������,USB_INT_SUCCESSΪ�ɹ����ļ�,ERR_OPEN_DIRΪ�ɹ���Ŀ¼(�ļ���),����Ϊ�������� */
	}
}

UINT8	CH376FileOpenPath( PUINT8 PathName )  /* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼(�ļ���),֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
{
	return( CH376FileOpenDir( PathName, 0xFF ) );
}

UINT8	CH376FileCreatePath( PUINT8 PathName )  /* �½��༶Ŀ¼�µ��ļ�,֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
{
	UINT8	s;
	UINT8	Name;
	Name = CH376SeparatePath( PathName );  /* ��·���з�������һ���ļ���,�������һ���ļ�����ƫ�� */
	if ( Name ) {  /* �Ƕ༶Ŀ¼ */
		s = CH376FileOpenDir( PathName, Name );  /* �򿪶༶Ŀ¼�µ����һ��Ŀ¼,�����½��ļ����ϼ�Ŀ¼ */
		if ( s != ERR_OPEN_DIR ) {  /* ��Ϊ�Ǵ��ϼ�Ŀ¼,����,������ǳɹ�����Ŀ¼,��ô˵�������� */
			if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );  /* �м�·��������Ŀ¼��,������ļ�������� */
			else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );  /* �м�·����ĳ����Ŀ¼û���ҵ�,������Ŀ¼���ƴ��� */
			else return( s );  /* �������� */
		}
	}
	return( CH376FileCreate( &PathName[Name] ) );  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼���½��ļ� */
}

#ifdef	EN_DIR_CREATE
UINT8	CH376DirCreatePath( PUINT8 PathName )  /* �½��༶Ŀ¼�µ�Ŀ¼(�ļ���)����,֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
{
	UINT8	s;
	UINT8	Name;
	UINT8	ClustBuf[4];
	Name = CH376SeparatePath( PathName );  /* ��·���з�������һ��Ŀ¼��,�������һ���ļ�����ƫ�� */
	if ( Name ) {  /* ���Ƕ༶Ŀ¼ */
		if ( PathName[0] == DEF_SEPAR_CHAR1 || PathName[0] == DEF_SEPAR_CHAR2 ) return( CH376DirCreate( PathName ) );  /* �ڸ�Ŀ¼���½�Ŀ¼ */
		else return( ERR_MISS_DIR );  /* �����ṩ����·������ʵ���ڵ�ǰĿ¼���½�Ŀ¼ */
	}
	else {  /* �Ƕ༶Ŀ¼ */
		s = CH376FileOpenDir( PathName, Name );  /* �򿪶༶Ŀ¼�µ����һ��Ŀ¼,�����½�Ŀ¼���ϼ�Ŀ¼ */
		if ( s != ERR_OPEN_DIR ) {  /* ��Ϊ�Ǵ��ϼ�Ŀ¼,����,������ǳɹ�����Ŀ¼,��ô˵�������� */
			if ( s == USB_INT_SUCCESS ) return( ERR_FOUND_NAME );  /* �м�·��������Ŀ¼��,������ļ�������� */
			else if ( s == ERR_MISS_FILE ) return( ERR_MISS_DIR );  /* �м�·����ĳ����Ŀ¼û���ҵ�,������Ŀ¼���ƴ��� */
			else return( s );  /* �������� */
		}
		xWriteCH376Cmd( CMD14_READ_VAR32 );
		xWriteCH376Data( VAR_START_CLUSTER );  /* �ϼ�Ŀ¼����ʼ�غ� */
		for ( s = 0; s != 4; s ++ ) ClustBuf[ s ] = xReadCH376Data( );
		xEndCH376Cmd( );
		s = CH376DirCreate( &PathName[Name] );  /* �ڵ�ǰĿ¼���½�Ŀ¼ */
		if ( s != USB_INT_SUCCESS ) return( s );
		s = CH376ByteLocate( sizeof(FAT_DIR_INFO) + STRUCT_OFFSET( FAT_DIR_INFO, DIR_FstClusHI ) );  /* �ƶ��ļ�ָ�� */
		if ( s != USB_INT_SUCCESS ) return( s );
		s = CH376ByteWrite( &ClustBuf[2], 2, NULL );  /* д���ϼ�Ŀ¼����ʼ�غŵĸ�16λ */
		if ( s != USB_INT_SUCCESS ) return( s );
		s = CH376ByteLocate( sizeof(FAT_DIR_INFO) + STRUCT_OFFSET( FAT_DIR_INFO, DIR_FstClusLO ) );  /* �ƶ��ļ�ָ�� */
		if ( s != USB_INT_SUCCESS ) return( s );
		s = CH376ByteWrite( ClustBuf, 2, NULL );  /* д���ϼ�Ŀ¼����ʼ�غŵĵ�16λ */
		if ( s != USB_INT_SUCCESS ) return( s );
		s = CH376ByteLocate( 0 );  /* �ƶ��ļ�ָ��,�ָ���Ŀ¼ͷλ�� */
		if ( s != USB_INT_SUCCESS ) return( s );
	}
}
#endif

UINT8	CH376FileErase( PUINT8 PathName )  /* ɾ���ļ�,����Ѿ�����ֱ��ɾ��,��������ļ����ȴ���ɾ��,֧�ֶ༶Ŀ¼·�� */
{
	UINT8	s;
	if ( PathName ) {  /* �ļ���δ�� */
		for ( s = 1; PathName[s] != DEF_SEPAR_CHAR1 && PathName[s] != DEF_SEPAR_CHAR2 && PathName[s] != 0; ++ s );  /* ������һ��·���ָ�������·�������� */
		if ( PathName[s] ) {  /* ��·���ָ���,�Ƕ༶Ŀ¼�µ��ļ�����Ŀ¼ */
			s = CH376FileOpenPath( PathName );  /* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼ */
			if ( s != USB_INT_SUCCESS || s != ERR_OPEN_DIR ) return( s );  /* �������� */
		}
		else CH376SetFileName( PathName );  /* û��·���ָ���,�Ǹ�Ŀ¼���ߵ�ǰĿ¼�µ��ļ�����Ŀ¼,���ý�Ҫ�������ļ����ļ��� */
	}
	return( CH376SendCmdWaitInt( CMD0H_FILE_ERASE ) );
}

UINT8	CH376FileClose( UINT8 UpdateSz )  /* �رյ�ǰ�Ѿ��򿪵��ļ�����Ŀ¼(�ļ���) */
{
	return( CH376SendCmdDatWaitInt( CMD1H_FILE_CLOSE, UpdateSz ) );
}

UINT8	CH376DirInfoRead( void )  /* ��ȡ��ǰ�ļ���Ŀ¼��Ϣ */
{
	return( CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, 0xFF ) );
}

UINT8	CH376DirInfoSave( void )  /* �����ļ���Ŀ¼��Ϣ */
{
	return( CH376SendCmdWaitInt( CMD0H_DIR_INFO_SAVE ) );
}

UINT8	CH376ByteLocate( UINT32 offset )  /* ���ֽ�Ϊ��λ�ƶ���ǰ�ļ�ָ�� */
{
	xWriteCH376Cmd( CMD4H_BYTE_LOCATE );
	xWriteCH376Data( (UINT8)offset );
	xWriteCH376Data( (UINT8)((UINT16)offset>>8) );
	xWriteCH376Data( (UINT8)(offset>>16) );
	xWriteCH376Data( (UINT8)(offset>>24) );
	xEndCH376Cmd( );
	return( Wait376Interrupt( ) );
}

UINT8	CH376ByteRead( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )  /* ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
{
	UINT8	s;
	xWriteCH376Cmd( CMD2H_BYTE_READ );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );
	xEndCH376Cmd( );
	if ( RealCount ) *RealCount = 0;
	while ( 1 ) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_READ ) {
			s = CH376ReadBlock( buf );  /* �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ�,���س��� */
			xWriteCH376Cmd( CMD0H_BYTE_RD_GO );
			xEndCH376Cmd( );
			buf += s;
			if ( RealCount ) *RealCount += s;
		}
/*		else if ( s == USB_INT_SUCCESS ) return( s );*/  /* ���� */
		else return( s );  /* ���� */
	}
}

UINT8	CH376ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )  /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
{
	UINT8	s;
	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );
	xEndCH376Cmd( );
	if ( RealCount ) *RealCount = 0;
	while ( 1 ) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_WRITE ) {
			s = CH376WriteReqBlock( buf );  /* ���ڲ�ָ��������д����������ݿ�,���س��� */
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
			xEndCH376Cmd( );
			buf += s;
			if ( RealCount ) *RealCount += s;
		}
/*		else if ( s == USB_INT_SUCCESS ) return( s );*/  /* ���� */
		else return( s );  /* ���� */
	}
}

#ifdef	EN_DISK_QUERY

UINT8	CH376DiskCapacity( PUINT32 DiskCap )  /* ��ѯ������������,������ */
{
	UINT8	s;
	s = CH376SendCmdWaitInt( CMD0H_DISK_CAPACITY );
	if ( s == USB_INT_SUCCESS ) {  /* �ο�CH376INC.H�ļ���CH376_CMD_DATA�ṹ��DiskCapacity */
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data( );  /* ��������sizeof(CH376_CMD_DATA.DiskCapacity) */
		*DiskCap = CH376Read32bitDat( );  /* CH376_CMD_DATA.DiskCapacity.mDiskSizeSec,��CH376оƬ��ȡ32λ�����ݲ��������� */
	}
	else *DiskCap = 0;
	return( s );
}

UINT8	CH376DiskQuery( PUINT32 DiskFre )  /* ��ѯ����ʣ��ռ���Ϣ,������ */
{
	UINT8	s;
	UINT8	c0, c1, c2, c3;
	s = CH376SendCmdWaitInt( CMD0H_DISK_QUERY );
	if ( s == USB_INT_SUCCESS ) {  /* �ο�CH376INC.H�ļ���CH376_CMD_DATA�ṹ��DiskQuery */
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data( );  /* ��������sizeof(CH376_CMD_DATA.DiskQuery) */
		xReadCH376Data( );  /* CH376_CMD_DATA.DiskQuery.mTotalSector */
		xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		c0 = xReadCH376Data( );  /* CH376_CMD_DATA.DiskQuery.mFreeSector */
		c1 = xReadCH376Data( );
		c2 = xReadCH376Data( );
		c3 = xReadCH376Data( );
		*DiskFre = c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24;
		xReadCH376Data( );  /* CH376_CMD_DATA.DiskQuery.mDiskFat */
		xEndCH376Cmd( );
	}
	else *DiskFre = 0;
	return( s );
}

#endif

UINT8	CH376SecLocate( UINT32 offset )  /* ������Ϊ��λ�ƶ���ǰ�ļ�ָ�� */
{
	xWriteCH376Cmd( CMD4H_SEC_LOCATE );
	xWriteCH376Data( (UINT8)offset );
	xWriteCH376Data( (UINT8)((UINT16)offset>>8) );
	xWriteCH376Data( (UINT8)(offset>>16) );
	xWriteCH376Data( 0 );  /* ��������ļ��ߴ� */
	xEndCH376Cmd( );
	return( Wait376Interrupt( ) );
}

#ifdef	EN_SECTOR_ACCESS

UINT8	CH376DiskReadSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount )  /* ��U�̶�ȡ������������ݿ鵽������,��֧��SD�� */
/* iLbaStart ��׼����ȡ��������ʼ������, iSectorCount ��׼����ȡ�������� */
{
	UINT8	s, err;
	UINT16	mBlockCount;
	for ( err = 0; err != 3; ++ err ) {  /* �������� */
		xWriteCH376Cmd( CMD5H_DISK_READ );  /* ��USB�洢�������� */
		xWriteCH376Data( (UINT8)iLbaStart );  /* LBA�����8λ */
		xWriteCH376Data( (UINT8)( (UINT16)iLbaStart >> 8 ) );
		xWriteCH376Data( (UINT8)( iLbaStart >> 16 ) );
		xWriteCH376Data( (UINT8)( iLbaStart >> 24 ) );  /* LBA�����8λ */
		xWriteCH376Data( iSectorCount );  /* ������ */
		xEndCH376Cmd( );
		for ( mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN; mBlockCount != 0; -- mBlockCount ) {  /* ���ݿ���� */
			s = Wait376Interrupt( );  /* �ȴ��жϲ���ȡ״̬ */
			if ( s == USB_INT_DISK_READ ) {  /* USB�洢�������ݿ�,�������ݶ��� */
				s = CH376ReadBlock( buf );  /* �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ�,���س��� */
				xWriteCH376Cmd( CMD0H_DISK_RD_GO );  /* ����ִ��USB�洢���Ķ����� */
				xEndCH376Cmd( );
				buf += s;
			}
			else break;  /* ���ش���״̬ */
		}
		if ( mBlockCount == 0 ) {
			s = Wait376Interrupt( );  /* �ȴ��жϲ���ȡ״̬ */
			if ( s == USB_INT_SUCCESS ) return( USB_INT_SUCCESS );  /* �����ɹ� */
		}
		if ( s == USB_INT_DISCONNECT ) return( s );  /* U�̱��Ƴ� */
		CH376DiskReqSense( );  /* ���USB�洢������ */
	}
	return( s );  /* ����ʧ�� */
}

UINT8	CH376DiskWriteSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount )  /* ���������еĶ�����������ݿ�д��U��,��֧��SD�� */
/* iLbaStart ��д�������ʼ��������, iSectorCount ��д��������� */
{
	UINT8	s, err;
	UINT16	mBlockCount;
	for ( err = 0; err != 3; ++ err ) {  /* �������� */
		xWriteCH376Cmd( CMD5H_DISK_WRITE );  /* ��USB�洢��д���� */
		xWriteCH376Data( (UINT8)iLbaStart );  /* LBA�����8λ */
		xWriteCH376Data( (UINT8)( (UINT16)iLbaStart >> 8 ) );
		xWriteCH376Data( (UINT8)( iLbaStart >> 16 ) );
		xWriteCH376Data( (UINT8)( iLbaStart >> 24 ) );  /* LBA�����8λ */
		xWriteCH376Data( iSectorCount );  /* ������ */
		xEndCH376Cmd( );
		for ( mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN; mBlockCount != 0; -- mBlockCount ) {  /* ���ݿ���� */
			s = Wait376Interrupt( );  /* �ȴ��жϲ���ȡ״̬ */
			if ( s == USB_INT_DISK_WRITE ) {  /* USB�洢��д���ݿ�,��������д�� */
				CH376WriteHostBlock( buf, CH376_DAT_BLOCK_LEN );  /* ��USB�����˵�ķ��ͻ�����д�����ݿ� */
				xWriteCH376Cmd( CMD0H_DISK_WR_GO );  /* ����ִ��USB�洢����д���� */
				xEndCH376Cmd( );
				buf += CH376_DAT_BLOCK_LEN;
			}
			else break;  /* ���ش���״̬ */
		}
		if ( mBlockCount == 0 ) {
			s = Wait376Interrupt( );  /* �ȴ��жϲ���ȡ״̬ */
			if ( s == USB_INT_SUCCESS ) return( USB_INT_SUCCESS );  /* �����ɹ� */
		}
		if ( s == USB_INT_DISCONNECT ) return( s );  /* U�̱��Ƴ� */
		CH376DiskReqSense( );  /* ���USB�洢������ */
	}
	return( s );  /* ����ʧ�� */
}

UINT8	CH376SecRead( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount )  /* ������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ�,��֧��SD�� */
{
	UINT8	s;
	UINT8	cnt;
	UINT32	StaSec;
	UINT32	fsz, fofs;
	if ( RealCount ) *RealCount = 0;
	do {
		xWriteCH376Cmd( CMD01_GET_IC_VER );
		cnt = xReadCH376Data( );
		if ( cnt == 0x41 ) {
			xWriteCH376Cmd( CMD14_READ_VAR32 );
			xWriteCH376Data( VAR_FILE_SIZE );
			xReadCH376Data( );
			fsz = xReadCH376Data( );
			fsz |= (UINT16)(xReadCH376Data( )) << 8;
			cnt = xReadCH376Data( );
			fsz |= (UINT32)cnt << 16;
			xWriteCH376Cmd( CMD14_READ_VAR32 );
			xWriteCH376Data( VAR_CURRENT_OFFSET );
			xReadCH376Data( );
			fofs = xReadCH376Data( );
			fofs |= (UINT16)(xReadCH376Data( )) << 8;
			fofs |= (UINT32)(xReadCH376Data( )) << 16;
			if ( fsz >= fofs + 510 ) CH376WriteVar8( VAR_FILE_SIZE + 3, 0xFF );
			else cnt = 0xFF;
		}
		else cnt = 0xFF;
		xWriteCH376Cmd( CMD1H_SEC_READ );
		xWriteCH376Data( ReqCount );
		xEndCH376Cmd( );
		s = Wait376Interrupt( );
		if ( cnt != 0xFF ) CH376WriteVar8( VAR_FILE_SIZE + 3, cnt );
		if ( s != USB_INT_SUCCESS ) return( s );
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data( );  /* ��������sizeof(CH376_CMD_DATA.SectorRead) */
		cnt = xReadCH376Data( );  /* CH376_CMD_DATA.SectorRead.mSectorCount */
		xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		StaSec = CH376Read32bitDat( );  /* CH376_CMD_DATA.SectorRead.mStartSector,��CH376оƬ��ȡ32λ�����ݲ��������� */
		if ( cnt == 0 ) break;
		s = CH376DiskReadSec( buf, StaSec, cnt );  /* ��U�̶�ȡ������������ݿ鵽������ */
		if ( s != USB_INT_SUCCESS ) return( s );
		buf += cnt * DEF_SECTOR_SIZE;
		if ( RealCount ) *RealCount += cnt;
		ReqCount -= cnt;
	} while ( ReqCount );
	return( s );
}

UINT8	CH376SecWrite( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount )  /* ������Ϊ��λ�ڵ�ǰλ��д�����ݿ�,��֧��SD�� */
{
	UINT8	s;
	UINT8	cnt;
	UINT32	StaSec;
	if ( RealCount ) *RealCount = 0;
	do {
		xWriteCH376Cmd( CMD1H_SEC_WRITE );
		xWriteCH376Data( ReqCount );
		xEndCH376Cmd( );
		s = Wait376Interrupt( );
		if ( s != USB_INT_SUCCESS ) return( s );
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		xReadCH376Data( );  /* ��������sizeof(CH376_CMD_DATA.SectorWrite) */
		cnt = xReadCH376Data( );  /* CH376_CMD_DATA.SectorWrite.mSectorCount */
		xReadCH376Data( );
		xReadCH376Data( );
		xReadCH376Data( );
		StaSec = CH376Read32bitDat( );  /* CH376_CMD_DATA.SectorWrite.mStartSector,��CH376оƬ��ȡ32λ�����ݲ��������� */
		if ( cnt == 0 ) break;
		s = CH376DiskWriteSec( buf, StaSec, cnt );  /* ���������еĶ�����������ݿ�д��U�� */
		if ( s != USB_INT_SUCCESS ) return( s );
		buf += cnt * DEF_SECTOR_SIZE;
		if ( RealCount ) *RealCount += cnt;
		ReqCount -= cnt;
	} while ( ReqCount );
	return( s );
}

#endif

#ifdef	EN_LONG_NAME

UINT8	CH376LongNameWrite( PUINT8 buf, UINT16 ReqCount )  /* ���ļ���ר�õ��ֽ�д�ӳ��� */
{
	UINT8	s, c;
	c = CH376ReadVar8( VAR_DISK_STATUS );
	if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, DEF_DISK_OPEN_DIR );
	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	xWriteCH376Data( (UINT8)ReqCount );
	xWriteCH376Data( (UINT8)(ReqCount>>8) );
	xEndCH376Cmd( );
	while ( 1 ) {
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_WRITE ) {
			if ( buf ) buf += CH376WriteReqBlock( buf );  /* ���ڲ�ָ��������д����������ݿ�,���س��� */
			else {
				xWriteCH376Cmd( CMD01_WR_REQ_DATA );  /* ���ڲ�ָ��������д����������ݿ� */
				s = xReadCH376Data( );  /* ���� */
				while ( s -- ) xWriteCH376Data( 0 );  /* ���0 */
			}
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
			xEndCH376Cmd( );
		}
/*		else if ( s == USB_INT_SUCCESS ) return( s );*/  /* ���� */
		else {
			if ( c == DEF_DISK_OPEN_ROOT ) CH376WriteVar8( VAR_DISK_STATUS, c );
			return( s );  /* ���� */
		}
	}
}

UINT8	CH376CheckNameSum( PUINT8 DirName )  /* ���㳤�ļ����Ķ��ļ��������,����Ϊ��С����ָ����Ĺ̶�11�ֽڸ�ʽ */
{
	UINT8	NameLen;
	UINT8	CheckSum;
	CheckSum = 0;
	for ( NameLen = 0; NameLen != 11; NameLen ++ ) CheckSum = ( CheckSum & 1 ? 0x80 : 0x00 ) + ( CheckSum >> 1 ) + *DirName++;
	return( CheckSum );
}

UINT8	CH376LocateInUpDir( PUINT8 PathName )  /* ���ϼ�Ŀ¼(�ļ���)���ƶ��ļ�ָ�뵽��ǰ�ļ�Ŀ¼��Ϣ���ڵ����� */
/* ����,˳�㽫��ǰ�ļ�Ŀ¼��Ϣ���ڵ�������ǰһ��������LBA��ַд��CH376�ڲ�VAR_FAT_DIR_LBA����(Ϊ�˷����ռ����ļ���ʱ��ǰ����,����Ҫ���ƶ�һ��) */
/* ʹ����ȫ�ֻ�����GlobalBuf��ǰ12���ֽ� */
{
	UINT8	s;
	xWriteCH376Cmd( CMD14_READ_VAR32 );
	xWriteCH376Data( VAR_FAT_DIR_LBA );  /* ��ǰ�ļ�Ŀ¼��Ϣ���ڵ�����LBA��ַ */
	for ( s = 4; s != 8; s ++ ) GlobalBuf[ s ] = xReadCH376Data( );  /* ��ʱ������ȫ�ֻ�������,��ԼRAM */
	xEndCH376Cmd( );
	s = CH376SeparatePath( PathName );  /* ��·���з�������һ���ļ�������Ŀ¼��,�������һ���ļ�������Ŀ¼����ƫ�� */
	if ( s ) s = CH376FileOpenDir( PathName, s );  /* �Ƕ༶Ŀ¼,�򿪶༶Ŀ¼�µ����һ��Ŀ¼,�����ļ����ϼ�Ŀ¼ */
	else s = CH376FileOpen( "/" );  /* ��Ŀ¼�µ��ļ�,��򿪸�Ŀ¼ */
	if ( s != ERR_OPEN_DIR ) return( s );
	*(PUINT32)(&GlobalBuf[0]) = 0;  /* Ŀ¼����ƫ��������,������ȫ�ֻ�������,��ԼRAM */
	while ( 1 ) {  /* �����ƶ��ļ�ָ��,ֱ���뵱ǰ�ļ�Ŀ¼��Ϣ���ڵ�����LBA��ַƥ�� */
		s = CH376SecLocate( *(PUINT32)(&GlobalBuf[0]) );  /* ������Ϊ��λ���ϼ�Ŀ¼���ƶ��ļ�ָ�� */
		if ( s != USB_INT_SUCCESS ) return( s );
		CH376ReadBlock( &GlobalBuf[8] );  /* ���ڴ滺������ȡCH376_CMD_DATA.SectorLocate.mSectorLba���ݿ�,���س�������sizeof(CH376_CMD_DATA.SectorLocate) */
		if ( *(PUINT32)(&GlobalBuf[8]) == *(PUINT32)(&GlobalBuf[4]) ) return( USB_INT_SUCCESS );  /* �ѵ���ǰ�ļ�Ŀ¼��Ϣ���� */
		xWriteCH376Cmd( CMD50_WRITE_VAR32 );
		xWriteCH376Data( VAR_FAT_DIR_LBA );  /* �õ�ǰһ������,����Ϊ�µ��ļ�Ŀ¼��Ϣ����LBA��ַ */
		for ( s = 8; s != 12; s ++ ) xWriteCH376Data( GlobalBuf[ s ] );
		xEndCH376Cmd( );
		++ *(PUINT32)(&GlobalBuf[0]);
	}
}

UINT8	CH376GetLongName( PUINT8 PathName, PUINT8 LongName )  /* �ɶ��ļ�������Ŀ¼(�ļ���)�������Ӧ�ĳ��ļ��� */
/* ��Ҫ������ļ���������·��PathName,��Ҫ�ṩ���������ճ��ļ���LongName(��UNICODEС�˱���,��˫0����) */
/* ʹ����ȫ�ֻ�����GlobalBuf��ǰ34���ֽ�,sizeof(GlobalBuf)>=sizeof(FAT_DIR_INFO)+2 */
{
	UINT8	s;
	UINT16	NameCount;	/* ���ļ����ֽڼ��� */
	s = CH376FileOpenPath( PathName );  /* �򿪶༶Ŀ¼�µ��ļ�����Ŀ¼ */
	if ( s != USB_INT_SUCCESS && s != ERR_OPEN_DIR ) return( s );
	s = CH376DirInfoRead( );  /* ��ȡ��ǰ�ļ���Ŀ¼��ϢFAT_DIR_INFO,��������ݵ����ڴ��� */
	if ( s != USB_INT_SUCCESS ) return( s );
	CH376ReadBlock( GlobalBuf );  /* ���ڴ滺������ȡFAT_DIR_INFO���ݿ�,���س�������sizeof(FAT_DIR_INFO) */
	CH376EndDirInfo( );  /* ��ȡ��FAT_DIR_INFO�ṹ */
	GlobalBuf[32] = CH376CheckNameSum( GlobalBuf );  /* ���㳤�ļ����Ķ��ļ��������,������ȫ�ֻ�������,��ԼRAM */
	GlobalBuf[33] = CH376ReadVar8( VAR_FILE_DIR_INDEX );  /* ��ǰ�ļ�Ŀ¼��Ϣ�������ڵ�������,������ȫ�ֻ�������,��ԼRAM */
	NameCount = 0;
	while ( 1 ) {
		if ( GlobalBuf[33] == 0 ) {  /* ��ǰ���ļ�Ŀ¼��Ϣ������������,ת��ǰһ������ */
			s = CH376LocateInUpDir( PathName );  /* ���ϼ�Ŀ¼���ƶ��ļ�ָ�뵽��ǰ�ļ�Ŀ¼��Ϣ���ڵ����� */
			if ( s != USB_INT_SUCCESS ) break;
			if ( CH376ReadVar32( VAR_CURRENT_OFFSET ) == 0 ) {  /* ��ǰ�Ѿ�����Ŀ¼�����Ŀ�ʼ,�޷���ȡ���ļ��� */
				s = ERR_LONG_NAME_ERR;
				break;
			}
			GlobalBuf[33] = DEF_SECTOR_SIZE / sizeof( FAT_DIR_INFO );  /* ָ��ǰһ�����������һ���ļ�Ŀ¼��Ϣ */
		}
		GlobalBuf[33] --;  /* �Ӻ���ǰ�����ļ�Ŀ¼��Ϣ */
		s = CH376SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, GlobalBuf[33] );  /* ��ȡָ����Ŀ¼��ϢFAT_DIR_INFO,��������ݵ����ڴ��� */
		if ( s != USB_INT_SUCCESS ) break;
		CH376ReadBlock( GlobalBuf );  /* ���ڴ滺������ȡFAT_DIR_INFO���ݿ�,���س�������sizeof(FAT_DIR_INFO) */
		CH376EndDirInfo( );  /* ��ȡ��FAT_DIR_INFO�ṹ */
		if ( ( GlobalBuf[11] & ATTR_LONG_NAME_MASK ) != ATTR_LONG_NAME || GlobalBuf[13] != GlobalBuf[32] ) {  /* ���ʹ������У��ʹ��� */
			s = ERR_LONG_NAME_ERR;
			break;  /* û��ֱ�ӷ�������Ϊ����Ǵ��˸�Ŀ¼��ô����Ҫ�رպ���ܷ��� */
		}
		for ( s = 1; s < sizeof( FAT_DIR_INFO ); s += 2 ) {  /* �ռ����ļ���,���ļ������ַ��ڴ�����UNICODE��С�˷�ʽ��� */
			if ( s == 1 + 5 * 2 ) s = 14;  /* �ӳ��ļ����ĵ�һ��1-5���ַ������ڶ���6-11���ַ� */
			else if ( s == 14 + 6 * 2 ) s = 28;  /* �ӳ��ļ����ĵڶ���6-11���ַ�����������12-13���ַ� */
			LongName[ NameCount++ ] = GlobalBuf[ s ];
			LongName[ NameCount++ ] = GlobalBuf[ s + 1 ];
			if ( GlobalBuf[ s ] == 0 && GlobalBuf[ s + 1 ] == 0 ) break;  /* ���ļ������� */
			if ( NameCount >= LONG_NAME_BUF_LEN ) {  /* ���ļ������������ */
				s = ERR_LONG_BUF_OVER;
				goto CH376GetLongNameE;
			}
		}
		if ( GlobalBuf[0] & 0x40 ) {  /* ���ļ���Ŀ¼��Ϣ����� */
			if ( s >= sizeof( FAT_DIR_INFO ) ) *(PUINT16)( &LongName[ NameCount ] ) = 0x0000;  /* ��δ�ռ������ļ����Ľ�����,��ǿ�ƽ��� */
			s = USB_INT_SUCCESS;  /* �ɹ���ɳ��ļ����ռ���� */
			break;
		}
	}
CH376GetLongNameE:
	CH376FileClose( FALSE );  /* ���ڸ�Ŀ¼�����Ҫ�ر� */
	return( s );
}

UINT8	CH376CreateLongName( PUINT8 PathName, PUINT8 LongName )  /* �½����г��ļ������ļ�,�ر��ļ��󷵻�,LongName����·��������RAM�� */
/* ��Ҫ������ļ���������·��PathName(�����Ȳο�FAT�淶�ɳ��ļ������в���),��Ҫ������UNICODEС�˱������˫0�����ĳ��ļ���LongName */
/* ʹ����ȫ�ֻ�����GlobalBuf��ǰ64���ֽ�,sizeof(GlobalBuf)>=sizeof(FAT_DIR_INFO)*2 */
{
	UINT8	s, i;
	UINT8	DirBlockCnt;	/* ���ļ���ռ���ļ�Ŀ¼�ṹ�ĸ��� */
	UINT16	count;			/* ��ʱ����,���ڼ���,�����ֽڶ��ļ���ʽ��ʵ�ʶ�ȡ���ֽ��� */
	UINT16	NameCount;		/* ���ļ����ֽڼ��� */
	UINT32	NewFileLoc;		/* ��ǰ�ļ�Ŀ¼��Ϣ���ϼ�Ŀ¼�е���ʼλ��,ƫ�Ƶ�ַ */
	for ( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) if ( *(PUINT16)(&LongName[count]) == 0 ) break;  /* ������λ�� */
	if ( count == 0 || count >= LONG_NAME_BUF_LEN || count > LONE_NAME_MAX_CHAR ) return( ERR_LONG_NAME_ERR );  /* ���ļ�����Ч */
	DirBlockCnt = count / LONG_NAME_PER_DIR;  /* ���ļ���ռ���ļ�Ŀ¼�ṹ�ĸ��� */
	i = count - DirBlockCnt * LONG_NAME_PER_DIR;
	if ( i ) {  /* ����ͷ */
		if ( ++ DirBlockCnt * LONG_NAME_PER_DIR > LONG_NAME_BUF_LEN ) return( ERR_LONG_BUF_OVER );  /* ��������� */
		count += 2;  /* ����0��������ĳ��� */
		i += 2;
		if ( i < LONG_NAME_PER_DIR ) {  /* ��ĩ���ļ�Ŀ¼�ṹ���� */
			while ( i++ < LONG_NAME_PER_DIR ) LongName[count++] = 0xFF;  /* ��ʣ��������Ϊ0xFF */
		}
	}
	s = CH376FileOpenPath( PathName );  /* �򿪶༶Ŀ¼�µ��ļ� */
	if ( s == USB_INT_SUCCESS ) {   /* ���ļ��������򷵻ش��� */
		s = ERR_NAME_EXIST;
		goto CH376CreateLongNameE;
	}
	if ( s != ERR_MISS_FILE ) return( s );
	s = CH376FileCreatePath( PathName );  /* �½��༶Ŀ¼�µ��ļ� */
	if ( s != USB_INT_SUCCESS ) return( s );
	i = CH376ReadVar8( VAR_FILE_DIR_INDEX );  /* ��ʱ���ڱ��浱ǰ�ļ�Ŀ¼��Ϣ�������ڵ������� */
	s = CH376LocateInUpDir( PathName );  /* ���ϼ�Ŀ¼���ƶ��ļ�ָ�뵽��ǰ�ļ�Ŀ¼��Ϣ���ڵ����� */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;  /* û��ֱ�ӷ�������Ϊ����Ǵ��˸�Ŀ¼��ô����Ҫ�رպ���ܷ��� */
	NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET ) + i * sizeof(FAT_DIR_INFO);  /* ���㵱ǰ�ļ�Ŀ¼��Ϣ���ϼ�Ŀ¼�е���ʼλ��,ƫ�Ƶ�ַ */
	s = CH376ByteLocate( NewFileLoc );  /* ���ϼ�Ŀ¼���ƶ��ļ�ָ�뵽��ǰ�ļ�Ŀ¼��Ϣ��λ�� */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	s = CH376ByteRead( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO), NULL );  /* ���ֽ�Ϊ��λ��ȡ����,��õ�ǰ�ļ���Ŀ¼��ϢFAT_DIR_INFO */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	for ( i = DirBlockCnt; i != 0; -- i ) {  /* �������е��ļ�Ŀ¼�ṹ���ڴ�ų��ļ��� */
		s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count );  /* ���ֽ�Ϊ��λ��ȡ����,�����һ���ļ�Ŀ¼��ϢFAT_DIR_INFO */
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
		if ( count == 0 ) break;  /* �޷���������,�ϼ�Ŀ¼������ */
		if ( GlobalBuf[0] && GlobalBuf[0] != 0xE5 ) {  /* ����������ʹ�õ��ļ�Ŀ¼�ṹ,���ڳ��ļ����������Ӵ��,���Կռ䲻��,���������ǰλ�ò����ת�� */
			s = CH376ByteLocate( NewFileLoc );  /* ���ϼ�Ŀ¼���ƶ��ļ�ָ�뵽��ǰ�ļ�Ŀ¼��Ϣ��λ�� */
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			GlobalBuf[ 0 ] = 0xE5;  /* �ļ�ɾ����־ */
			for ( s = 1; s != sizeof(FAT_DIR_INFO); s ++ ) GlobalBuf[ s ] = GlobalBuf[ sizeof(FAT_DIR_INFO) + s ];
			s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );  /* д��һ���ļ�Ŀ¼�ṹ,����ɾ��֮ǰ�½����ļ�,ʵ�����Ժ�Ὣ֮ת�Ƶ�Ŀ¼����ĩλ�� */
			if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			do {  /* ����������е��ļ�Ŀ¼�ṹ */
				s = CH376ByteRead( GlobalBuf, sizeof(FAT_DIR_INFO), &count );  /* ���ֽ�Ϊ��λ��ȡ����,�����һ���ļ�Ŀ¼��ϢFAT_DIR_INFO */
				if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
			} while ( count && GlobalBuf[0] );  /* �����Ȼ������ʹ�õ��ļ�Ŀ¼�ṹ������������,ֱ���ϼ�Ŀ¼������������δʹ�ù����ļ�Ŀ¼�ṹ */
			NewFileLoc = CH376ReadVar32( VAR_CURRENT_OFFSET );  /* ���ϼ�Ŀ¼�ĵ�ǰ�ļ�ָ����Ϊ��ǰ�ļ�Ŀ¼��Ϣ���ϼ�Ŀ¼�е���ʼλ�� */
			i = DirBlockCnt + 1;  /* ��Ҫ�Ŀ��е��ļ�Ŀ¼�ṹ�ĸ���,�������ļ�������һ���ͳ��ļ��� */
			if ( count == 0 ) break;  /* �޷���������,�ϼ�Ŀ¼������ */
			NewFileLoc -= sizeof(FAT_DIR_INFO);  /* ���ص��ղ��������Ŀ��е��ļ�Ŀ¼�ṹ����ʼλ�� */
		}
	}
	if ( i ) {  /* ���е��ļ�Ŀ¼�ṹ�����Դ�ų��ļ���,ԭ�����ϼ�Ŀ¼������,���������ϼ�Ŀ¼�ĳ��� */
		s = CH376ReadVar8( VAR_SEC_PER_CLUS );  /* ÿ�������� */
		if ( s == 128 ) {  /* FAT12/FAT16�ĸ�Ŀ¼,�����ǹ̶���,�޷������ļ�Ŀ¼�ṹ */
			s = ERR_FDT_OVER;  /* FAT12/FAT16��Ŀ¼�µ��ļ���Ӧ������512��,��Ҫ�������� */
			goto CH376CreateLongNameE;
		}
		count = s * DEF_SECTOR_SIZE;  /* ÿ���ֽ��� */
		if ( count < i * sizeof(FAT_DIR_INFO) ) count <<= 1;  /* һ�ز���������һ��,�������ֻ�ᷢ����ÿ��Ϊ512�ֽڵ������ */
		s = CH376LongNameWrite( NULL, count );  /* ���ֽ�Ϊ��λ��ǰλ��д��ȫ0���ݿ�,��������ӵ��ļ�Ŀ¼�� */
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376ByteLocate( NewFileLoc );  /* ���ϼ�Ŀ¼���ƶ��ļ�ָ�뵽��ǰ�ļ�Ŀ¼��Ϣ��λ�� */
	if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	GlobalBuf[11] = ATTR_LONG_NAME;
	GlobalBuf[12] = 0x00;
	GlobalBuf[13] = CH376CheckNameSum( &GlobalBuf[ sizeof(FAT_DIR_INFO) ] );  /* ���㳤�ļ����Ķ��ļ�������� */
	GlobalBuf[26] = 0x00;
	GlobalBuf[27] = 0x00;
	for ( s = 0; DirBlockCnt != 0; ) {  /* ���ļ���ռ�õ��ļ�Ŀ¼�ṹ���� */
		GlobalBuf[0] = s ? DirBlockCnt : DirBlockCnt | 0x40;  /* �״�Ҫ�ó��ļ�����ڱ�־ */
		DirBlockCnt --;
		NameCount = DirBlockCnt * LONG_NAME_PER_DIR;
		for ( s = 1; s < sizeof( FAT_DIR_INFO ); s += 2 ) {  /* ������ļ���,���ļ������ַ��ڴ�����UNICODE��С�˷�ʽ��� */
			if ( s == 1 + 5 * 2 ) s = 14;  /* �ӳ��ļ����ĵ�һ��1-5���ַ������ڶ���6-11���ַ� */
			else if ( s == 14 + 6 * 2 ) s = 28;  /* �ӳ��ļ����ĵڶ���6-11���ַ�����������12-13���ַ� */
			GlobalBuf[ s ] = LongName[ NameCount++ ];
			GlobalBuf[ s + 1 ] = LongName[ NameCount++ ];
		}
		s = CH376LongNameWrite( GlobalBuf, sizeof(FAT_DIR_INFO) );  /* ���ֽ�Ϊ��λд��һ���ļ�Ŀ¼�ṹ,���ļ��� */
		if ( s != USB_INT_SUCCESS ) goto CH376CreateLongNameE;
	}
	s = CH376LongNameWrite( &GlobalBuf[ sizeof(FAT_DIR_INFO) ], sizeof(FAT_DIR_INFO) );  /* ���ֽ�Ϊ��λд��һ���ļ�Ŀ¼�ṹ,����ת������֮ǰ�½����ļ���Ŀ¼��Ϣ */
CH376CreateLongNameE:
	CH376FileClose( FALSE );  /* ���ڸ�Ŀ¼�����Ҫ�ر� */
	return( s );
}

#endif