#include "shell.h"
#include "../include/communication.h"
#include "../include/nameserver.h"
#include "../include/task.h"
#include "../include/io.h"
#include "../include/clockserver.h"
#include "../lib/string.h"
#include "../lib/print.h"
#include "../lib/debug.h"
#include "../lib/terminal.h"

#include <stddef.h>

static void Handle( int channel, char *buffer, size_t buffer_len, int *ret ) {
  int command_len = strcspn( buffer, " " );
  char command_buf[command_len + 1];

  strncpy( command_buf, buffer, command_len );

  int handler_tid = WhoIs( command_buf );

  *ret = -1;

  switch ( handler_tid ) {
    case ERR_INVALID_NAMESERVER_TID: {
      Print( channel, "Can't connect to NameServer\r\n" );
      break;
    }
    case ERR_LOOKUP_FAILED: {
      Print( channel, "Command not found: %s\r\n", command_buf );
      break;
    }
    default: {
      int err = Send( handler_tid, buffer, buffer_len, ret, sizeof( *ret ) );
      switch ( err ) {
        case ERR_SEND_IMPOSSIBLE_TID:
        case ERR_SEND_NONEXISTENT_TASK: {
          Print( channel, "Task disappeared before receiving command\r\n" );
          break;
        }
        case ERR_SRR_INCOMPLETE: {
          Print( channel, "Internal error... exiting\r\n" );
          Exit();
        }
        default: break;
      }
      break;
    }
  }
}

static void Repl( struct ShellConfig *config ) {
  char buffer[config->command_buffer_size];
  int buffer_pos = 0;
  int length = 0;
  int ret = 0;

  prompt_loop:
  for ( ;; ) {
    if ( ret != 0 ) {
      Print( config->channel, "(%s%d%s) ", TERM_YELLOW, ret, TERM_RESET );
    }
    Print( config->channel, "%s%s%s", ret == 0 ? TERM_GREEN : TERM_RED, config->prompt, TERM_RESET );

    for ( ;; ) {
      char ch = Getc( config->channel );
      switch ( ch ) {
        /* return */
        case '\r':
        case '\n': {
          Putc( config->channel, '\r' );
          Putc( config->channel, '\n' );

          int orig_length = length;
          ret = buffer_pos = length = 0;

          if ( orig_length == 0 ) {
            goto prompt_loop;
          }

          if ( strcmp( buffer, "exit" ) == 0 ) {
            Print( config->channel, "exit...\r\n" );
            Exit();
          }

          Handle( config->channel, buffer, sizeof( buffer ), &ret );

          goto prompt_loop;
        }

        case '\f': {
          Print( config->channel, TERM_MOVE_UPPER_LEFT );
          Print( config->channel, TERM_CLEAR );
          Print( config->channel, TERM_RESET );

          goto prompt_loop;
        }

        /* arrow key */
        case '\033': {
          Getc( config->channel ); /* skip the [ */
          char dir = Getc( config->channel );
          switch ( dir ) {
            case 'A' /* up */: break;
            case 'B' /* down */: break;
            case 'C' /* right */: {
              if ( buffer_pos < length ) {
                buffer_pos += 1;
                Putc( config->channel, ch );
                Putc( config->channel, '[');
                Putc( config->channel, 'C');
              }
              break;
            }
            case 'D' /* left */: {
              if ( buffer_pos > 0 ) {
                buffer_pos -= 1;
                Putc( config->channel, ch );
                Putc( config->channel, '[');
                Putc( config->channel, 'D');
              }
              break;
            }
          }
          break;
        }

        /* backspace */
        case 127: {
          if ( buffer_pos > 0 ) {
            Print( config->channel, "\b%s ", buffer + buffer_pos );
            Print( config->channel, TERM_MOVE_N_LEFT, length - buffer_pos + 1 );

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

          Putc( config->channel, ch );
          if ( buffer_pos < length ) {
            Print( config->channel, "%s", buffer + buffer_pos );
            Print( config->channel, TERM_MOVE_N_LEFT, length - buffer_pos );
            memmove( buffer + buffer_pos + 1, buffer + buffer_pos, length - buffer_pos );
          }

          length += 1;
          buffer[buffer_pos++] = ch;
          break;
        }
      }

      buffer[length] = '\0';

      if ( buffer_pos == config->command_buffer_size - 1 ) {
        Print( config->channel, TERM_HIDE_CURSOR );
      }
      else {
        Print( config->channel, TERM_SHOW_CURSOR );
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

  Debugln( "Shell (%d) received config from (%d), running on channel (%d)!", my_tid, sender_tid, config.channel );

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
