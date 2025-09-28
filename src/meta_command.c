#include "meta_command.h"

meta_command_type_t
do_meta_command (input_buffer_t * input_buffer, table_t * table)
{
  if (strcmp (input_buffer->buffer, ".exit") == 0)
    {
      printf ("exiting..\n");
      free_table (table);
      close_input_buffer (input_buffer);
      exit (EXIT_SUCCESS);
    }
  else
    {
      return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
