#include "nameserver.h"
#include "../lib/string.h"
#include "../usr/nameserver.h"
#include "../include/nameserver.h"
#include "../include/common.h"
#include "../include/communication.h"

int nameserver_tid = -1;

int RegisterAsProxy( char *name ) {
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

int WhoIsProxy( char *name ) {
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
