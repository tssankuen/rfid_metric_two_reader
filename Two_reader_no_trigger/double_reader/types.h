#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef		TRUE
#define		TRUE	1

#define		FALSE	0
#endif
#ifndef		NULL
#define		NULL	0
#endif

#ifndef uint8
typedef unsigned char                uint8;
#endif
#ifndef uint16
typedef unsigned short               uint16;
#endif
#ifndef uint32
typedef unsigned long                uint32;
#endif

#ifndef UINT8
typedef unsigned char                UINT8;
#endif
#ifndef UINT16
typedef unsigned short               UINT16;
#endif
#ifndef UINT32
typedef unsigned long                UINT32;
#endif
#ifndef PUINT8
typedef unsigned char               *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short              *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long               *PUINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile       UINT8V;
#endif
#ifndef PUINT8V
typedef unsigned char volatile      *PUINT8V;
#endif

#endif
