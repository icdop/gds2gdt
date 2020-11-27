/*
 *
 * Package : GenCAD Utility Header
 * Date    : 11/03/19
 *
 */
#ifndef __libutil_HEADER_H_
#define __libutil_HEADER_H_
#undef  PKGNAME
#undef  OKGVERN

#define PKGNAME_UTIL  "libutil"
#define PKGVERN_UTIL  "V2017.9"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <ctype.h>
#include <alloca.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

#define MAX_FILENAME_LEN 1024
#define MAX_OUTBUF_SIZE   4096 


typedef char *STR;
typedef void *PTR;
typedef char BOOL;

#define FALSE 0
#define TRUE  1

#define NIL(t) ((t) 0)
#define abs(x) (((x) < 0) ? -(x) : (x))
#define minimum(x,y) (((x) < (y)) ? (x) : (y))
#define maximum(x,y) (((x) > (y)) ? (x) : (y))

#define allocate(n, els) (els *) calloc(n, sizeof(els))
#define free_nil(x) {if(x){free(x);(x)=NULL;}}

#define flag_setbit(v,b) (v |= b)
#define flag_clrbit(v,b) (v &= ~(b))
#define flag_chkbit(v,b) ((v&b) != 0)

#endif
#ifndef __libutil_LINK_H
#define __libutil_LINK_H

typedef void *LINK;

#define link_data(e) (e)
#define link_next(e) (e)->next
#define link_empty(e) (e==NULL)
#define link_enqueue(l,e) {(e)->next=l;l=e;}
#define link_append(l1,l2,e) { \
	if (e=l1) {  while(e->next){e=e->next;} e->next=l2; } \
	else { l1=l2; }  l2=NULL; }

#define link_for_all(l,e) \
	if (l) { LINK _tmp_; for(e=l;e;e=_tmp_) { _tmp_=(e)->next;
#define link_for_all_end }}
#define link_func_all(l,e,f) \
        if (l) { LINK _tmp_; for(e=l;e;e=_tmp_){_tmp_=(e)->next;(f)(e);} }
#define link_clear(l)       {l=NULL;}
#define link_free(l)        {link_clear(l);}
#define link_free_nil(l)    {link_free(l);l=NULL;}
#define link_free_all(l,e,f)  {link_func_all(l,e,f);link_free_nil(l);}
#define link_clear_all(l,e,f) {link_func_all(l,e,f);link_clear(l);}

#endif
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

#if !defined(__libutil_HASH_H)
#define __libutil_HASH_H

typedef struct hash_table {
  int size,
      num_entries,
      max_density;
  double grow_factor;
  int (* hash)();
  int (* compare)();
  void (* delete)();
  struct hash_entry **table;
  struct hash_table *next;
} HASH_TABLE_STRUCT, *HASH_TABLE;

typedef struct hash_entry {
  PTR key;
  PTR data;
  struct hash_entry *next;
} HASH_ENTRY_STRUCT, *HASH_ENTRY;

extern HASH_TABLE hash_create();
extern HASH_TABLE hash_create_with_parms();
extern void hash_put();
extern BOOL hash_get();
extern BOOL hash_delete();
extern void hash_each();
extern void hash_clear();
extern void hash_free();
extern int hash_str();
extern int hash_ptr();
extern int hash_num();
extern int hash_cmp_num();
extern int hash_cmp_ptr();

extern void hash_del_key();
extern void hash_del_data();
extern void hash_del_entry();

#define HASH_DEF_MAX_DENSITY 4
#define HASH_DEF_INIT_TABLE_SIZE 131
#define HASH_DEF_GROW_FACTOR 2.0
#define hash_key(e) (e)->key
#define hash_data(e) (e)->data
#define hash_entries(t) (t?(t)->num_entries:0)
#define hash_for_all(t,x) \
        {int _i_; HASH_TABLE _t_=t; HASH_ENTRY _e_,_en_; \
        if (_t_) for(_i_=0;_i_<(_t_)->size;_i_++) \
               for(_e_=(_t_)->table[_i_];_e_;_e_=_en_) { _en_=_e_->next; x=hash_data(_e_);
#define hash_for_all_pair(t,x,y) \
        {int _i_; HASH_TABLE _t_=t; HASH_ENTRY _e_,_en_; \
        if (_t_) for(_i_=0;_i_<(_t_)->size;_i_++) \
               for(_e_=(_t_)->table[_i_];_e_;_e_=_en_) { _en_=_e_->next; x=hash_key(_e_); y=hash_data(_e_); 
#define hash_for_all_entry(t,e) \
        {int _i_; HASH_TABLE _t_=t;  HASH_ENTRY _en_; \
        if (_t_) for(_i_=0;_i_<(_t_)->size;_i_++) \
               for(e=(_t_)->table[_i_];e;e=_en_) { _en_=(e)->next;
#define hash_for_all_data(t,x) \
        {int _i_; HASH_TABLE _t_=t;  HASH_ENTRY _e_,_en_; \
        if (_t_) for(_i_=0;_i_<(_t_)->size;_i_++) \
               for(_e_=(_t_)->table[_i_];_e_;_e_=_en_) { _en_=_e_->next; x=hash_data(_e_);
#define hash_for_all_key(t,x) \
        {int _i_; HASH_TABLE _t_=t;  HASH_ENTRY _e_,_en_; \
        if (_t_) for(_i_=0;_i_<(_t_)->size;_i_++) \
               for(_e_=(_t_)->table[_i_];_e_;_e_=_en_) { _en_=_e_->next; x=hash_key(_e_);
#define hash_for_all_end  }}
#define hash_func_all(t,x,f) \
       {  hash_for_all(t,x){ (f)(x); } hash_for_all_end;}
#define hash_func_all_key(t,x,f) \
       {  hash_for_all_key(t,x){ (f)(x); } hash_for_all_end;}
#define hash_func_all_entry(t,f) \
       { HASH_ENTRY _e_; \
       hash_for_all_entry(t,_e_){ \
       (f)(_e_->key,_e_->data); \
       } hash_for_all_end;}
#define hash_free_nil(t) {hash_free(t); (t)=NIL(HASH_TABLE);}
#define hash_free_all(t,x,f) {hash_func_all(t,x,f);hash_free_nil(t);}
#define hash_free_all_key(t,x,f) {hash_func_all_key(t,x,f);hash_free_nil(t);}
#define hash_free_all_entry(t,f) {hash_func_all_entry(t,f);hash_free_nil(t);}
#define hash_clear_all(t,x,f) {hash_func_all(t,x,f); hash_clear(t);}
#define hash_clear_all_key(t,x,f) {hash_func_all_key(t,x,f);hash_clear(t);}
#define hash_clear_all_entry(t,f) {hash_func_all_entry(t,f);hash_clear(t);}

int hash_get_count();
int hash_add_count();
int hash_inc_count();
int hash_dec_count();
int *hash_put_count();
#endif

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

#if !defined(__libutil_SORT_H)
#define __libutil_SORT_H

ARRAY sort_table();
ARRAY sort_table_data();
ARRAY sort_table_key();
int   hash_cmp_count();

#endif
#if !defined(__libutil_STR_H)
#define __libutil_STR_H
#include <string.h>
#include <stdlib.h>

#define  MATCH_LINE      0
#define  MATCH_STR       1
#define  MATCH_CHAR      2
#define  MATCH_WORD      3
#define  MATCH_MULTIPLE  4
#define  MATCH_NORM_MASK 15
#define  DELETE_REMARK   16
#define  NO_END_MARK	 32
#define  MATCH_RELOAD    64

#define  WHITE_SPACE_MASK " \t\n\r"
/*
 ************************************************************************
 *   Function Name: 
 *     char *read_lines();
 *     
 *   Arguments:
 *     FILE* file_ptr 
 *     int   match_type     // define match type
 *     char* mask_str       // definne mask str
 *     
 *   Description:
 *     (1) read one line at a time
 *         match_type = MATCH_LINE
 *         mask_str : by pass (default set to "\n")
 *         
 *     (2) specify terminating string
 *         match_type = MATCH_STR
 *         mask_str : terminating string
 *         
 *     (3) specify terminate char mask
 *         match_type = MATCH_CHAR
 *         mask_str : terminating char set
 *   
 *     (4) read multiple lines and specify connecting char mask
 *         match_type = MATCH_MULTIPLE
 *         mask_str : connecting char set on the end of one line
 *   
 *   Remarks:
 *     (1) if the line only contains white space, then the fgets
 *         will return a empty string, so as this function.
 *         
 **************************************************************************
 */
char *read_lines();

/*
 *********************************
 *  char *lowercase(char*);
 *  char *uppercase(char*);
 *  char *num2str(char*);
 *********************************
 */
char *lowercase();
char *uppercase();
char *num2str();

/*
 *********************************
 * DYNAMIC STRING ASSIGNMENT
 *  char *copy_str(char **dest_str, char *src_str);
 *  char *join_str(cahr **dest_str, char *head_str, char *tail_str);
 *  char *append_str(char **dest_str, char *tail_str);
 *  char *free_str(char **dest_str);
 *********************************
 */

char *copy_str();
char *join_str();
char *append_str();
BOOL free_str();
char *clear_white_space();
char *clear_remark_str();

/*
 ***********************************************
 *  STRING MACRO DEFINITON
 ***********************************************
 */
 
#define string_enqueue(s,p) {append_str(&s,p);append_str(&s," ");}
#define string_enqueue_sep(s,p,c) {append_str(&s,p);append_str(&s,c);}
#define string_for_all(s,p)  \
        if (s) {static char *_tmp1_=NULL; char *_tmp2_; \
           copy_str(&_tmp1_,s); _tmp2_=_tmp1_-1; \
           while(p=_tmp2_) { p++; \
            if (_tmp2_=strpbrk(p,WHITE_SPACE_MASK)) *_tmp2_=0; if (strlen(p)){
#define string_for_all_str(s,p,c)  \
        if (s) {static char *_tmp1_=NULL; char *_tmp2_; \
           copy_str(&_tmp1_,s); _tmp2_=_tmp1_-strlen(c); \
           while(p=_tmp2_) { p+=strlen(c); \
            if (_tmp2_=strstr(p,c)) *_tmp2_=0; if (strlen(p)) { 
#define string_for_all_sep(s,p,c)  \
        if (s) {static char *_tmp1_=NULL; char *_tmp2_; \
           copy_str(&_tmp1_,s); _tmp2_=_tmp1_-1; \
           while(p=_tmp2_) { p++; \
            if (_tmp2_=strpbrk(p,c)) *_tmp2_=0; if (strlen(p)) { 
#define string_for_all_spc(s,p) string_for_all_sep(s,p,WHITE_SPACE_MASK)
#define string_enqueue_spc(s,p) string_enqueue_sep(s,p," ")
#define string_for_all_end   } }  }
#define string_free(s)       {if (s) free(s);}
#define string_free_nil(s)   {string_free(s); s=NULL;}

#endif
#if !defined(__libutil_FILE_H)
#define __libutil_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern FILE *open_file();
extern char *extract_filename();
extern char *check_filename();
extern int  backup_exist_file();

#endif
#if !defined(__libutil_STATUS_H)
#define __libutil_STATUS_H

extern void util_print_cpu_stats();
extern long until_cpu_time();
extern long elapse_time();
extern char *get_time();
extern char datetime_str[];
extern char *system_status();

#endif
#if !defined(__libutil_IOFUNC_H)
#define __libutil_IOFUNC_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern int set_syslog_file();
extern int open_syslog_file();
extern int close_syslog_file();
extern int set_sysout_fp(FILE *);
extern int io_log( const char* format, ...);
extern int io_printf( const char* format, ...);
extern int io_error( const char* format, ...);
extern int io_warning( const char* format, ...);
extern int io_summary();
extern int io_error_count();
extern int io_warning_count();
extern int io_print_cpu_stats();
#endif
