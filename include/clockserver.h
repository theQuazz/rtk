#ifndef __CLOCKSERVER_H__
#define __CLOCKSERVER_H__

enum ClockServerError {
  ERR_INVALID_CLOCKSERVER_TID = -1,
};

/** @brief Wait for a given amount of time
 *
 *  Delay returns after the given number of ticks has elapsed. How long after is
 *  not guaranteed because the caller may have to wait on higher priority tasks.
 *
 *  @param ticks Number of ticks to wait
 *  @return RETURN_STATUS_OK or ClockServerError
 */
int Delay( int ticks );

/** @brief Give the time since clock server start up
 *
 *  Time returns the number of ticks since the clock server was created and
 *  initialized. With a 10 millisecond tick and a 32-bit int there should be
 *  neither wraparound nor negative time. Time is actually a wrapper for a send
 *  to the clock server.
 *
 *  @return Number of ticks or ClockServerError
 */
int Time( void );

/** @brief Wait until a time
 *
 *  Delay returns when the time since clock server initialization is greater
 *  than the given number of ticks. How long after is not guaranteed because the
 *  caller may have to wait on higher priority tasks
 *
 *  @param ticks Number of ticks to wait until
 *  @return RETURN_STATUS_OK or ClockServerError
 */
int DelayUntil( int ticks );

#endif
