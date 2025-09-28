#include "buffer.h"

input_buffer_t *
new_input_buffer ()
{
  input_buffer_t *input_buffer = malloc (sizeof (input_buffer_t));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

void
close_input_buffer (input_buffer_t * input_buffer)
{
  free (input_buffer->buffer);
  free (input_buffer);
}
