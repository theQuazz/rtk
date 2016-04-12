#ifndef __K_COMMUNICATION_H__
#define __K_COMMUNICATION_H__

int SendProxy( int tid, void *msg, int msglen, void *reply, int replylen );

int ReceiveProxy( int *tid, void *msg, int msglen );

int ReplyProxy( int tid, void *reply, int replylen);

#endif
