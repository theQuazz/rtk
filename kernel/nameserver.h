#ifndef __NAMESERVER_H__
#define __NAMESERVER_H__

enum NameServerError {
  ERR_INVALID_NAMESERVER_TID = -1,
  ERR_LOOKUP_FAILED = -2,
};

/** @brief Register a name with the name server
 *
 *  RegisterAs registers the task id of the caller under the given name.
 *
 *  On return without error it is guaranteed that all WhoIs calls by any task
 *  will return the task id of the caller until the registration is overwritten.
 *
 *  If another task has already registered with the given name its registration
 *  is overwritten.
 *
 *  A single task may register under several different names, but each name is
 *  assigned to a single task.
 *
 *  RegisterAs is actually a wrapper covering a send to the name server.
 *
 *  @param name Name to register as
 *  @return RETURN_STATUS_OK or NameServerError
 */
int RegisterAs( char *name );

/** @brief Register a name with the name server
 *
 *  WhoIs asks the nameserver for the task id of the task that is registered
 *  under the given name.
 *
 *  Whether WhoIs blocks waiting for a registration or returns with an error if
 *  no task is registered under the given name is implementation-dependent.
 *
 *  There is guaranteed to be a unique task id associated with each registered
 *  name, but the registered task may change at any time after a call to WhoIs.
 *
 *  WhoIs is actually a wrapper covering a send to the nameserver.
 *
 *  @param name Name to query for
 *  @return Task ID or NameServerError
 */
int WhoIs( char *name );

#endif
