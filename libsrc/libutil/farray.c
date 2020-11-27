#include "header.h"
#include "farray.h"

#define NUM_FARRAY_DATA 10
static FARRAY allocate_farray();

int def_farray_comp(val1,val2)
double *val1,*val2;
{
  return (int)(*val1-*val2);
}

static FARRAY allocate_farray()
{
  FARRAY tmp;
  long idx;

  if ((tmp = allocate(1, FARRAY_STRUCT))==NULL) {
     fprintf(stderr,"Out of Memory in 'allocate_farray()'.\n");
     exit(-1);
  }

  tmp->comp = NULL;
  tmp->size = 0L;
  tmp->dsize = 0L;
  tmp->data = NULL;
  
  return tmp;
}



FARRAY farray_create_with_size(compare,size)
int (*compare)();
int size;
{
  FARRAY farray;
  farray = allocate_farray();
  farray->comp = compare;
  farray->dsize = size;
  farray->data  = (double*) malloc(farray->dsize*sizeof(double));
  if (farray->data) bzero(farray->data,farray->dsize*sizeof(double));
  return farray;
}

FARRAY farray_create(compare)
int (*compare)();
{
  return farray_create_with_size(compare,0);
}

void farray_clear(farray)
FARRAY farray;
{
  if (farray==NULL) return;
  farray->size = 0L;
  if (farray->data) bzero(farray->data,farray->dsize*sizeof(double));
}


void farray_free(farray)
FARRAY farray;
{
  if (farray==NULL) return;
  free_nil(farray->data);
  free(farray);
}

double farray_enqueue(farray,datum)
FARRAY farray;
double   datum;
{
  if (farray==NULL) return 0.0;
  if (farray->size>=farray->dsize) farray_resize(farray,farray->size+1);
  farray->data[farray->size]=datum;
  farray->size++;
  return datum;
}

FARRAY farray_resort(farray) 
FARRAY farray;
{
  if (farray==NULL) return NULL;
  if (farray->comp) qsort(farray->data,farray->size,sizeof(double),farray->comp);
  return farray;  
}

FARRAY farray_sorting(farray,comp_func) 
FARRAY farray;
int (*comp_func)();
{
  if (farray==NULL) return NULL;
  if (comp_func) qsort(farray->data,farray->size,sizeof(double),comp_func);
  return farray;  
}

FARRAY farray_resize(farray,dsize)
FARRAY farray;
int    dsize;
{
  int osize;
  if (farray==NULL) return NULL;
  if (dsize<=farray->dsize) return farray;
  if (dsize<=0) return farray;
  osize = farray->dsize;
  farray->dsize = ((dsize-1)/NUM_FARRAY_DATA+1) *NUM_FARRAY_DATA;
  farray->data = (double*)realloc(farray->data,farray->dsize*sizeof(double));
  bzero(&(farray->data[osize]),(farray->dsize-osize)*sizeof(double));
  return farray;
}

BOOL farray_get(farray,index,ptr)
FARRAY farray;
int index;
double *ptr;
{
  if (farray==NULL) return FALSE;
  if ((index<=0)||(index>farray->size)) return FALSE;
  *ptr=farray->data[index-1];
  return (*ptr);
}

void farray_put(farray,index,datum)
FARRAY farray;
int index;
double datum;
{
  if (farray==NULL) return;
  if (index<=0) return;
  farray_resize(farray,index);
  if (index>farray->size) farray->size=index;
  farray->data[index-1]=datum;
  return;
}

FARRAY farray_copy(farray1,farray2)
FARRAY farray1,farray2;
{
  if (farray1==NULL) farray1=farray_create(NULL);
  else               farray_clear(farray1);
  farray_resize(farray1,farray2->size);
  farray1->comp=farray2->comp;
  farray1->size=farray2->size;
  memcpy(farray1->data,farray2->data,farray1->size*sizeof(double));
  return farray1;
}

FARRAY farray_subset(farray1,farray2,begin,end)
FARRAY farray1,farray2;
int begin,end;
{
  int size;
  if (farray1==NULL) farray1=farray_create(NULL);
  else               farray_clear(farray1);
  if (begin>farray2->size) begin=farray2->size;
  if (begin<=0) begin=1;
  if (end>farray2->size) end=farray2->size;
  if (end<=0) end=1;
  size = end-begin+1;
  if (size<0) size=0;
  farray1->comp=farray2->comp;
  farray_resize(farray1,size);
  farray1->size=size;
  memcpy(farray1->data,farray2->data+begin-1,size*sizeof(double));
  return farray1;
}
