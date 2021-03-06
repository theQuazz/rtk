#include "clockserver.h"
#include "../include/task.h"
#include "../include/nameserver.h"
#include "../include/clockserver.h"
#include "../include/communication.h"
#include "../include/interrupt.h"
#include "../include/common.h"
#include "../lib/debug.h"
#include "../lib/string.h"
#include "../lib/sortedlist.h"
#include "../arch/arm/versatilepb.h"

#include <stddef.h>

enum ClockRequestType {
  CS_TIME,
  CS_DELAY,
  CS_DELAY_UNTIL,
  CS_TICK,
};

struct ClockRequest {
  struct BaseMsg;
  int data;
};

struct task_sorted_list_node {
  struct sorted_list_node;
  int tid;
};

int Delay( int num_ticks ) {
  const int cs_tid = WhoIs( "clock-server" );

  if ( cs_tid < 0 ) {
    return ERR_INVALID_CLOCKSERVER_TID;
  }

  struct ClockRequest req = {
    .type = CS_DELAY,
    .data = num_ticks,
  };

  Send( cs_tid, &req, sizeof( req ), NULL, 0 );

  return RETURN_STATUS_OK;
}

int DelayUntil( int num_ticks ) {
  const int cs_tid = WhoIs( "clock-server" );

  if ( cs_tid < 0 ) {
    return ERR_INVALID_CLOCKSERVER_TID;
  }

  struct ClockRequest req = {
    .type = CS_DELAY_UNTIL,
    .data = num_ticks,
  };

  Send( cs_tid, &req, sizeof( req ), NULL, 0 );

  return RETURN_STATUS_OK;
}

int Time( void ) {
  const int cs_tid = WhoIs( "clock-server" );

  if ( cs_tid < 0 ) {
    return ERR_INVALID_CLOCKSERVER_TID;
  }

  struct ClockRequest req = {
    .type = CS_TIME,
  };

  int time;

  Send( cs_tid, &req, sizeof( req ), &time, sizeof( time ) );

  return time;
}

void TimerNotifier( void ) {
  const int server_tid = MyParentTid();

  struct ClockRequest req = {
    .type = CS_TICK,
  };

  for ( ;; ) {
    AwaitEvent( PIC_TIMER01 );
    Debugln( "TIMER_EXPIRED" );
    Send( server_tid, &req, sizeof( req ), NULL, 0 );
  }
}

void ClockServer( void ) {
  while ( RegisterAs( "clock-server" ) < 0 );

  Create( HIGHEST_PRIORITY, TimerNotifier );

  struct task_sorted_list_node nodes[MAX_TASKS];
  struct sorted_list_node *delayed_queue = NULL;

  int num_ticks = 0;

  for ( ;; ) {
    struct ClockRequest req;
    int sender_tid;

    Receive( &sender_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case CS_TIME: {
        Reply( sender_tid, &num_ticks, sizeof( num_ticks ) );
        break;
      }
      case CS_DELAY: {
        nodes[sender_tid].tid = sender_tid;
        nodes[sender_tid].priority = req.data + num_ticks;
        SortedListInsert( &delayed_queue, &nodes[sender_tid] );
        break;
      }
      case CS_DELAY_UNTIL: {
        nodes[sender_tid].tid = sender_tid;
        nodes[sender_tid].priority = req.data;
        SortedListInsert( &delayed_queue, &nodes[sender_tid] );
        break;
      }
      case CS_TICK: {
        Reply( sender_tid, NULL, 0 );
        num_ticks += 1;
        break;
      }
      default: break;
    }

    struct sorted_list_node *head = ( void* )delayed_queue;
    while ( head && head->priority <= num_ticks ) {
      Reply( ( ( struct task_sorted_list_node* )head )->tid, NULL, 0 );
      head = head->next;
    }
    delayed_queue = head;
  }
}
