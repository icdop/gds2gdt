#if !defined(__libutil_FARRAY_H)
#define __libutil_FARRAY_H

typedef struct farray {
  int (*comp)();
  long size;
  long dsize;
  double* data;
} FARRAY_STRUCT, *FARRAY;


#define farray_first(a) ((a)->data)
#define farray_last(a)  (&(a)->data[(a)->size-1])
#define farray_next(p)  (p+1) 
#define farray_prev(p)  (p-1)
#define farray_data(p)  (*p)
#define farray_empty(a) ((a)->size ? FALSE : TRUE)
#define farray_size(a) ((a)->size)
#define farray_for_all(a,x) { int _i_; \
        if (a) for (_i_=0; _i_<(a)->size; _i_++) { \
           x = (a)->data[_i_]; 
#define farray_for_all_end }}
#define farray_func_all(l,x,f) \
        {farray_for_all(l,x){(f)(x);}farray_for_all_end;}
#define farray_free_nil(a)    {farray_free(a); a=NIL(FARRAY);}
#define farray_free_all(a,x,f)  {farray_func_all(a,x,f);farray_free_nil(a);}
#define farray_clear_all(a,x,f) {farray_func_all(a,x,f);farray_clear(a);}

extern FARRAY farray_create_with_size(int(),int);
extern FARRAY farray_create(int());
extern FARRAY farray_resort(FARRAY);
extern FARRAY farray_resize(FARRAY,int);
extern FARRAY farray_copy(FARRAY,FARRAY);
extern FARRAY farray_subset(FARRAY,FARRAY,int,int);
extern double  farray_enqueue(FARRAY,double);
extern void farray_clear(FARRAY);
extern void farray_free(FARRAY);
extern BOOL farray_get(FARRAY,int,double*);
extern void farray_put(FARRAY,int,double);
extern FARRAY farray_sorting(FARRAY,int());
extern int  def_farray_comp(double*,double*);
#endif

