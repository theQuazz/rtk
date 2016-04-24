#include "rps.h"
#include "../include/task.h"
#include "../include/nameserver.h"
#include "../lib/random.h"
#include "../lib/debug.h"
#include "../lib/string.h"

#include <stddef.h>

struct RpsGame {
  struct RpsGame *opponent;
  struct RpsPlayer {
    int tid;
    enum RpsThrow choice;
    enum RpsPlayerState {
      RPS_PLAYER_ACTIVE,
      RPS_PLAYER_WAITING,
      RPS_PLAYER_QUIT,
    } state;
  } player;
};

void RpsSignup( void ) {
  int rps_tid = WhoIs( "rps-server" );

  struct RpsRequest req = {
    type: RPS_REQ_SIGNUP,
  };

  Send( rps_tid, &req, sizeof( req ), NULL, 0 );
}

enum RpsResult RpsPlay( enum RpsThrow choice ) {
  int rps_tid = WhoIs( "rps-server" );

  struct RpsPlayResponse res;
  struct RpsThrowRequest req = {
    type: RPS_REQ_PLAY,
    choice: choice,
  };

  const int err = Send( rps_tid, &req, sizeof( req ), &res, sizeof( res ) );

  if ( err < 0 ) {
    return err;
  }

  return res.result;
}

void RpsQuit( void ) {
  int rps_tid = WhoIs( "rps-server" );

  struct RpsRequest req = {
    type: RPS_REQ_QUIT,
  };

  Send( rps_tid, &req, sizeof( req ), NULL, 0 );
}

void RpsClient( void ) {
  int cumulative_score = 0;

  RpsSignup();

  while ( cumulative_score < 20 ) {
    enum RpsThrow choice = RandMax( RPS_NUM_THROWS - 1 );
    enum RpsResult res = RpsPlay( choice );
    if ( res < 0 ) {
      break;
    }
    cumulative_score += res;
  }

  RpsQuit();
}

enum RpsResult RpsGetResult( enum RpsThrow mine, enum RpsThrow theirs ) {
  if ( mine == theirs ) {
    return RPS_TIE;
  }
  else if ( ( mine == RPS_ROCK && theirs == RPS_SCISSORS ) || mine > theirs ) {
    return RPS_WIN;
  }
  return RPS_LOSS;
}

void RpsServer( void ) {
  RegisterAs( "rps-server" );

  struct RpsGame *unmatched = NULL;
  struct RpsGame games[MAX_TASKS];

  for ( ;; ) {
    union {
      struct RpsRequest;
      struct RpsThrowRequest throw;
    } req;
    int sender_tid;

    Receive( &sender_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case RPS_REQ_SIGNUP: {
        Debugln( "%d RPS_REQ_SIGNUP", sender_tid );

        struct RpsGame *game = &games[sender_tid];
        struct RpsPlayer *p = &game->player;

        if ( ! unmatched ) {
          unmatched = game;
        }
        else {
          unmatched->opponent = game;
          game->opponent = unmatched;
          Reply( unmatched->player.tid, NULL, 0 );
          Reply( sender_tid, NULL, 0 );
          unmatched = NULL;
        }

        p->tid = sender_tid;
        p->state = RPS_PLAYER_ACTIVE;

        break;
      }

      case RPS_REQ_PLAY: {
        Debugln( "%d RPS_REQ_PLAY", sender_tid );

        struct RpsGame *game = &games[sender_tid];
        struct RpsPlayer *curr = &game->player;
        struct RpsPlayer *opp = &game->opponent->player;

        if ( ! curr->state == RPS_PLAYER_ACTIVE ) {
          break;
        }

        curr->choice = req.throw.choice;
        Debugln( "%d throws %d", sender_tid, req.throw.choice );

        switch ( opp->state ) {
          case RPS_PLAYER_ACTIVE: {
            Debugln( "%d sees %d RPS_PLAYER_ACTIVE", sender_tid, opp->tid );

            curr->state = RPS_PLAYER_WAITING;

            break;
          }

          case RPS_PLAYER_WAITING: {
            Debugln( "%d sees %d RPS_PLAYER_WAITING", sender_tid, opp->tid );

            opp->state = RPS_PLAYER_ACTIVE;

            struct RpsPlayResponse my_result = {
              .type = RPS_RES_PLAY,
              .result = RpsGetResult( curr->choice, opp->choice ),
            };
            struct RpsPlayResponse their_result = {
              .type = RPS_RES_PLAY,
              .result = RpsGetResult( opp->choice, curr->choice ),
            };
            Debugln( "result: %d => %d, %d => %d", curr->tid, my_result.result, opp->tid, their_result.result );
            Reply( curr->tid, &my_result, sizeof( my_result ) );
            Reply( opp->tid, &their_result, sizeof( their_result ) );

            break;
          }

          case RPS_PLAYER_QUIT: {
            Debugln( "%d sees %d RPS_PLAYER_QUIT", sender_tid, opp->tid );

            struct RpsPlayResponse res = {
              type: RPS_QUIT,
              result: RPS_QUIT,
            };

            Reply( sender_tid, &res, sizeof( res ) );

            break;
          }
        }

        break;
      }

      case RPS_REQ_QUIT: {
        Debugln( "%d RPS_REQ_QUIT", sender_tid );

        struct RpsPlayer *opp = &( games[sender_tid].opponent->player );

        if ( opp->state == RPS_PLAYER_WAITING ) {
          Debugln( "informing %d that their opponent quit", opp->tid );

          struct RpsPlayResponse res = {
            type: RPS_QUIT,
            result: RPS_QUIT,
          };

          Reply( sender_tid, &res, sizeof( res ) );
        }

        games[sender_tid].player.state = RPS_PLAYER_QUIT;
        Reply( sender_tid, NULL, 0 );

        break;
      }

      default: {
        Debugln( "%d RPS_REQ_UNKNOWN", sender_tid );

        break;
      }
    }
  }
}

static void RpsCmdClient( void ) {
  RpsSignup();
  for ( ;; ) {
    RpsPlay( RandMax( RPS_NUM_THROWS - 1 ) );
  }
  RpsQuit();
}

void RpsCmdServer( void ) {
  while ( RegisterAs( "rps" ) == ERR_INVALID_NAMESERVER_TID );

  Create( MEDIUM_PRIORITY, RpsCmdClient );

  RpsSignup();

  for ( ;; ) {
    char buf[128];
    int sender_tid;

    Receive( &sender_tid, buf, sizeof( buf ) );

    strtok( buf, " " );
    char *throw = strtok( NULL, " " );

    int ret = -1;
    if ( 0 == strcmp( "rock", throw ) ) {
      ret = RpsPlay( RPS_ROCK );
    }
    else if ( 0 == strcmp( "paper", throw ) ) {
      ret = RpsPlay( RPS_PAPER );
    }
    else if ( 0 == strcmp( "scissors", throw ) ) {
      ret = RpsPlay( RPS_SCISSORS );
    }
    else {
      Print( 0, "Invalid invocation\r\n" );
    }

    switch ( ret ) {
      case RPS_WIN:
        Print( 0, "win\r\n" );
        ret = 0;
        break;
      case RPS_LOSS:
        Print( 0, "loss\r\n" );
        ret = 0;
        break;
      case RPS_TIE:
        Print( 0, "tie\r\n" );
        ret = 0;
        break;
      default:
        ret = -1;
        break;
    }

    Reply( sender_tid, &ret, sizeof( ret ) );
  }
}
