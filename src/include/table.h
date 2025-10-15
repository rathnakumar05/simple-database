#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 100

typedef struct
{
    uint32_t id;
    char username[COLUMN_EMAIL_SIZE+1];
    char email[COLUMN_EMAIL_SIZE+1];
} row_t;

#define size_of_attribute(struct_t, attribute) sizeof(((struct_t *)0)->attribute)

extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;

extern const uint32_t PAGE_SIZE;

typedef enum {
    NODE_INTERNAL,
    NODE_LEAF
} node_type_t;

extern const uint32_t NODE_TYPE_SIZE;
extern const uint32_t NODE_TYPE_OFFSET;
extern const uint32_t IS_ROOT_SIZE;
extern const uint32_t IS_ROOT_OFFSET;
extern const uint32_t PARENT_POINTER_SIZE;
extern const uint32_t PARENT_POINTER_OFFSET;
extern const uint8_t COMMON_NODE_HEADER_SIZE;

extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;
extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;

typedef struct {
    int file_d;
    uint32_t file_size;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES];
} pager_t;

typedef struct
{
    uint32_t root_page_num;
    pager_t * pager;
} table_t;

typedef struct {
    table_t * table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;
} cursor_t;

uint32_t * leaf_node_num_cells (void *node);
void * leaf_node_cell (void *node, uint32_t num);
uint32_t * leaf_node_key (void *node, uint32_t num);
void * leaf_node_value (void *node, uint32_t num);
void initialize_leaf_node (void *node);

void print_row(row_t *row);
void serialize_row(row_t *source, void *destination);
void deserialize_row(void *source, row_t *destination);

cursor_t * table_start(table_t * table);
cursor_t * table_end (table_t * table);
void * cursor_value (cursor_t * cursor);
void cursor_advance (cursor_t * cursor);

void* get_page(pager_t *pager, uint32_t page_num);
pager_t * pager_open (const char *file_name);
table_t * db_open (const char *file_name);
int pager_flush (pager_t * pager, uint32_t page_num);
int db_close (table_t * table);

int leaf_node_insert (cursor_t * cursor, uint32_t key, row_t * value);

#endif