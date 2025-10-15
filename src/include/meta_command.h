#ifndef META_COMMAND_H
#define META_COMMAND_H

#include <stdio.h>
#include <string.h>

#include "buffer.h"
#include "table.h"

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} meta_command_type_t;

void print_constants ();
void print_leaf_node (void *node);
meta_command_type_t do_meta_command(input_buffer_t *input_buffer, table_t *table);

#endif