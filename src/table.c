#include "table.h"

const uint32_t ID_SIZE = size_of_attribute (row_t, id);
const uint32_t USERNAME_SIZE = size_of_attribute (row_t, username);
const uint32_t EMAIL_SIZE = size_of_attribute (row_t, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;

const uint32_t NODE_TYPE_SIZE = sizeof (uint32_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof (uint32_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof (uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t COMMON_NODE_HEADER_SIZE =
  NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof (uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE =
  COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

const uint32_t LEAF_NODE_KEY_SIZE = sizeof (uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET =
  LEAF_NODE_KEY_OFFSET + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE =
  LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS =
  LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

uint32_t *
leaf_node_num_cells (void *node)
{
  return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *
leaf_node_cell (void *node, uint32_t num)
{
  return node + LEAF_NODE_HEADER_SIZE + num * LEAF_NODE_CELL_SIZE;
}

uint32_t *
leaf_node_key (void *node, uint32_t num)
{
  return leaf_node_cell (node, num);
}

void *
leaf_node_value (void *node, uint32_t num)
{
  return leaf_node_cell (node, num) + LEAF_NODE_KEY_SIZE;
}

void
initialize_leaf_node (void *node)
{
  *leaf_node_num_cells (node) = 0;
}

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
      if (page_num >= pager->num_pages)
	{
	  pager->num_pages = page_num + 1;
	}
    }

  return pager->pages[page_num];
}

cursor_t *
table_start (table_t * table)
{
  cursor_t *cursor = malloc (sizeof (cursor_t));
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  cursor->cell_num = 0;

  void *root_node = get_page (table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells (root_node);
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

cursor_t *
table_end (table_t * table)
{
  cursor_t *cursor = malloc (sizeof (cursor_t));
  cursor->table = table;
  cursor->page_num = table->root_page_num;

  void *root_node = get_page (table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells (root_node);
  cursor->cell_num = num_cells;
  cursor->end_of_table = true;

  return cursor;
}

void *
cursor_value (cursor_t * cursor)
{
  uint32_t page_num = cursor->page_num;
  void *page = get_page (cursor->table->pager, page_num);
  if (page == NULL)
    {
      return NULL;
    }

  return leaf_node_value (page, cursor->cell_num);
}

void
cursor_advance (cursor_t * cursor)
{
  uint32_t page_num = cursor->page_num;
  void *node = get_page (cursor->table->pager, page_num);

  cursor->cell_num++;
  if (cursor->cell_num >= (*leaf_node_num_cells (node)))
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
  pager->num_pages = (file_size / PAGE_SIZE);

  if (file_size % PAGE_SIZE != 0)
    {
      printf ("DB file is not found!");
      return NULL;
    }

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

  table_t *table = malloc (sizeof (table_t));
  table->pager = pager;
  table->root_page_num = 0;

  if (pager->num_pages == 0)
    {
      void *root_node = get_page (pager, 0);
      initialize_leaf_node (root_node);
    }

  return table;
}

int
pager_flush (pager_t * pager, uint32_t page_num)
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

  ssize_t bytes_written =
    write (pager->file_d, pager->pages[page_num], PAGE_SIZE);

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

  for (uint32_t i = 0; i < pager->num_pages; i++)
    {
      if (pager->pages[i] == NULL)
	{
	  continue;
	}

      pager_flush (pager, i);
      free (pager->pages[i]);
      pager->pages[i] = NULL;
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

int
leaf_node_insert (cursor_t * cursor, uint32_t key, row_t * value)
{
  void *node = get_page (cursor->table->pager, cursor->page_num);
  uint32_t num_cells = *leaf_node_num_cells (node);

  if (num_cells >= LEAF_NODE_MAX_CELLS)
    {
      printf ("Num of cells exceeds\n");
      return -1;
    }

  if (cursor->cell_num < num_cells)
    {
      for (uint32_t i = num_cells; i > cursor->cell_num; i--)
	{
	  memcpy (leaf_node_cell (node, i), leaf_node_cell (node, i - 1),
		  LEAF_NODE_CELL_SIZE);
	}
    }

  *(leaf_node_num_cells (node)) += 1;
  *(leaf_node_key (node, cursor->cell_num)) = key;
  serialize_row (value, leaf_node_value (node, cursor->cell_num));
  return 0;
}
