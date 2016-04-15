#ifndef __TERMINAL_H__
#define __TERMINAL_H__

/* utils */
#define TERM_CLEAR "\033[2J"
#define TERM_MOVE_UPPER_LEFT "\033[H"
#define TERM_MOVE_ROW_COL "\033[%d;%dH"
#define TERM_HIDE_CURSOR "\033[?25l"
#define TERM_SHOW_CURSOR "\033[?25h"
#define TERM_DEL_TO_EOL "\033[K"
#define TERM_MOVE_N_LEFT "\033[%dD"

/* colours */
#define TERM_RESET "\033[0m"
#define TERM_BLACK "\033[30m"
#define TERM_RED "\033[31m"
#define TERM_GREEN "\033[32m"
#define TERM_YELLOW "\033[33m"
#define TERM_BLUE "\033[34m"
#define TERM_MAGENTA "\033[35m"
#define TERM_CYAN "\033[36m"
#define TERM_WHITE "\033[37m"

#endif
