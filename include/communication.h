#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

/** @brief Base message struct
 *
 *  Custom messages can be created by extending this struct and adding a custom
 *  type
 */
struct BaseMsg {
  int type;
};

/** @brief Send a message
 *
 *  Send sends a message to another task and receives a reply. The message, in a
 *  buffer in the sending task’s address space is copied to the address space of
 *  the task to which it is sent by the kernel. Send supplies a buffer into
 *  which the reply is to be copied, and the size of the buffer so that the
 *  kernel can detect overflow. When Send returns without error it is guaranteed
 *  that the message has been received, and that a reply has been sent, not
 *  necessarily by the same task. If either the message or the reply is a string
 *  it is necessary that the length should include the terminating null. 
 *
 *  The kernel will not overflow the reply buffer. The caller is expected to
 *  compare the return value to the size of the reply buffer. If part of the
 *  reply is missing the return value will exceed the size of the supplied reply
 *  buffer.
 *
 *  There is no guarantee that Send will return. If, for example, the task to
 *  which the message is directed never calls Receive, Send never returns and
 *  the sending task remains blocked forever. 
 *
 *  Send has a passing resemblance, and no more, to remote procedure call.
 *
 *  @param tid Task to send the message to
 *  @param msg Body of sent message
 *  @param msglen Length of sent message (bytes)
 *  @param reply Body of reply message
 *  @param replylen Length of sent message (bytes)
 *  @return The size of the message supplied by the replying task
 */
int Send( int tid, void *msg, int msglen, void *reply, int replylen );
enum SendError {
  ERR_SEND_IMPOSSIBLE_TID = -1,
  ERR_SEND_NONEXISTENT_TASK = -2,
  ERR_SRR_INCOMPLETE = -3,
};

/** @brief Receive a message
 *
 *  Receive blocks until a message is sent to the caller, then returns with the
 *  message in its message buffer and tid set to the task id of the task that
 *  sent the message. Messages sent before Receive is called are retained in a
 *  send queue, from which they are received in first-come, firstserved order.
 *
 *  The argument msg must point to a buffer at least as large as msglen. If the
 *  size of the message received exceeds msglen, no overflow occurs and the
 *  buffer will contain the first msglen characters of the message sent.
 *
 *  The caller is expected to compare the return value, which contains the size
 *  of the message that was sent, to determine whether or not the message is
 *  complete, and to act accordingly
 *
 *  @param tid Tid of process which sent the message
 *  @param msg Buffer to hold the incoming message
 *  @param msglen Size of the message buffer
 *  @return The size of the message sent
 */
int Receive( int *tid, void *msg, int msglen );

/** @brief Reply to a message
 *
 *  Reply sends a reply to a task that previously sent a message.  When it
 *  returns without error, the reply has been copied into the sender’s address
 *  space. The calling task and the sender return at the same logical time, so
 *  whichever is of higher priority runs first. If they are of the same priority
 *  the sender runs first.
 *
 *  @param tid Task to send to
 *  @param reply Body of reply
 *  @param replylen Length of body in bytes
 *  @return RETURN_STATUS_OK or ReplyError
 */
int Reply( int tid, void *reply, int replylen );
enum ReplyError {
  ERR_REPLY_IMPOSSIBLE_TID = -1,
  ERR_REPLY_NONEXISTENT_TASK = -2,
  ERR_NOT_REPLY_BLOCKED = -3,
  ERR_BUFFER_TOO_SMALL = -4,
};

#endif
