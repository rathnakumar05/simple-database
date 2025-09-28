#include "io_handle.h"

void
print_prompt ()
{
  printf ("db >");
}

int
read_input (input_buffer_t * input_buffer)
{
  ssize_t bytes_read =
    getline (&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read < 0)
    {
      return -1;
    }

  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = '\0';

  return 0;
}
