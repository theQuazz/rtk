#include "nameserver.h"
#include "../include/common.h"
#include "../include/task.h"
#include "../lib/hash.h"
#include "../lib/hashtable.h"
#include "../lib/string.h"
#include "../lib/debug.h"

static int nameserver_tid = -1;

int RegisterAs( char *name ) {
  struct NameServerRegisterAsResponse response;
  struct NameServerRequest request;

  request.type = NS_REGISTER_AS;

  const int name_length = strnlen( name, NS_NAME_BUFFER_SIZE - 1 );
  strncpy( request.name, name, name_length );

  const int err = Send( nameserver_tid, &request, sizeof( request ), &response, sizeof( response ) );

  switch ( err ) {
    case RETURN_STATUS_OK: {
      switch ( response.status ) {
        case RETURN_STATUS_OK: return RETURN_STATUS_OK;
        default: return ERR_LOOKUP_FAILED;
      }
    };
    default: return ERR_INVALID_NAMESERVER_TID;
  }
}

int WhoIs( char *name ) {
  struct NameServerWhoIsResponse response;
  struct NameServerRequest request;

  request.type = NS_WHO_IS;

  const int name_length = strnlen( name, NS_NAME_BUFFER_SIZE - 1 );
  strncpy( request.name, name, name_length );

  const int err = Send( nameserver_tid, &request, sizeof( request ), &response, sizeof( response ) );

  switch ( err ) {
    case RETURN_STATUS_OK: {
      switch ( response.status ) {
        case RETURN_STATUS_OK: return response.whois_tid;
        default: return ERR_LOOKUP_FAILED;
      }
    };
    default: return ERR_INVALID_NAMESERVER_TID;
  }
}

void NameServer() {
  Debugln( "NameServer starting up..." );

  struct HashTable ht;

  HashTableInitialize( &ht );

  nameserver_tid = MyTid();

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
