#ifndef STATEMENT_H
#define STATEMENT_H

#include <stdio.h>
#include <string.h>

#include "table.h"
#include "buffer.h"

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LARGE,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT,
} prepare_type_t;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
} statement_type_t;

typedef struct
{
    statement_type_t type;
    row_t row;
} statement_t;

typedef enum{
    EXECUTE_FAILURE,
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_DUPLICATE_KEY,
} execute_type_t;

prepare_type_t prepare_statement(input_buffer_t *input_buffer, statement_t *statement);
execute_type_t execute_insert (statement_t * statement, table_t * table);
execute_type_t execute_select (table_t * table);
execute_type_t execute_statement (statement_t * statement, table_t * table);

#endif