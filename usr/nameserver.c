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
  request.name[name_length] = '\0';

  Debugln( "Sending RegisterAs request (%s)", request.name );

  const int err = Send( nameserver_tid, &request, sizeof( request ), &response, sizeof( response ) );

  Debugln( "Got RegisterAs reply err( %d )", err );

  if ( err > 0 ) {
    switch ( response.status ) {
      case RETURN_STATUS_OK: return RETURN_STATUS_OK;
      default: return ERR_LOOKUP_FAILED;
    }
  }
  else {
    return ERR_INVALID_NAMESERVER_TID;
  }
}

int WhoIs( char *name ) {
  struct NameServerWhoIsResponse response;
  struct NameServerRequest request;

  request.type = NS_WHO_IS;

  const int name_length = strnlen( name, NS_NAME_BUFFER_SIZE - 1 );
  strncpy( request.name, name, name_length );

  Debugln( "Sending WhoIs request (%s)", request.name );

  const int err = Send( nameserver_tid, &request, sizeof( request ), &response, sizeof( response ) );

  Debugln( "Got WhoIs reply err( %d )", err );

  if ( err > 0 ) {
    switch ( response.status ) {
      case RETURN_STATUS_OK: return response.whois_tid;
      default: return ERR_LOOKUP_FAILED;
    }
  }
  else {
    return ERR_INVALID_NAMESERVER_TID;
  }
}

static struct HashTable ht;

void NameServer( void ) {
  Debugln( "NameServer starting up..." );

  HashTableInitialize( &ht );

  nameserver_tid = MyTid();

  Debugln( "NameServer ready (%d)", nameserver_tid );

  for ( ;; ) {
    int tid = -1;
    struct NameServerRequest msg;
    Receive( &tid, &msg, sizeof( msg ) );

    switch ( msg.type ) {
      case NS_WHO_IS: {
        Debugln( "Received WhoIs request for \"%s\"", msg.name );
        int whois_tid;
        struct NameServerWhoIsResponse reply = {
          type: NS_WHO_IS_REPLY,
          status: HashTableGet( &ht, Hash( msg.name ), &whois_tid ),
          whois_tid,
        };
        Debugln( "Responding to WhoIs request with %d err( %d )", whois_tid, reply.status );
        Reply( tid, &reply, sizeof( reply ) );
        break;
      }
      case NS_REGISTER_AS: {
        Debugln( "Received RegisterAs request for \"%s\"", msg.name );
        struct NameServerRegisterAsResponse reply = {
          type: NS_REGISTER_AS_REPLY,
          status: HashTableInsert( &ht, Hash( msg.name ), tid ),
        };
        Debugln( "Responding to RegsiterAs request with err( %d )", reply.status );
        Reply( tid, &reply, sizeof( reply ) );
        break;
      }
      default: {
        Debugln( "Unknown message type, aborting!" );
        break;
      }
    }
  }
}
