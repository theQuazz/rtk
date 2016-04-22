#ifndef __IO_H__
#define __IO_H__

#include "common.h"

enum IoError {
  ERR_INVALID_SERIAL_SERVER_TID = -1,
};

/** @brief Get a character from a UART
 *
 *  Getc returns first unreturned character from the given UART. How
 *  communication errors are handled is implementation-dependent. Getc is
 *  actually a wrapper for a send to the serial server.
 *
 *  @param channel UART channel to fetch from
 *  @return Character retrieved or IoError
 */
int Getc( int channel );

/** @brief Transmit a character from a UART
 *
 *  Putc queues the given character for transmission by the given UART. On return
 *  the only guarantee is that the character has been queued. Whether it has been
 *  transmitted or received is not guaranteed. How configuration errors are
 *  handled is implementation-dependent. Putc is actually a wrapper for a send to
 *  the serial server.
 *
 *  @param channel UART channel to transmit from
 *  @param ch Character to write
 *  @return RETURN_STATUS_OK or IoError
 */
int Putc( int channel, char ch );

/** @brief Transmit character from a UART (blocking!)
 *
 *  @param channel UART channel to transmit from
 *  @param ch Character to write
 */
void bwputc( volatile unsigned int *uart, char c );

/** @brief Get a character from a UART (blocking!)
 *
 *  @param channel UART channel to retreive from
 *  @return Character retrieved
 */
char bwgetc( volatile unsigned int *uart );

#endif
