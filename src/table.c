#include "table.h"

const uint32_t ID_SIZE = size_of_attribute (row_t, id);
const uint32_t USERNAME_SIZE = size_of_attribute (row_t, username);
const uint32_t EMAIL_SIZE = size_of_attribute (row_t, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void
print_row (row_t * row)
{
  printf ("(%u, %s, %s)\n", row->id, row->username, row->email);
}

void
serialize_row (row_t * source, void *destination)
{
  memcpy (destination + ID_OFFSET, &(source->id), ID_SIZE);
  strncpy (destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
  strncpy (destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void
deserialize_row (void *source, row_t * destination)
{
  memcpy (&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy (destination->username, source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy (destination->email, source + EMAIL_OFFSET, EMAIL_SIZE);
}

void *
get_page (pager_t * pager, uint32_t page_num)
{
  if (page_num >= TABLE_MAX_PAGES)
    {
      printf ("Exceed max page\n");
      return NULL;
    }

  if (pager->pages[page_num] == NULL)
    {
      void *page = malloc (PAGE_SIZE);
      uint32_t num_pages = pager->file_size / PAGE_SIZE;

      if (pager->file_size % PAGE_SIZE)
	{
	  num_pages++;
	}

      if (page_num <= num_pages)
	{
	  lseek (pager->file_d, page_num * PAGE_SIZE, SEEK_SET);
	  ssize_t bytes_read = read (pager->file_d, page, PAGE_SIZE);
	  if (bytes_read == -1)
	    {
	      return NULL;
	    }
	}

      pager->pages[page_num] = page;
    }

  return pager->pages[page_num];
}

cursor_t *
table_start (table_t * table)
{
  cursor_t *cursor = malloc (sizeof (cursor_t));
  cursor->table = table;
  cursor->row_num = 0;
  cursor->end_of_table = (table->num_rows == 0);

  return cursor;
}

cursor_t *
table_end (table_t * table)
{
  cursor_t *cursor = malloc (sizeof (cursor_t));
  cursor->table = table;
  cursor->row_num = table->num_rows;
  cursor->end_of_table = true;

  return cursor;
}

void *
cursor_value (cursor_t * cursor)
{
  uint32_t row_num = cursor->row_num;
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  void *page = get_page (cursor->table->pager, page_num);
  if (page == NULL)
    {
      return NULL;
    }
  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;

  return page + byte_offset;
}

void
cursor_advance (cursor_t * cursor)
{
  cursor->row_num++;
  if (cursor->row_num >= cursor->table->num_rows)
    {
      cursor->end_of_table = true;
    }
}

pager_t *
pager_open (const char *file_name)
{
  int fd = open (file_name, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
  if (fd == -1)
    {
      return NULL;
    }
  off_t file_size = lseek (fd, 0, SEEK_END);

  pager_t *pager = malloc (sizeof (pager_t));
  pager->file_d = fd;
  pager->file_size = file_size;
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
      pager->pages[i] = NULL;
    }

  return pager;
}

table_t *
db_open (const char *file_name)
{
  pager_t *pager = pager_open (file_name);
  if (pager == NULL)
    {
      return NULL;
    }

  uint32_t num_rows = pager->file_size / ROW_SIZE;

  table_t *table = malloc (sizeof (table_t));
  table->pager = pager;
  table->num_rows = num_rows;

  return table;
}

int
pager_flush (pager_t * pager, uint32_t page_num, uint32_t size)
{
  if (pager->pages[page_num] == NULL)
    {
      printf ("Failed to flush null page\n");
      return -1;
    }

  off_t offset = lseek (pager->file_d, page_num * PAGE_SIZE, SEEK_SET);

  if (offset == -1)
    {
      return -1;
    }

  ssize_t bytes_written = write (pager->file_d, pager->pages[page_num], size);

  if (bytes_written == -1)
    {
      return -1;
    }

  return 0;
}

int
db_close (table_t * table)
{
  pager_t *pager = table->pager;
  uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

  for (uint32_t i = 0; i < num_full_pages; i++)
    {
      if (pager->pages[i] == NULL)
	{
	  continue;
	}

      pager_flush (pager, i, PAGE_SIZE);
      free (pager->pages[i]);
      pager->pages[i] = NULL;
    }

  uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
  if (num_additional_rows > 0)
    {
      uint32_t page_num = num_full_pages;
      if (pager->pages[page_num] != NULL)
	{
	  pager_flush (pager, page_num, num_additional_rows * ROW_SIZE);
	  free (pager->pages[page_num]);
	  pager->pages[page_num] = NULL;
	}
    }

  int result = close (pager->file_d);
  if (result == -1)
    {
      return -1;
    }

  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
      void *page = pager->pages[i];
      if (page)
	{
	  free (page);
	  pager->pages[i] = NULL;
	}
    }

  free (pager);
  free (table);

  return 0;
}
