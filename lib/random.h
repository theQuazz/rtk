#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>

void Seed( uint32_t s );
uint32_t GetSeed( void );
uint32_t Rand( void ); /* [0, UINT_MAX] */
uint32_t RandMax( uint32_t max ); /* [0, max] */
uint32_t RandRange( uint32_t min, uint32_t max ); /* [min, max] */
int RandErrno( void );

#endif
