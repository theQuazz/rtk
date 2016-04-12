#include "communication.h"
#include "../include/common.h"

int SendProxy( int tid, void *msg, int msglen, void *reply, int replylen ) {
  return RETURN_STATUS_OK;
}

int ReceiveProxy( int *tid, void *msg, int msglen ) {
  return RETURN_STATUS_OK;
}

int ReplyProxy( int tid, void *reply, int replylen ) {
  return RETURN_STATUS_OK;
}
