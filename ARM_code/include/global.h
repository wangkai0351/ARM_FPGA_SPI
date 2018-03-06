#ifndef __global_H__
#define __global_H__

typedef unsigned char   Uint8;
typedef signed char Int8;
typedef unsigned short  Uint16;
typedef signed short    Int16;
typedef unsigned int    Uint32;
typedef signed int  Int32;
typedef unsigned char   OctetString;
typedef char VisualString;

typedef struct  TimeDifference
{
    Uint16  Date;
    Uint32  Millisecond;
} S_TIME_DIFF;

typedef struct  BinaryDate
{
    Uint16  year;
    Uint8   mouth;
    Uint8   day;
    Uint8   hour;
    Uint8   minit;
    Uint16  ms;
} S_BDATE;
#define abb_analysis_err 0x00001010
#define F8X 0xffffffff
#define F4X 0xffff

//typedef unsigned int    cpu_reg;	
//typedef unsigned int    mem_ptr_t;
#define FALSE   0
#define TRUE    1

//数据的NULL
#ifndef NULL
#define NULL    ((void *) 0x0)
#endif

//只有一个参数的函数指针的NULL
#ifndef ODFUCNULL
#define ODFUCNULL   ((int(*) (void *, Uint16)) 0x0)
#endif

//只有一个参数的函数指针的NULL
#ifndef FUCNULL
#define FUCNULL ((void(*) (void *)) 0x0)
#endif

//有5个参数的函数指针的NULL     EPA
#ifndef EPARCVFUCNULL
#define EPARCVFUCNULL   ((void(*) (void *arg, void *upcb, Uint8 *p, Uint8 size, struct epa_sockaddr_in *srcaddr)) 0x0ef00000)
#endif

//有6个参数的函数指针的NULL     UDP
#ifndef UDPRCVFUCNULL
#define UDPRCVFUCNULL   ((void(*) (void *arg, void *pcb, Uint8 *p, Uint16 size, struct ip_addr *addr, Uint16 port)) 0x0ef00000)
#endif

//  void (* recv)(void *arg, void *pcb, Uint8 *p,Uint8 size,
//		struct ip_addr *addr, Uint16 port);
//  void (* recv)(void *arg, void *upcb, Uint8 *p,Uint8 size,
//            struct epa_sockaddr_in * srcaddr);
#define EPA_PORT    0x88bc

extern void gDelay(Uint32 i);

extern int  memcmp(const void * /*s1*/, const void * /*s2*/, Uint32 /*n*/ );
extern void *memcpy(void *s1, const void *s2, Uint32);
extern void *memset(void *, int, Uint32);
extern int  strcmp(const char * /*s1*/, const char * /*s2*/ );
extern char *strcpy(char * /*s1*/, const char * /*s2*/ );
extern Uint32   strlen(const char * /*s*/ );

extern int  rand(void);

/*
    * Computes a sequence of pseudo-random integers in the range 0 to RAND_MAX.
    * Uses an additive generator (Mitchell & Moore) of the form:
    *   Xn = (X[n-24] + X[n-55]) MOD 2^31
    * This is described in section 3.2.2 of Knuth, vol 2. It's period is
    * in excess of 2^55 and its randomness properties, though unproven, are
    * conjectured to be good. Empirical testing since 1958 has shown no flaws.
    * Returns: a pseudo-random integer.
    */
extern void srand(unsigned int /*seed*/ );

/*
    * uses its argument as a seed for a new sequence of pseudo-random numbers
    * to be returned by subsequent calls to rand. If srand is then called with
    * the same seed value, the sequence of pseudo-random numbers is repeated.
    * If rand is called before any calls to srand have been made, the same
    * sequence is generated as when srand is first called with a seed value
    * of 1.
    */
typedef struct
{
    unsigned int    timeStampSeconds;
    int timeStampNanosecs;
} TIMESTAMP;
#endif /* __global_H__ */
