#if !defined(__libutil_LIST_H)
#define __libutil_LIST_H

typedef struct list {
  int (*comp)();
  long size;
  struct list_el *first;
  struct list_el *last;
  struct list *next;
} LIST_STRUCT, *LIST;

typedef struct list_el {
  PTR data;
  struct list_el *next; 
  struct list_el *prev;
} LIST_EL_STRUCT, *LIST_EL;

#define sortDescend	-1
#define sortNone	0
#define sortAscend	1

#define list_first(l) (LIST_EL)((l)->first)
#define list_last(l) (LIST_EL)((l)->last)
#define list_next(e) (LIST_EL)((e)->next)
#define list_prev(e) (LIST_EL)((e)->prev)
#define list_data(e) (e)->data
#define list_data_replace(e,i) (e)->data = (i)
#define list_size(l) ((l)?(l)->size:0)
#define list_empty(l) (list_size(l) ? FALSE : TRUE)
#define list_for_all_el(l,e) { LIST_EL _en_; LIST _l_=l; \
        if (_l_) for (e = (_l_)->first; e; e = _en_){ _en_=(e)->next;
#define list_for_all(l,x) { LIST_EL _e_,_en_; LIST _l_=l; \
        if (_l_) for (_e_ = (_l_)->first; _e_; _e_ = _en_) { \
        _en_= _e_->next; x = list_data(_e_);
#define list_for_all_end }}

#define list_rev_all_el(l,e) { LIST_EL _en_; LIST _l_=l; \
        if (_l_) for (e = (_l_)->last; e; e = _en_){ _en_=(e)->prev;
#define list_rev_all(_l_,x) { LIST_EL _e_,_en_; LIST _l_=l; \
        if (_l_) for (_e_ = (_l_)->last; _e_; _e_ = _en_) { \
        _en_= _e_->prev; x = list_data(_e_);
#define list_rev_all_end }}

#define list_func_all(l,x,f) \
        {list_for_all(l,x){(f)(x);}list_for_all_end;}
#define list_free_nil(l)    {list_free(l); l=NIL(LIST);}
#define list_free_all(l,x,f)  {list_func_all(l,x,f);list_free_nil(l);}
#define list_clear_all(l,x,f) {list_func_all(l,x,f);list_clear(l);}

#define list_shift(l)   list_delete_first(l)
#define list_pop(l)     list_delete_last(l)
#define push_queue(l,x) list_enqueue(l,x)
#define pop_queue(l)    list_delete_first(l)
#define push_stack(l,x) list_enqueue(l,x)
#define pop_stack(l)    list_delete_last(l)
#define queue_free(l)	list_free(l);
#define stack_free()    list_free(l);

extern LIST list_create();
extern LIST queue_create();
extern LIST stack_create();
extern LIST_EL list_enqueue();
extern LIST list_append();
extern PTR  list_delete_first();
extern PTR  list_delete_last();
extern PTR  list_delete_item();
extern void list_clear();
extern void list_free();
extern LIST_EL list_push();
extern LIST_EL list_unshift();

#endif

