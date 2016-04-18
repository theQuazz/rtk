#include "sortedlist.h"

#include <stddef.h>

void SortedListInsert( struct sorted_list_node **lst, struct sorted_list_node *node ) {
  struct sorted_list_node **at = lst;

  for ( ;; ) {
    if ( ( ! *at ) || ( ( *at )->priority > node->priority ) ) {
      node->next = *at;
      *at = node;
      return;
    }

    if ( ! ( *at )->next ) {
      ( *at )->next = node;
      node->next = NULL;
      return;
    }

    at = &( *at )->next;
  }
}
