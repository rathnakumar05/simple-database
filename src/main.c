#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "buffer.h"
#include "io_handle.h"
#include "meta_command.h"
#include "statement.h"
#include "table.h"

int
main (int argc, char *argv[])
{

  if (argc < 2)
    {
      printf ("Must supply a database filename.\n");
      exit (EXIT_FAILURE);
    }

  char *filename = argv[1];
  table_t *table = db_open (filename);
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
	  db_close (table);
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

	case (PREPARE_NEGATIVE_ID):
	  printf ("ID must be positive\n");
	  continue;

	case (PREPARE_STRING_TOO_LARGE):
	  printf ("String too large\n");
	  continue;

	case (PREPARE_SYNTAX_ERROR):
	  printf ("Syntax error\n");
	  continue;

	case (PREPARE_UNRECOGNIZED_STATEMENT):
	  printf ("Unrecognized statement: %s\n", input_buffer->buffer);
	  continue;
	}

      execute_type_t execute_status = execute_statement (&statement, table);
      if (execute_status == EXECUTE_FAILURE)
	{
	  close_input_buffer (input_buffer);
	  db_close (table);
	  perror ("Failed to execute: ");
	  exit (EXIT_FAILURE);
	}

    }

  return 0;
}
