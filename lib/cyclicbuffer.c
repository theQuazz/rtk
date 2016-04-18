#include "cyclicbuffer.h"
#include "string.h"

int CyclicBufferPush( struct cyclic_buffer *buffer, void *data ) {
  if ( IsCyclicBufferFull( buffer ) ) {
    return -1;
  }

  int insert_at = ( buffer->start + buffer->num_els ) % buffer->size;
  memcpy( buffer->data + insert_at, data, buffer->data_size );
  buffer->num_els += 1;

  return 0;
}

int CyclicBufferPop( struct cyclic_buffer *buffer, void *dest ) {
  if ( IsCyclicBufferEmpty( buffer ) ) {
    return -1;
  }

  memcpy( dest, buffer->data + buffer->start, buffer->data_size );
  buffer->start = ( buffer->start + 1 ) % buffer->size;
  buffer->num_els -= 1;

  return 0;
}

size_t CyclicBufferLen( struct cyclic_buffer *buffer ) {
  return buffer->num_els;
}

bool IsCyclicBufferFull( struct cyclic_buffer *buffer ) {
  return CyclicBufferLen( buffer ) == buffer->size;
}

bool IsCyclicBufferEmpty( struct cyclic_buffer *buffer ) {
  return CyclicBufferLen( buffer ) == 0;
}
