#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>


typedef struct
{
  char *buffer;
  size_t buffer_length;
  size_t input_length;
} input_buffer_t;

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

void
close_input_buffer (input_buffer_t * input_buffer)
{
  free (input_buffer->buffer);
  free (input_buffer);
}

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
