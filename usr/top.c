#include "top.h"
#include "../include/task.h"
#include "../include/clockserver.h"
#include "../include/nameserver.h"
#include "../include/communication.h"
#include "../include/io.h"
#include "../lib/print.h"
#include "../lib/math.h"
#include "../lib/terminal.h"

#include <stdlib.h>

struct TasksStats stats;

int CompareTaskStatsByTid( const void *a, const void *b ) {
  const struct TaskStats *ta = a;
  const struct TaskStats *tb = b;

  return ( ta->tid > tb->tid ) - ( ta->tid < tb->tid );
}

void Top( void ) {
  for ( ;; ) {
    int time = Time();
    GetTasksStats( &stats );

    qsort( stats.tasks, stats.num_tasks, sizeof( struct TaskStats ), CompareTaskStatsByTid );

    Print( 0, TERM_MOVE_UPPER_LEFT );

    Print(
        0,
        "# tasks: (%d/%d) uptime: %02d:%02d:%02d.%01d\r\n\r\n",
        stats.num_alive_tasks,
        stats.num_tasks,
        ( time / 360000 ) % 60,
        ( time / 6000 ) % 60,
        ( time / 100 ) % 60,
        ( time / 10) % 10
    );

    Print( 0, "+------+----------+-------+-------------+------------+------------+-----------------+-----------------+\r\n" );
    Print( 0, "| tid  | priority | state | # activates | time given |  time used | %% used of given | %% used of total |\r\n" );
    Print( 0, "+------+----------+-------+-------------+------------+------------+-----------------+-----------------+\r\n" );

    for ( unsigned int i = 0; i < stats.num_tasks; i++ ) {
      Print(
          0,
          "| %4d | %8d | %5d | %11d | %10lu | %10lu | %15d | %15d |\r\n",
          stats.tasks[i].tid,
          stats.tasks[i].priority,
          stats.tasks[i].state,
          stats.tasks[i].num_activates,
          stats.tasks[i].allowed_user_time / 10,
          stats.tasks[i].used_user_time / 10,
          stats.tasks[i].used_user_time / max( stats.tasks[i].allowed_user_time / 100, 100 ),
          stats.tasks[i].used_user_time / max( time, 1 )
      );
    }

    Print( 0, "+------+----------+-------+-------------+------------+------------+-----------------+-----------------+" );

    Delay( 100 );
  }
}

void TopServer( void ) {
  while ( RegisterAs( "top" ) == ERR_INVALID_NAMESERVER_TID );

  for ( ;; ) {
    char buf[128];
    int sender_tid;

    Receive( &sender_tid, buf, sizeof( buf ) );

    Print( 0, TERM_CLEAR );
    Print( 0, TERM_HIDE_CURSOR );

    const int top_tid = Create( GetMyPriority(), Top );

    for ( ;; ) {
      if ( Getc( 0 ) == 'q' ) {
        break;
      }
    }

    Destroy( top_tid );

    Print( 0, "\r\n" );
    Print( 0, TERM_SHOW_CURSOR );

    int ret = 0;
    Reply( sender_tid, &ret, sizeof( ret ) );
  }
}
