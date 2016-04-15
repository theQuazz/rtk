#include "rps.h"
#include "../include/nameserver.h"
#include "../lib/random.h"

#include <stddef.h>

enum RpsResult RpsPlay( enum RpsThrow choice ) {
  int rps_tid = WhoIs( "rps-server" );

  struct RpsPlayResponse res;
  struct RpsRequest req = {
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
  int rps_tid = WhoIs( "rps-server" );

  struct RpsRequest req = {
    type: RPS_REQ_SIGNUP,
  };

  Send( rps_tid, &req, sizeof( req ), NULL, 0 );

  while ( cumulative_score < 10 ) {
    enum RpsThrow choice = RandMax( RPS_NUM_THROWS - 1 );
    enum RpsResult res = RpsPlay( choice );
    if ( res < 0 ) {
      break;
    }
    cumulative_score += res;
  }

  RpsQuit();
}

static struct RpsGame games[128];

void RpsServer( void ) {
  RegisterAs( "rps-server" );

  struct RpsGame *unmatched = NULL;
  struct RpsGame *games = NULL;

  for ( ;; ) {
    struct RpsRequest req;
    int sender_tid;
    Receive( &sender_tid, &req, sizeof( req ) );

    switch ( req.type ) {
      case RPS_REQ_SIGNUP: {
        struct RpsPlayer *p = &req.game.player_a;
        if ( ! unmatched ) {
          unmatched = &req.game;
        }
        else {
          p = &req.game.player_b;
        }

        p->wins = 0;
        p->ties = 0;
        p->losses = 0;
        p->tid = sender_tid;
        p->state = RPS_PLAYER_ACTIVE;

        break;
      }

      case RPS_REQ_PLAY: {
        struct RpsPlayer *curr, *opp;
        struct RpsGame *game
        switch ( sender_tid ) {
          case 
        }
        break;
      }

      default: break;
    }
  }
}
