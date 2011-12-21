#ifndef PORT_H
#define PORT_H

#include "Types.h"

// if a >= 0 return x else y
static __inline int isel( int a, int x, int y )
{
	int mask = (a >> 31); // arithmetic shift right, splat out the sign bit
	// mask is 0xFFFFFFFF if (a < 0) and 0x00 otherwise.
	return (x & (~mask)) + (y & mask);
}

#ifndef LSB_FIRST
#if defined(__GNUC__) && defined(__ppc__)
#define READ16LE( base )        ({unsigned ppc_lhbrx_; asm( "lhbrx %0,0,%1" : "=r" (ppc_lhbrx_) : "r" (base), "0" (ppc_lhbrx_) ); ppc_lhbrx_;})
#define READ32LE( base )        ({unsigned ppc_lwbrx_; asm( "lwbrx %0,0,%1" : "=r" (ppc_lwbrx_) : "r" (base), "0" (ppc_lwbrx_) ); ppc_lwbrx_;})
#define WRITE16LE( base, value )    ({asm( "sthbrx %0,0,%1" : : "r" (value), "r" (base) );})
#define WRITE32LE( base, value)    ({asm( "stwbrx %0,0,%1" : : "r" (value), "r" (base) );})
#else
#define READ16LE(x) (*((u16 *)(x))<<8)|(*((u16 *)(x))>>8);
#define READ32LE(x) (*((u32 *)(x))<<24)|((*((u32 *)(x))<<8)&0xff0000)|((((*((u32 *)(x))x>>8)&0xff00)|(*((u32 *)(x))>>24);
#define WRITE16LE(x,v) *((u16 *)x) = (*((u16 *)(v))<<8)|(*((u16 *)(v))>>8);
#define WRITE32LE(x,v) *((u32 *)x) = (v<<24)|((v<<8)&0xff0000)|((v>>8)&0xff00)|(v>>24);
#endif
#else
#define READ16LE(x) *((u16 *)x)
#define READ32LE(x) *((u32 *)x)
#define WRITE16LE(x,v) *((u16 *)x) = (v)
#define WRITE32LE(x,v) *((u32 *)x) = (v)
#endif

#endif // PORT_H