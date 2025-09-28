#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>

typedef struct
{
    char *buffer;
    size_t buffer_length;
    size_t input_length;
} input_buffer_t;

input_buffer_t *new_input_buffer();

void close_input_buffer(input_buffer_t *input_buffer);

#endif