#ifndef __USR_SHELL_H__
#define __USR_SHELL_H__

struct ShellConfig {
  char *prompt;
  const int command_buffer_size;
  const int channel;
};

void Shell( void );

#endif
