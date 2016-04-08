#ifndef __ASM_H__
#define __ASM_H__

void activate( uint32_t spsr, uint32_t sp, uint32_t code );
void syscall( void );

#endif
