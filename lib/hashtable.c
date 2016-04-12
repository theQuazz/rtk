#include "hashtable.h"
#include "debug.h"

// https://en.wikipedia.org/wiki/Hopscotch_hashing

enum {
  HT_SENTINEL = 0,
};

enum {
  HT_NEIGHBOURHOOD_SIZE = 16,
};

void HashTableInitialize( struct HashTable *ht ) {
  Debugln( "Initializing HashTable (%u)", ht );

  for ( int i = 0; i < HT_SIZE; i++ ) {
    ht->nodes[i].key = HT_SENTINEL;
  }
  ht->num_elements = 0;
}

unsigned long H( unsigned long k ) {
  return k % HT_SIZE;
}

enum HashTableInsertReturnStatus
HashTableInsert( struct HashTable *ht, unsigned long key, const int data ) {
  Debugln( "HashTableInsert( %u, %u, %d )", ht, key, data );

  const int bucket = H( key );
  int insert_at = -1;

  Debugln( "H( %u ) => %d", key, bucket );

  for ( int i = 0; i < HT_SIZE; i++ ) {
    const int index = ( i + bucket ) % HT_SIZE;

    Debugln( "index => %d", index );

    if ( ht->nodes[index].key != HT_SENTINEL ) {
      continue;
    }

    if ( i < HT_NEIGHBOURHOOD_SIZE ) {
      insert_at = index;
      break;
    }
    else {
      for ( int j = 0; j < HT_NEIGHBOURHOOD_SIZE; j++ ) {
        const int y = index + j % HT_SIZE;
        const int y_hash = H( ht->nodes[y].key );
        const int y_range = y_hash + HT_NEIGHBOURHOOD_SIZE;

        if ( y_hash <= index && index < y_range % HT_SIZE ) {
          insert_at = index;
          break;
        }
      }
      if ( insert_at != -1 ) {
        break;
      }
    }
  }

  if ( insert_at == -1 ) {
    return HT_INSERT_FAILED;
  }
  else {
    ht->nodes[bucket].key = key;
    ht->nodes[bucket].data = data;
    ht->num_elements++;

    return HT_INSERT_SUCCESS;
  }
}

enum HashTableGetReturnStaus
HashTableGet( struct HashTable *ht, unsigned long key, int *data ) {
  Debugln( "HashTableGet( %u, %u, %u )", ht, key, data );

  const int bucket = H( key );

  Debugln( "H( %u ) => %d", key, bucket );

  for ( int i = 0; i < HT_NEIGHBOURHOOD_SIZE; i++ ) {
    if ( ht->nodes[bucket + i].key == key ) {
      *data = ht->nodes[bucket + i].data;
      return HT_GET_SUCCESS;
    }
  }

  return HT_GET_FAILED;
}
