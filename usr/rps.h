#ifndef __RPS_H_
#define __RPS_H_

#include "../include/communication.h"

enum RpsMsgType {
  RPS_REQ_SIGNUP,
  RPS_REQ_QUIT,
  RPS_REQ_PLAY,
  RPS_RES_PLAY,
};

enum RpsResult {
  RPS_QUIT = -1,
  RPS_LOSS,
  RPS_TIE,
  RPS_WIN,
};

enum RpsThrow {
  RPS_ROCK,
  RPS_PAPER,
  RPS_SCISSORS,
};

enum {
  RPS_NUM_THROWS = RPS_SCISSORS + 1,
};

struct RpsGame {
  struct RpsGame *next, *prev;
  struct RpsPlayer {
    int wins;
    int ties;
    int losses;
    int tid;
    int choice;
    enum RpsPlayerState {
      RPS_PLAYER_ACTIVE,
      RPS_PLAYER_QUIT,
    } state;
  } player_a, player_b;
};

struct RpsRequest {
  struct BaseMsg;
  union {
    enum RpsThrow choice;
    struct RpsGame game;
  };
};

struct RpsPlayResponse {
  struct BaseMsg;
  enum RpsResult result;
};

void RpsSignup( void );
enum RpsResult RpsPlay( enum RpsThrow choice );
void RpsQuit( void );

void RpsClient( void );
void RpsServer( void );

#endif
