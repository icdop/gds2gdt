#if !defined(__libutil_ARRAY_H)
#define __libutil_ARRAY_H

typedef struct array {
  int (*comp)();
  long size;
  long dsize;
  PTR* data;
} ARRAY_STRUCT, *ARRAY;


#define sortDescend	-1
#define sortNone	0
#define sortAscend	1

#define array_first(a) ((a)->data)
#define array_last(a)  (&(a)->data[(a)->size-1])
#define array_next(p)  (p+1) 
#define array_prev(p)  (p-1)
#define array_data(p)  (*p)
#define array_empty(a) ((a)->size ? FALSE : TRUE)
#define array_size(a) ((a)->size)
#define array_for_all(a,x) { int _i_; ARRAY _a_ = a;\
        if (_a_) for (_i_=0; _i_<(_a_)->size; _i_++) { \
           x = (_a_)->data[_i_]; 
#define array_for_all_end }}
#define array_func_all(l,x,f) \
        {array_for_all(l,x){(f)(x);}array_for_all_end;}
#define array_free_nil(a)    {array_free(a); a=NIL(ARRAY);}
#define array_free_all(a,x,f)  {array_func_all(a,x,f);array_free_nil(a);}
#define array_clear_all(a,x,f) {array_func_all(a,x,f);array_clear(a);}

extern ARRAY array_create_with_size(int(),int);
extern ARRAY array_create(int());
extern ARRAY array_resort(ARRAY);
extern ARRAY array_resize(ARRAY,int);
extern PTR  array_enqueue(ARRAY,PTR);
extern void array_clear(ARRAY);
extern void array_free(ARRAY);
extern BOOL array_get(ARRAY,int,PTR*);
extern void array_put(ARRAY,int,PTR);
extern ARRAY array_copy(ARRAY,ARRAY);
#endif

