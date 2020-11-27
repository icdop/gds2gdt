#include "header.h"
#include "array.h"

#define NUM_ARRAY_DATA 10
static int (*array_compare)() = NULL;
static int compare_dataptr(ptr1,ptr2)
PTR *ptr1;
PTR *ptr2;
{
  return (int)(*array_compare)((*ptr1),(*ptr2));
}


static ARRAY allocate_array()
{
  ARRAY tmp;

  if ((tmp = allocate(1, ARRAY_STRUCT))==NULL) {
     fprintf(stderr,"Out of Memory in 'allocate_array()'.\n");
     exit(-1);
  }

  tmp->comp = NULL;
  tmp->size = 0L;
  tmp->dsize = 0L;
  tmp->data = NULL;
  
  return tmp;
}



ARRAY array_create_with_size(compare,dsize)
int (*compare)();
int dsize;
{
  ARRAY array;
  if (dsize<=0) dsize=0;
  array = allocate_array();
  array->comp = compare;
  array->dsize = dsize;
  array->data  = (PTR*) malloc(array->dsize*sizeof(PTR));
  if (array->data) bzero(array->data,array->dsize*sizeof(PTR));
  return array;
}

ARRAY array_create(compare)
int (*compare)();
{
  return array_create_with_size(compare,0);
}

void array_clear(array)
ARRAY array;
{
  if (array==NULL) return;
  array->size = 0L;
  if (array->data) bzero(array->data,array->dsize*sizeof(PTR));
}


void array_free(array)
ARRAY array;
{
  if (array==NULL) return;
  array_clear(array);
  free_nil(array->data);
  free(array);
}

PTR array_enqueue(array,ptr)
ARRAY array;
PTR   ptr;
{
  if (array==NULL) return NULL;
  if (array->size>=array->dsize) array_resize(array,array->size+1);
  array->data[array->size]=ptr;
  array->size++;
  return ptr;
}

ARRAY array_resort(array) 
ARRAY array;
{
  if ((array==NULL)||(array->data==NULL)) return NULL;
  if (array_compare=array->comp) {
     qsort(array->data,array->size,sizeof(PTR),compare_dataptr);
  }
  return array;  
}

ARRAY array_resize(array,dsize)
ARRAY array;
int dsize;
{
  int osize;
  if (array==NULL) return NULL;
  if (dsize<=array->dsize) return array;
  if (dsize<=0) return array;
  osize = array->dsize;
  array->dsize = ((dsize-1)/NUM_ARRAY_DATA+1) *NUM_ARRAY_DATA;
  if (array->data)
     array->data = (PTR*)realloc(array->data,array->dsize*sizeof(PTR));
  else
    array->data  = (PTR*) malloc(array->dsize*sizeof(PTR));
  bzero(&(array->data[osize]),(array->dsize-osize)*sizeof(PTR));
  return array;
}

BOOL array_get(array,index,ptr)
ARRAY array;
int index;
PTR *ptr;
{
  if (array==NULL) return FALSE;
  if ((index<=0)||(index>array->size)) return FALSE;
  if (array->data[index-1]==NULL) return FALSE;
  *ptr=array->data[index-1];
  return (*ptr!=NULL);
}

void array_put(array,index,data)
ARRAY array;
int index;
PTR data;
{
  if (array==NULL) return;
  if (index<=0) return;
  array_resize(array,index);
  if (index>array->size) array->size=index;
  array->data[index-1]=data;
  return;
}

ARRAY array_copy(dest,src)
ARRAY dest;
ARRAY src;
{
  dest->comp = src->comp;
  array_resize(dest,src->size);
  dest->size = src->size;
  bcopy(src->data,dest->data,src->size*sizeof(PTR));
  return dest;
}
