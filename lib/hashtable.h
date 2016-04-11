#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

enum {
 HT_SIZE = 128,
};

struct HashTableNode {
  unsigned long key;
  int data;
};

struct HashTable {
  struct HashTableNode nodes[HT_SIZE];
  int num_elements;
};

void HashTableInitialize( struct HashTable *ht );

enum HashTableInsertReturnStatus {
  HT_INSERT_SUCCESS = 0,
  HT_INSERT_FAILED = -1,
}
HashTableInsert( struct HashTable *ht, unsigned long key, const int data );

enum HashTableGetReturnStaus {
  HT_GET_SUCCESS = 0,
  HT_GET_FAILED = -1,
}
HashTableGet( struct HashTable *ht, unsigned long key, int *data );

#endif
