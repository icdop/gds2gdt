#include "header.h"
#include "hash.h"

static HASH_ENTRY allocate_entry();
static void free_table();
static void free_entry();
static void rehash();

static HASH_TABLE free_tables = NIL(HASH_TABLE);
static HASH_ENTRY free_entries = NIL(HASH_ENTRY);
static long tables_allocated = 0L;
static long entries_allocated = 0L;

#define HASH_CMP_NUM(x,y) ((x) != (y))
#define HASH_NUM(x,size) (abs((long) (x)) % (size))
#define HASH_PTR(x,size) ((long) ((unsigned long) (x) >> 2) % size)
#define EQL(func,x,y)						\
  ((((func) == hash_cmp_num) || ((func) == hash_cmp_ptr)) ?		\
   (HASH_CMP_NUM((x),(y)) == 0) : ((*func)((x), (y)) == 0))
#define HASH(key,table) 						\
  (((table)->hash == hash_ptr) ? HASH_PTR((key), (table)->size) :	\
   ((table)->hash == hash_num) ? HASH_NUM((key), (table)->size) :	\
   (*(table)->hash)(key, (table)->size))


HASH_TABLE hash_create_with_parms(compare,hash,delete,size,dens,grow_fac)
int (*compare)();
int (*hash)();
void (*delete)();
int size;
int dens;
double grow_fac;
{
  int idx;
  HASH_TABLE tbl;

  if ((tbl = allocate(1, HASH_TABLE_STRUCT))==NULL) {
     fprintf(stderr,"Out of Memory in 'hash_create_with_parms()'.\n");
     exit(-1);
  }
  tbl->compare = compare;
  tbl->hash = hash;
  tbl->delete = delete;
  tbl->size = (size>1)?size:1;
  tbl->num_entries = 0;
  tbl->max_density = dens;
  tbl->grow_factor = grow_fac;
  tbl->next = NIL(HASH_TABLE);

  if ((tbl->table = allocate(size, HASH_ENTRY))==NULL) {
     fprintf(stderr,"Out of Memory in 'hash_create_with_parms()'.\n");
     exit(-1);
  }
  for (idx = 0; idx < size; idx++)
    tbl->table[idx] = NIL(HASH_ENTRY);
  tables_allocated++;

  return tbl;
}


HASH_TABLE hash_create(compare,hash,delete)
int (*compare)();
int (*hash)();
void (*delete)();
{
  return hash_create_with_parms(compare, hash, delete,
				HASH_DEF_INIT_TABLE_SIZE,
				HASH_DEF_MAX_DENSITY,
				HASH_DEF_GROW_FACTOR);
}


void hash_put(tbl,key,data)
HASH_TABLE tbl;
PTR key;
PTR data;
{
  HASH_ENTRY entry;
  int val;

  if (tbl==NULL) return;
  if ((tbl->num_entries / tbl->size) >= tbl->max_density)
    rehash(tbl);
  val = HASH(key, tbl);
  for (entry = tbl->table[val]; entry; entry = entry->next)
    if (EQL(tbl->compare, entry->key, key)) {
      if (tbl->delete) tbl->delete(entry->key,entry->data);
      entry->key = key;
      entry->data = data;
      return ;
    }
  entry = allocate_entry();
  entry->key = key;
  entry->data = data;
  entry->next = tbl->table[val];
  tbl->table[val] = entry;
  tbl->num_entries++;
}


BOOL hash_get(tbl,key,data)
HASH_TABLE tbl;
PTR key;
PTR *data;
{
  HASH_ENTRY entry;
  long val;

  if (tbl==NULL) return FALSE;
  val = HASH(key, tbl);
  for (entry = tbl->table[val]; entry; entry = entry->next)
    if (EQL(tbl->compare, entry->key, key)) {
      *data = entry->data;
      return TRUE;
    }
  return FALSE;
}

BOOL hash_delete(tbl,key)
HASH_TABLE tbl;
PTR key;
{
  HASH_ENTRY entry,*last;
  long val;

  val = HASH(key, tbl);
  for (entry = tbl->table[val],last=&(tbl->table[val]); entry; 
       last=&(entry->next),entry = entry->next)
    if (EQL(tbl->compare, entry->key, key)) {
      *last=entry->next;
      if (tbl->delete) tbl->delete(entry->key,entry->data);
      free_entry(entry); 
      tbl->num_entries--;
      return TRUE;
    }

  return FALSE;
}

void hash_each(tbl,func)
HASH_TABLE tbl;
void (*func)();
{
  HASH_ENTRY entry, next, *old_table;
  int idx, old_size, val;

  if (tbl==NULL) return;
  old_table = tbl->table;
  old_size = tbl->size;

  for (idx = 0; idx < old_size; idx++){ 
     for (entry = old_table[idx]; entry; entry = next) {
        next = entry->next;
        (*func)(entry->key,entry->data);        
     }
  }  
}



void hash_clear(tbl)
HASH_TABLE tbl;
{
  HASH_ENTRY entry, next;
  long idx;

  if (tbl==NULL) return;
  for (idx = 0; idx < tbl->size; idx++) {
    for (entry = tbl->table[idx]; entry != NIL(HASH_ENTRY); entry = next) {
      next = entry->next;
      if (tbl->delete) tbl->delete(entry->key,entry->data);
      free_entry(entry);
      tbl->num_entries--;
    }
    tbl->table[idx] = NIL(HASH_ENTRY);
  }
}


void hash_free(tbl)
HASH_TABLE tbl;
{
  if (tbl==NULL) return;
  hash_clear(tbl);
  free(tbl->table);
  free_table(tbl);
  free(tbl);
}


#define NUM_ENTRIES 100L
static HASH_ENTRY allocate_entry()
{
  HASH_ENTRY tmp;
  long idx;

  if (! free_entries) {
    if ((tmp = allocate(NUM_ENTRIES, HASH_ENTRY_STRUCT))==NULL) {
       fprintf(stderr,"Out of Memory in 'allocate_entry()'.\n");
       exit(-1);
    }
    for (idx = 0; idx < NUM_ENTRIES; idx++, tmp++) {
      tmp->next = free_entries;
      free_entries = tmp;
    }
  }

  tmp = free_entries;
  free_entries = tmp->next;
  entries_allocated++;

  tmp->key = NIL(PTR);
  tmp->data = NIL(PTR);

  return tmp;
}


static void free_table(tbl)
HASH_TABLE tbl;
{
  tbl->next = free_tables;
  free_tables = tbl;
  tables_allocated--;
}


static void free_entry(entry)
HASH_ENTRY entry;
{
  entry->next = free_entries;
  free_entries = entry;
  entries_allocated--;
}


static void rehash(tbl)
HASH_TABLE tbl;
{
  HASH_ENTRY entry, next, *old_table;
  int idx, old_size, val;

  old_table = tbl->table;
  old_size = tbl->size;

  if ((tbl->size = tbl->grow_factor * old_size) == old_size)
    tbl->size++;

  tbl->num_entries = 0;
  if ((tbl->table = allocate(tbl->size, HASH_ENTRY))==NULL) {
       fprintf(stderr,"Out of Memory in 'rehash()'.\n");
       exit(-1);
  }
  for (idx = 0; idx < tbl->size; idx++)
    tbl->table[idx] = NIL(HASH_ENTRY);

  for (idx = 0; idx < old_size; idx++) 
    for (entry = old_table[idx]; entry; entry = next) {
      next = entry->next;
      val = HASH(entry->key, tbl);
      entry->next = tbl->table[val];
      tbl->table[val] = entry;
      tbl->num_entries++;
    }
  free(old_table);  
}


int hash_str(str,size)
STR str;
int size;
{
  STR p;
  unsigned long h = 0, g;
  if (str)
  for (p = str; *p; p++) {
    if (!isalnum(*p)) continue;
    h = (h << 4) + toupper(*p);
    if (g = h & 0xF0000000) {
      h = h^(g >> 24);
      h = h^g;
    }
  }
  return h % size;
}



int hash_num(x,size)
int x;
int size;
{
  return HASH_NUM(x, size);
}


int hash_ptr(p,size)
PTR p;
int size;
{
  return HASH_PTR(p, size);
}


int hash_cmp_num(x,y)
int x, y;
{
  return HASH_CMP_NUM(x, y);
}


int hash_cmp_ptr(x,y)
PTR x, y;
{
  return HASH_CMP_NUM(x, y);
}

void hash_del_key(key,data)
PTR key,data;
{
  if (key) free(key);
}

void hash_del_data(key,data)
PTR key,data;
{
  if (data) free(data);
}

void hash_del_entry(key,data)
PTR key,data;
{
  if (key) free(key);
  if (data) free(data);
}

int hash_get_count(table,ptr) 
HASH_TABLE table;
PTR ptr;
{
  int *valueptr;
  if (!hash_get(table,(PTR)ptr,(PTR)&valueptr)) {
     *valueptr=0;
  }
  return *valueptr;
}

int hash_inc_count(table,ptr)
HASH_TABLE table;
PTR ptr;
{
  int *valueptr;
  if (!hash_get(table,(PTR)ptr,(PTR)&valueptr)) {
     valueptr=allocate(1,int);
     *valueptr=0;
     hash_put(table,(PTR)ptr,(PTR)valueptr);
  }
  (*valueptr)++;
  return *valueptr;
}

int hash_dec_count(table,ptr)
HASH_TABLE table;
PTR ptr;
{
  int *valueptr;
  if (!hash_get(table,(PTR)ptr,(PTR)&valueptr)) {
     valueptr=allocate(1,int);
     *valueptr=0;
     hash_put(table,(PTR)ptr,(PTR)valueptr);
  }
  (*valueptr)--;
  return *valueptr;
}

int *hash_put_count(table,ptr,value)
HASH_TABLE table;
PTR ptr;
int value;
{
  int *valueptr;
  if (!hash_get(table,(PTR)ptr,(PTR)&valueptr)) {
     valueptr=allocate(1,int);
     *valueptr=0;
     hash_put(table,(PTR)ptr,(PTR)valueptr);
  }
  *valueptr=value;
  return valueptr;
}

int hash_add_count(table,ptr,value)
HASH_TABLE table;
PTR ptr;
int value;
{
  int *valueptr;
  if (!hash_get(table,(PTR)ptr,(PTR)&valueptr)) {
     valueptr=allocate(1,int);
     *valueptr=0;
     hash_put(table,(PTR)ptr,(PTR)valueptr);
  }
  (*valueptr)+=value;
  return *valueptr;
}
