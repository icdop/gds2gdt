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
