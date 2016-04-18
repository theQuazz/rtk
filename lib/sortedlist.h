#ifndef __SORTEDLIST_H__
#define __SORTEDLIST_H__

struct sorted_list_node {
  struct sorted_list_node *next;
  int priority;
};

void SortedListInsert( struct sorted_list_node **lst, struct sorted_list_node *node );

#endif
