#include "header.h"
#include "list.h"

static LIST allocate_list();
static LIST_EL allocate_list_el();
static void free_list();
static void free_list_el();
static int def_comp();

static LIST free_lists = NIL(LIST);
static LIST_EL free_list_els = NIL(LIST_EL);
static long lists_allocated = 0L;
static long list_els_allocated = 0L;

LIST list_create(compare)
int (*compare)();
{
  LIST list;
  list = allocate_list();
  list->comp=compare;
  return list;
}
LIST_EL list_unshift(list,data)
LIST list;
PTR data;
{
  LIST_EL el;
  if (list==NULL) return NULL;
  el = allocate_list_el();
  el->data = data;
  if (list->last == NIL(LIST_EL)) {
    list->first = list->last = el;
  } else {
    list->first->prev = el;
    el->next = list->first;
    list->first = el;
  }
  list->size++;
  return el;
}

LIST_EL list_push(list,data)
LIST list;
PTR data;
{
  LIST_EL el;
  if (list==NULL) return NULL;
  el = allocate_list_el();
  el->data = data;
  if (list->last == NIL(LIST_EL)) {
    list->first = list->last = el;
  } else {
    list->last->next= el;
    el->prev = list->last;
    list->last = el;
  }
  list->size++;
  return el;
}

LIST_EL list_enqueue(list,data)
LIST list;
PTR data;
{
  LIST_EL el,el2;
  
  if (list==NULL) return NULL;
  el = allocate_list_el();
  el->data = data;
  if (list->last == NIL(LIST_EL)) {
    list->first = list->last = el;
  } else if ((list->comp==NULL)||(list->comp)(el->data,list->last->data)>=0) {
    list->last->next= el;
    el->prev = list->last;
    list->last = el;
  } else if ((list->comp)(el->data,list->first->data)<0) {
    list->first->prev = el;
    el->next = list->first;
    list->first = el;
  } else {
    for(el2 = list->first; el2; el2 = el2->next) {
      if ((list->comp)(el->data,el2->data)<0) {
         el->prev = el2->prev;
         el2->prev = el;
         el->next = el2;
         el->prev->next = el;
         list->size++;
         return el;
      }   
    }
    fprintf(stderr,"Error! Internal List Structure Error\n");
    exit(-1);
  }
  list->size++;
  return el;
}

/* Added by Albert Li 05/04/97 */
void list_delete_elem(list,el)
LIST list;
LIST_EL el;
{
   if (list==NULL) return;
   if (el == list->first) {
      if (list->first = el->next)
         list->first->prev = NIL(LIST_EL);
      else
         list->last = NIL(LIST_EL);
   } else if (el == list->last) {
      if (list->last  = el->prev)
         list->last->next = NIL(LIST_EL);
      else
         list->first = NIL(LIST_EL);
   } else {
     el->next->prev = el->prev;
     el->prev->next = el->next;
   }
  free_list_el(el);
  list->size--;
  return;
}

PTR list_delete_item(list,item)
LIST list;
PTR item;
{
  LIST_EL el,tmp;
  PTR data=NIL(PTR);
  if (list)
  for(el=list->first;el; el=tmp) {
    tmp  = el->next;
    if (item==el->data) {
       data = el->data;
       list_delete_elem(list,el);
    }
  }
  return data;
}

/* Added by Albert Li 05/04/97 */
PTR list_delete_first(list)
LIST list;
{
  LIST_EL el;
  PTR data=NIL(PTR);
  if (list)
  if (el=list->first) {
    data=el->data;
    list_delete_elem(list,el);
  }
  return data;
}

/* Added by Albert Li 05/10/96 */
PTR list_delete_last(list)
LIST list;
{
  LIST_EL el;
  PTR data=NIL(PTR);
  if (list)
  if (el=list->last) {
    data=el->data;
    list_delete_elem(list,el);
  }
  return data;
}

LIST list_append(l1,l2)
LIST l1, l2;
{
  LIST_EL el;
  if (l1==NULL) return NULL;
  if (l2)
  for (el=l2->first; el; el=el->next) {
     list_enqueue(l1,el->data);
  }
  return l1;
}

LIST list_merge(l1,l2)
LIST l1, l2;
{
  if (l1==NULL) return NULL;
  if (l2==NULL) return l1;
  if (list_empty(l1)) {
    if (! list_empty(l2)) {
      l1->size = l2->size;
      l1->first = l2->first;
      l1->last = l2->last;
      l2->first = l2->last = NIL(LIST_EL);
    }
  } else {
    if (! list_empty(l2)) {
      l1->size = l1->size + l2->size;
      l1->last->next = l2->first;
      l2->first->prev = l1->last;
      l1->last = l2->last;
      l2->size = 0;
      l2->first = l2->last = NIL(LIST_EL);
    }
  }
  return l1;
}


void list_clear(list)
LIST list;
{
  LIST_EL el, el1;
  
  if (list==NULL) return;
  for (el = list->first; el; el = el1) {
    el1 = el->next;
    free_list_el(el);
  }

  list->first = list->last = NIL(LIST_EL);
  list->size = 0L;
}


void list_free(list)
LIST list;
{
  if (list==NULL) return;
  list_clear(list);
  free_list(list);
}


#define NUM_LISTS 10L
static LIST allocate_list()
{
  LIST tmp;
  long idx;

  if (! free_lists) {
    if ((tmp = allocate(NUM_LISTS, LIST_STRUCT))==NULL) {
       fprintf(stderr,"Out of Memory in 'allocate_list()'.\n");
       exit(-1);
    }
    for (idx = 0; idx < NUM_LISTS; idx++, tmp++) {
      tmp->next = free_lists;
      free_lists = tmp;
    }
  }

  tmp = free_lists;
  free_lists = tmp->next;
  lists_allocated++;
  
  tmp->comp = NULL;
  tmp->size = 0L;
  tmp->first = tmp->last = NIL(LIST_EL);
  tmp->next = NIL(LIST);

  return tmp;
}


#define NUM_LIST_ELS 50L
static LIST_EL allocate_list_el()
{
  LIST_EL tmp;
  long idx;

  if (! free_list_els) {
    if ((tmp = allocate(NUM_LIST_ELS, LIST_EL_STRUCT))==NULL){
       fprintf(stderr,"Out of Memory in 'allocate_list_el()'.\n");
       exit(-1);
    }
    for (idx = 0; idx < NUM_LIST_ELS; idx++, tmp++) {
      tmp->next = free_list_els;
      free_list_els = tmp;
    }
  }

  tmp = free_list_els;
  free_list_els = tmp->next;
  list_els_allocated++;

  tmp->data = NIL(PTR);
  tmp->next = tmp->prev = NIL(LIST_EL);

  return tmp;
}


static void free_list(list)
LIST list;
{
  list->next = free_lists;
  free_lists = list;
  lists_allocated--;
}


static void free_list_el(el)
LIST_EL el;
{
  el->next = free_list_els;
  free_list_els = el;
  list_els_allocated--;
}


static int def_comp(d1,d2)
PTR d1, d2;
{
  return 1;
}


LIST queue_create(comp_func)
int (*comp_func)();
{
  LIST list = allocate_list();
 
  list->comp = comp_func;
  return list;
}


LIST stack_create(comp_func)
int (*comp_func)();
{
  LIST list = allocate_list();

  list->comp = comp_func;
  return list;
}

