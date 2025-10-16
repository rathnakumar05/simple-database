#include "statement.h"

prepare_type_t
prepare_insert (input_buffer_t * input_buffer, statement_t * statement)
{
  statement->type = STATEMENT_INSERT;
  char *keyword = strtok (input_buffer->buffer, " ");
  char *id_string = strtok (NULL, " ");
  char *username = strtok (NULL, " ");
  char *email = strtok (NULL, " ");

  // printf ("%s %s %s\n", id_string, username, email);

  if (id_string == NULL || username == NULL || email == NULL)
    {
      return PREPARE_SYNTAX_ERROR;
    }

  int id = atoi (id_string);
  if (id < 0)
    {
      return PREPARE_NEGATIVE_ID;
    }

  if (strlen (username) > COLUMN_USERNAME_SIZE)
    {
      return PREPARE_STRING_TOO_LARGE;
    }

  if (strlen (email) > COLUMN_EMAIL_SIZE)
    {
      return PREPARE_STRING_TOO_LARGE;
    }

  statement->row.id = id;
  strcpy (statement->row.email, email);
  strcpy (statement->row.username, username);

  return PREPARE_SUCCESS;
}

prepare_type_t
prepare_statement (input_buffer_t * input_buffer, statement_t * statement)
{
  if (strncmp (input_buffer->buffer, "insert", 6) == 0)
    {
      return prepare_insert (input_buffer, statement);
    }
  if (strcmp (input_buffer->buffer, "select") == 0)
    {
      statement->type = STATEMENT_SELECT;
      return PREPARE_SUCCESS;
    }
  return PREPARE_UNRECOGNIZED_STATEMENT;
}

execute_type_t
execute_insert (statement_t * statement, table_t * table)
{
  void *node = get_page (table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells (node);

  row_t *source = &(statement->row);
  uint32_t key = source->id;
  cursor_t *cursor = table_find (table, key);
  if (cursor->cell_num < num_cells)
    {
      uint32_t key_at_index = *leaf_node_key (node, cursor->cell_num);
      if (key_at_index == key)
	{
	  free (cursor);
	  return EXECUTE_DUPLICATE_KEY;
	}
    }

  leaf_node_insert (cursor, source->id, source);
  free (cursor);

  return EXECUTE_SUCCESS;
}

execute_type_t
execute_select (table_t * table)
{
  cursor_t *cursor = table_start (table);
  row_t row;

  while (!cursor->end_of_table)
    {
      void *source = cursor_value (cursor);
      if (source == NULL)
	{
	  return EXECUTE_FAILURE;
	}
      deserialize_row (source, &row);
      print_row (&row);
      cursor_advance (cursor);
    }
  free (cursor);

  return EXECUTE_SUCCESS;
}

execute_type_t
execute_statement (statement_t * statement, table_t * table)
{
  switch (statement->type)
    {
    case (STATEMENT_INSERT):
      return execute_insert (statement, table);

    case (STATEMENT_SELECT):
      return execute_select (table);
    }
}
