#include "../lib/hash.h"
#include "../lib/hashtable.h"

void NameServer() {
  for ( ;; ) {
    const int *tid = -1;
    struct NameServerIncomingMsg msg;
    const int err = Receive( tid, &msg, sizeof( msg ) );

    switch ( err ) {
      default: {
        switch ( msg.type ) {
          case NAME_SERVER_MSG_TYPE_WHO_IS: {
            const int whois_tid = HashTableGet( Hash( msg.name ) );
            struct NameServerWhoIsReplyMsg = { type: whois_tid };
            break;
          }
          case NAME_SERVER_MSG_TYPE_REGISTER_AS: {
            HashTableInsert( Hash( msg.name ), *tid );
            struct BaseMsg reply = { type: RETURN_STATUS_OK };
            Reply( *tid, &reply, sizeof( reply ) );
            break;
          }
          default: break;
        }
        break;
      }
      case ERR_INVALID_NAMESERVER_TID: break;
    }
  }
}
