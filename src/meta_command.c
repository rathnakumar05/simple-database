#include "meta_command.h"

void
print_constants ()
{
  printf ("ROW_SIZE: %d\n", ROW_SIZE);
  printf ("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
  printf ("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
  printf ("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
  printf ("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
  printf ("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void
print_leaf_node (void *node)
{
  uint32_t num_cells = *leaf_node_num_cells (node);
  printf ("leaf (size %d)\n", num_cells);
  for (uint32_t i = 0; i < num_cells; i++)
    {
      uint32_t key = *leaf_node_key (node, i);
      printf ("  - %d : %d\n", i, key);
    }
}

meta_command_type_t
do_meta_command (input_buffer_t * input_buffer, table_t * table)
{
  if (strcmp (input_buffer->buffer, ".exit") == 0)
    {
      printf ("exiting..\n");
      db_close (table);
      close_input_buffer (input_buffer);
      exit (EXIT_SUCCESS);
    }
  else if (strcmp (input_buffer->buffer, ".constants") == 0)
    {
      printf ("Constants:\n");
      print_constants ();
      return META_COMMAND_SUCCESS;
    }
  else if (strcmp (input_buffer->buffer, ".btree") == 0)
    {
      printf ("Tree:\n");
      print_leaf_node (get_page (table->pager, 0));
      return META_COMMAND_SUCCESS;
    }
  else
    {
      return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
