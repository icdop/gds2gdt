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
