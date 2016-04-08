#ifndef __NAMESERVER_PROC_H__
#define __NAMESERVER_PROC_H__

#include "../kernel/communication.h"

enum {
  NAMESERVER_NAME_MAX_LEN = 128,
};

enum NameServerMsgType {
  NS_WHO_IS,
  NS_REGISTER_AS,
  NS_WHO_IS_REPLY,
};

struct NameServerIncomingMsg {
  struct BaseMsg;
  const char name[NAMESERVER_NAME_MAX_LEN + 1];
};

struct NameServerResponseMsg {
  struct BaseMsg;
  const int whois_tid;
};

#endif
