#include "shell.h"
#include "../include/communication.h"
#include "../include/nameserver.h"
#include "../include/task.h"
#include "../include/io.h"
#include "../lib/string.h"
#include "../lib/print.h"
#include "../lib/debug.h"
#include "../lib/terminal.h"

#include <stddef.h>

static void Handle( char *buffer, size_t buffer_len, int *ret ) {
  int command_len = strcspn( buffer, " " );
  char command_buf[command_len + 1];

  strncpy( command_buf, buffer, command_len );

  int handler_tid = WhoIs( command_buf );

  *ret = -1;

  switch ( handler_tid ) {
    case ERR_INVALID_NAMESERVER_TID: {
      Print( 0, "Can't connect to NameServer\n" );
      break;
    }
    case ERR_LOOKUP_FAILED: {
      Print( 0, "Command not found: %s\n", command_buf );
      break;
    }
    default: {
      int err = Send( handler_tid, buffer, buffer_len, ret, sizeof( *ret ) );
      switch ( err ) {
        case ERR_SEND_IMPOSSIBLE_TID:
        case ERR_SEND_NONEXISTENT_TASK: {
          Print( 0, "Task disappeared before receiving command" );
          break;
        }
        case ERR_SRR_INCOMPLETE: {
          Print( 0, "Internal error... exiting" );
          Exit();
        }
        default: break;
      }
      break;
    }
  }
}

static void Repl( struct ShellConfig * config ) {
  char buffer[config->command_buffer_size];
  int buffer_pos = 0;
  int length = 0;
  int ret = 0;

  prompt_loop:
  for ( ;; ) {
    if ( ret != 0 ) {
      Print( 0, "(%s%d%s) ", TERM_YELLOW, ret, TERM_RESET );
    }
    Print( 0, "%s%s%s", ret == 0 ? TERM_GREEN : TERM_RED, config->prompt, TERM_RESET );

    for ( ;; ) {
      char ch = Getc( 0 );
      switch ( ch ) {
        /* return */
        case '\r':
        case '\n': {
          Putc( 0, '\n' );

          int orig_length = length;
          ret = buffer_pos = length = 0;

          if ( orig_length == 0 ) {
            goto prompt_loop;
          }

          if ( strcmp( buffer, "exit" ) == 0 ) {
            Print( 0, "exit...\n" );
            Exit();
          }

          Handle( buffer, sizeof( buffer ), &ret );

          goto prompt_loop;
        }

        /* arrow key */
        case '\033': {
          Getc( 0 ); /* skip the [ */
          char dir = Getc( 0 );
          switch ( dir ) {
            case 'A' /* up */: break;
            case 'B' /* down */: break;
            case 'C' /* right */: {
              if ( buffer_pos < length ) {
                buffer_pos += 1;
                Putc( 0, ch );
                Putc( 0, '[');
                Putc( 0, 'C');
              }
              break;
            }
            case 'D' /* left */: {
              if ( buffer_pos > 0 ) {
                buffer_pos -= 1;
                Putc( 0, ch );
                Putc( 0, '[');
                Putc( 0, 'D');
              }
              break;
            }
          }
          break;
        }

        /* backspace */
        case 127: {
          if ( buffer_pos > 0 ) {
            Print( 0, "\b%s ", buffer + buffer_pos );
            Print( 0, TERM_MOVE_N_LEFT, length - buffer_pos + 1 );

            if ( buffer_pos != length ) {
              memmove( buffer + buffer_pos - 1, buffer + buffer_pos, length - buffer_pos );
            }

            length -= 1;
            buffer_pos -= 1;
          }
          break;
        }

        /* put char into buffer */
        default: {
          if ( length == config->command_buffer_size - 1 ) {
            break;
          }

          Putc( 0, ch );
          if ( buffer_pos < length ) {
            Print( 0, "%s", buffer + buffer_pos );
            Print( 0, TERM_MOVE_N_LEFT, length - buffer_pos );
            memmove( buffer + buffer_pos + 1, buffer + buffer_pos, length - buffer_pos );
          }

          length += 1;
          buffer[buffer_pos++] = ch;
          break;
        }
      }

      buffer[length] = '\0';

      if ( buffer_pos == config->command_buffer_size - 1 ) {
        Print( 0, TERM_HIDE_CURSOR );
      }
      else {
        Print( 0, TERM_SHOW_CURSOR );
      }
    }
  }
}

void Shell( void ) {
  struct ShellConfig config;
  int sender_tid;
  int parent_tid = MyParentTid();
  int my_tid = MyTid();

  Debugln( "Shell (%d) up, waiting for config...", my_tid );

  const int err = Receive( &sender_tid, &config, sizeof( config ) );

  Debugln( "Shell (%d) received config from (%d), running!", my_tid, sender_tid );

  Reply( sender_tid, NULL, 0 );

  if ( err < 0 ) {
    Errorln( "Shell (%d) aborting, received err( %d ) on startup", my_tid, err );
  }

  if ( sender_tid != parent_tid ) {
    Errorln( "Shell (%d) aborting, config sent from (%d) not parent (%d)", my_tid, sender_tid, parent_tid );
    Exit();
  }

  Repl( &config );
}
