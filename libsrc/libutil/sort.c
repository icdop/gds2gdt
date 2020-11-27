#include "header.h"
#include "hash.h"
#include "array.h"
#include "sort.h"

static ARRAY array=NIL(ARRAY);
static HASH_TABLE tbl=NIL(HASH_TABLE);

int hash_cmp_count(ptr1,ptr2) 
PTR ptr1,ptr2;
{
  return (hash_get_count(tbl,ptr2))-(hash_get_count(tbl,ptr1));
}

ARRAY sort_table_data(table,compare)
HASH_TABLE table;
int (*compare)();
{
    void *data;

    tbl=table;
    if (array==NULL) {
       array = array_create_with_size(compare,hash_entries(table));
    } else {
       array->comp=compare;    
       array_resize(array,hash_entries(table));
    }
    array_clear(array);
    hash_for_all_data(table,data) {
      array_enqueue(array,data);
    } hash_for_all_end;
    array_resort(array);
    return array;
}

ARRAY sort_table_key(table,compare)
HASH_TABLE table;
int (*compare)();
{
    void *key;

    tbl=table;
    if (array==NULL) {
       array = array_create_with_size(compare,0);
    } else {
       array->comp=compare;    
    }
    array_resize(array,hash_entries(table));
    array_clear(array);
    hash_for_all_key(table,key) {
      array_enqueue(array,key);
    } hash_for_all_end;
    array_resort(array);
    return array;
}

ARRAY sort_table(table)
HASH_TABLE table;
{
    void *data;

    tbl=table;
    sort_table_key(table,table->compare);
    array_for_all(array,data) {
      hash_get(table,(PTR)data,(PTR*)&data);
    } array_for_all_end;
    return array;
}
