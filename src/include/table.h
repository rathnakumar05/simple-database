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
extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

typedef struct {
    int file_d;
    uint32_t file_size;
    void *pages[TABLE_MAX_PAGES];
} pager_t;

typedef struct
{
    uint32_t num_rows;
    pager_t * pager;
} table_t;

typedef struct {
    table_t * table;
    uint32_t row_num;
    bool end_of_table;
} cursor_t;

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
int pager_flush (pager_t * pager, uint32_t page_num, uint32_t size);
int db_close (table_t * table);

#endif