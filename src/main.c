#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "buffer.h"
#include "io_handle.h"
#include "meta_command.h"
#include "statement.h"
#include "table.h"

int
main ()
{
  table_t *table = new_table ();
  if (table == NULL)
    {
      exit (EXIT_FAILURE);
    }
  input_buffer_t *input_buffer = new_input_buffer ();
  while (true)
    {
      print_prompt ();
      if (read_input (input_buffer) == -1)
	{
	  close_input_buffer (input_buffer);
	  free_table (table);
	  perror ("Failed to read line: ");
	  exit (EXIT_FAILURE);
	}

      if (input_buffer->buffer[0] == '.')
	{
	  switch (do_meta_command (input_buffer, table))
	    {
	    case META_COMMAND_SUCCESS:
	      continue;
	    case META_COMMAND_UNRECOGNIZED_COMMAND:
	      printf ("Unrecognized command '%s'\n", input_buffer->buffer);
	      continue;
	    }
	}

      statement_t statement;
      switch (prepare_statement (input_buffer, &statement))
	{
	case (PREPARE_SUCCESS):
	  break;

	case (PREPARE_UNRECOGNIZED_STATEMENT):
	  printf ("Unrecognized statement: %s\n", input_buffer->buffer);
	  continue;
	}

      execute_type_t execute_status = execute_statement (&statement, table);
      if (execute_status == EXECUTE_FAILURE)
	{
	  close_input_buffer (input_buffer);
	  free_table (table);
	  perror ("Failed to execute: ");
	  exit (EXIT_FAILURE);
	}

    }

  return 0;
}
