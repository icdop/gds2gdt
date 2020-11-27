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
