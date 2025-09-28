#include "statement.h"

prepare_type_t
prepare_statement (input_buffer_t * input_buffer, statement_t * statement)
{
  if (strncmp (input_buffer->buffer, "insert", 6) == 0)
    {
      statement->type = STATEMENT_INSERT;
      int args_assigned = sscanf (input_buffer->buffer, "insert %d %s %s",
				  &(statement->row.id),
				  statement->row.username,
				  statement->row.email);
      if (args_assigned < 3)
	{
	  return PREPARE_SYNTAX_ERROR;
	}
      return PREPARE_SUCCESS;
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
  if (table->num_rows == TABLE_MAX_ROWS)
    {
      return EXECUTE_TABLE_FULL;
    }

  row_t *source = &(statement->row);
  void *destination = row_slot (table, table->num_rows);
  if (destination == NULL)
    {
      return EXECUTE_FAILURE;
    }
  serialize_row (source, destination);
  table->num_rows++;
  return EXECUTE_SUCCESS;
}

execute_type_t
execute_select (table_t * table)
{
  row_t row;
  for (uint32_t i = 0; i < table->num_rows; i++)
    {
      void *source = row_slot (table, i);
      if (source == NULL)
	{
	  return EXECUTE_FAILURE;
	}
      deserialize_row (source, &row);
      print_row (&row);
    }
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
