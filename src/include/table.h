#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 100

typedef struct
{
    uint32_t id;
    char username[COLUMN_EMAIL_SIZE];
    char email[COLUMN_EMAIL_SIZE];
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

typedef struct
{
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} table_t;

void print_row(row_t *row);
void serialize_row(row_t *source, void *destination);
void deserialize_row(void *source, row_t *destination);
void *row_slot(table_t *table, uint32_t row_num);
table_t * new_table ();
void free_table(table_t * table);

#endif