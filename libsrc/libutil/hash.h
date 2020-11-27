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

