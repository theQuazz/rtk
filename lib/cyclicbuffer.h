#ifndef __CYCLICBUFFER_H__
#define __CYCLICBUFFER_H__

#include <stdbool.h>
#include <stddef.h>

struct cyclic_buffer {
  const size_t size;
  const size_t data_size;
  void *data;
  int start;
  int num_els;
};

int CyclicBufferPush( struct cyclic_buffer *buffer, void *data );

int CyclicBufferPop( struct cyclic_buffer *buffer, void *dest );

size_t CyclicBufferLen( struct cyclic_buffer *buffer );

bool IsCyclicBufferFull( struct cyclic_buffer *buffer );

bool IsCyclicBufferEmpty( struct cyclic_buffer *buffer );

#endif
