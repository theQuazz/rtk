#include "nameserver.h"
#include "../lib/hash.h"
#include "../lib/hashtable.h"

void NameServer() {
  struct HashTable ht;

  HashTableInitialize( &ht );

  for ( ;; ) {
    int tid = -1;
    struct NameServerRequest msg;
    const int err = Receive( &tid, &msg, sizeof( msg ) );

    switch ( err ) {
      default: {
        switch ( msg.type ) {
          case NS_WHO_IS: {
            int whois_tid;
            struct NameServerWhoIsResponse reply = {
              type: NS_WHO_IS_REPLY,
              status: HashTableGet( &ht, Hash( msg.name ), &whois_tid ),
              whois_tid,
            };
            Reply( tid, &reply, sizeof( reply ) );
            break;
          }
          case NS_REGISTER_AS: {
            struct NameServerRegisterAsResponse reply = {
              type: NS_REGISTER_AS_REPLY,
              status: HashTableInsert( &ht, Hash( msg.name ), tid ),
            };
            Reply( tid, &reply, sizeof( reply ) );
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
