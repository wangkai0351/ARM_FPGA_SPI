#ifndef GR_Basic_Header_H
#define GR_Basic_Header_H



#ifdef __cplusplus
extern "C" {
#endif


//********************函数定义*******************//
//异或
#define XOR(x,y) ((x)==(y)? 0:1)
#define Uint8toUint16(x,y) (((uint16)(y)<<8)+(uint16)(x));
#define Uint8toUint32(a,b,c,d) (((uint32)(d)<<24)+((uint32)(c)<<16)+((uint32)(b)<<8)+(uint32)(a));
#define Uint16toUint32(x,y) (((uint32)(y)<<16)+(uint32)(x));

//********************变量定义*******************//
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

typedef signed char     int8;
typedef unsigned char   uint8;

typedef signed short    int16;
typedef unsigned short  uint16;

typedef signed long     int32;
typedef unsigned long   uint32;

//typedef unsigned char   OctetString;
//typedef char            VisualString;

#endif
