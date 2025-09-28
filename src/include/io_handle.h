#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "buffer.h"

void print_prompt();
int read_input(input_buffer_t *input_buffer);

#endif