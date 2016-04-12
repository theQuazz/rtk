#ifndef __NAMESERVER_PROC_H__
#define __NAMESERVER_PROC_H__

#include "../include/nameserver.h"
#include "../include/communication.h"

enum {
  NS_NAME_BUFFER_SIZE = 128,
};

enum NameServerMsgType {
  NS_REGISTER_AS,
  NS_REGISTER_AS_REPLY,
  NS_WHO_IS,
  NS_WHO_IS_REPLY,
};

struct NameServerRequest {
  struct BaseMsg;
  char name[NS_NAME_BUFFER_SIZE];
};

struct NameServerWhoIsResponse {
  struct BaseMsg;
  const int status;
  const int whois_tid;
};

struct NameServerRegisterAsResponse {
  struct BaseMsg;
  const int status;
};

void NameServer( void );

#endif
