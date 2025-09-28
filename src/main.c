#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "buffer.h"
#include "io_handle.h"

int
main ()
{
  input_buffer_t *input_buffer = new_input_buffer ();
  while (true)
    {
      print_prompt ();
      if (read_input (input_buffer) == -1)
	{
	  close_input_buffer (input_buffer);
	  perror ("Failed to read line: ");
	  exit (EXIT_FAILURE);
	}

      if (strcmp (input_buffer->buffer, ".exit") == 0)
	{
	  printf ("exiting..\n");
	  close_input_buffer (input_buffer);
	  exit (EXIT_SUCCESS);
	}
      else
	{
	  printf ("Command not found\n");
	}

    }

  return 0;
}
