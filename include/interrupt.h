#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

/** @brief Wait for an external event
 *
 *  AwaitEvent blocks until the event identified by eventid occurs then returns.
 *  The following details are implementation-dependent:
 *  • Whether or not the kernel collects volatile data and re-enables the
 *    interrupt.
 *  • Whether volatile data is returned as a positive integer in the return
 *    value, or in the event buffer, or not returned at all.
 *  • Whether or not interrupts are enabled when AwaitEvent returns.
 *  • Whether or not to allow more than one task to block on a single event.
 *
 *  The trade-offs associated with these implementation decisions will be
 *  discussed in class, and see below
 *
 *  @param eventid Event to wait for
 *  @param event Event data buffer
 *  @param eventlen Length of event data buffer in bytes
 *  @return Volatile data if positive integer else data is in buffer if
 *          RETURN_STATUS_OK else AwaitEventError
 */
int AwaitEvent( int eventid, void *event, int eventlen );
enum AwaitEventError {
  ERR_INVALID_EVENT = -1,
  ERR_CORRUPTED_DATA = -2,
  ERR_COLLECT_DATA_AND_REENABLE_INTERRUPTS = -3,
};

#endif
