#include "header.h"
#include "str.h"

#define MAX_LINE_SIZE 1024

static char *InputBuffer=NULL;
static int  BufferSize=MAX_LINE_SIZE;

char *read_lines(fp,type,endmask)
FILE *fp;
int type;
char *endmask;
{
  static long position=0;
  static FILE *prefp=NULL;
  static char InputStr[MAX_LINE_SIZE+1];
  char *ptr,*endptr;
  BOOL ReadFlag;
  BOOL NoEndMarkFlag;
  BOOL DeleteRemFlag;
  int  totalstrlen,masklen;
  int  index;
  
  if (InputBuffer==NULL) { 
     InputBuffer= (char*)malloc(BufferSize+1); 
  }
  if ((fp==0)||(fp!=prefp)){ position=0; prefp=fp;}
  if (type & MATCH_RELOAD) { 
     fseek(fp,position,0);
  } else {
     position=ftell(fp);
  }
  NoEndMarkFlag=type & NO_END_MARK;
  DeleteRemFlag=type & DELETE_REMARK;
  type = type & MATCH_NORM_MASK;
  if ((type==MATCH_LINE)||(endmask==NULL)) {
     NoEndMarkFlag = TRUE;
     endmask = "\n";
  }
  masklen=strlen(endmask);
  ReadFlag=TRUE;
  InputBuffer[0]='\0';
  InputStr[MAX_LINE_SIZE]='\0';
  while (!feof(fp) && ReadFlag) {
    *InputStr=0;
    fgets(InputStr,MAX_LINE_SIZE,fp);
    index=strlen(InputBuffer);
    totalstrlen=strlen(InputStr)+index;
    if (totalstrlen>=BufferSize) {
       BufferSize += MAX_LINE_SIZE;
       InputBuffer=(char*) realloc(InputBuffer,BufferSize+1);
    }
    if (index) {
       strcpy(InputBuffer+index,InputStr);
    } else {
       for (ptr=InputStr; *ptr && isspace(*ptr); ptr++);
       strcpy(InputBuffer,ptr);
    }
    if (DeleteRemFlag && strchr(InputStr,'/')) clear_remark_str(InputBuffer);
    switch(type) {
      case MATCH_CHAR:
        if (endptr=strpbrk(InputBuffer+index,endmask)) 
           ptr = endptr+1;
        break;
      case MATCH_WORD:
        while (endptr=strstr(InputBuffer+index,endmask)) {
           ptr = endptr+masklen;
           index = ptr-InputBuffer;
           if (isspace(*ptr)&&((endptr==InputBuffer)||isspace(*(endptr-1)))) {
               ptr++;
               break;
           }
        }
        break;
      case MATCH_STR:
        if (endptr=strstr(InputBuffer+index,endmask)) 
           ptr = endptr+masklen;
        break;
      default: /* MATCH_LINE */
        if (endptr=strchr(InputBuffer+index,'\n')) 
           ptr = endptr+1;
        break;
    }
    if (endptr) {
       if (*ptr) {
          *ptr=0;
          for(ptr=InputStr; *ptr && isspace(*ptr); ptr++);
          if (ptr=strstr(ptr,endptr))
             fseek(fp,ptr-InputStr-strlen(InputStr)+strlen(endptr),1);
       } 
       ReadFlag = FALSE;
       if (type==MATCH_MULTIPLE) {
          ptr = endptr-1;
          if ((ptr>=InputBuffer)&&strchr(endmask,*ptr)) {
             *ptr=0;
             ReadFlag = TRUE;
          }
       } 
    }
  }
  if  (NoEndMarkFlag && endptr &&(strcmp(endptr,endmask)==0)) *endptr=0;
  return InputBuffer;
}

/*
 *********************************
 *  char *lowercase(char*);
 *  char *uppercase(char*);
 *  char *num2str(char*);
 *********************************
 */

char *lowercase(s)
char *s;
{
    char *t;

    if (s==NULL) return NULL;
    for (t=s ; *t; t++) *t = tolower(*t);
    return(s);
}

 
char *uppercase(s)
char *s;
{
    char *t;

    if (s==NULL) return NULL;
    for (t=s; *t; t++) *t = toupper(*t);
    return(s);
}

char* num2str(num)
double num;
{
  static char buf[40];
  sprintf(buf,"%f",num);
  return buf;
}

/*
 *********************************
 * DYNAMIC STRING ASSIGNMENT
 *  char *copy_str(char **dest_str, char *src_str);
 *  char *join_str(cahr **dest_str, char *head_str, char *tail_str);
 *  char *append_str(char **dest_str, char *tail_str);
 *  char *free_str(char **dest_str);
 *********************************
 */

char *copy_str(dest,src)
char **dest;
char *src;
{
  char *temp;
  if (dest==NULL) return NULL;
  temp = *dest;
  *dest = (src) ? strdup(src):NULL;
  if (temp) free(temp);
  return (*dest);
}

char *join_str(dest,src1,src2)
char **dest;
char *src1;
char *src2;
{
  char *temp;
  if (dest==NULL) return NULL;
  temp = *dest;
  if (src1==NULL)
     *dest = (src2) ? strdup(src2):NULL;
  else if (src2==NULL)
     *dest = strdup(src1);
  else {   
     *dest = malloc(strlen(src1)+strlen(src2)+1);
     strcpy(*dest,src1);
     strcat(*dest,src2);
     }
  if (temp) free(temp);
  return (*dest);
}

char *append_str(dest,src)
char **dest;
char *src;
{
  char *temp;
  if (dest==NULL) return NULL;
  if ((src==NULL)||(strlen(src)==0)) return *dest;
  temp = *dest;
  if (temp==NULL) return *dest=strdup(src);
  *dest = malloc(strlen(temp)+strlen(src)+1);
  strcpy(*dest,temp);
  strcat(*dest,src);
  if (temp) free(temp);
  return (*dest);
}

BOOL free_str(dest)
char **dest;
{
  if (dest==NULL) return FALSE;
  if (*dest) free(*dest);
  *dest = NULL;
  return TRUE;
}


char* clear_white_space(buf)
char *buf;
{
  char *ptr,*mark;
  int flag=0;
  if (buf==NULL) return NULL;
  for(ptr=mark=buf;*ptr;ptr++) {
    if (!flag && isspace(*ptr)) continue;
    if (*ptr=='"') flag=!flag;
    *mark=*ptr; 
    mark++;
  }
  *mark=0;
  return buf;
}

char *clear_remark_str(buf)
char *buf;
{
  char *ptr,*mark;
  int  quote,rem,star,slash;
  if (buf==NULL) return NULL;
  quote=rem=star=slash=0;
  for(ptr=mark=buf;*ptr;ptr++) {
     if (rem==2) {
        if (star && (*ptr=='/')) {
          rem=star=0;
/*          *mark=0; strcat(mark,ptr+1); */
          strcpy(mark,ptr+1);
          ptr=mark;
        } else {
          star =(*ptr=='*');
        }
     } else if (rem==1) {
        if (*ptr=='\n') {
           rem=0;
/*          *mark=0; strcat(mark,ptr); */
           strcpy(mark,ptr);
           ptr=mark;
        }
     } else if (*ptr=='"')  {
        quote=!quote;
     } else if (quote==0) {
        if (slash) {
           if (*ptr=='/') rem=1;
           else if(*ptr=='*') rem=2;
           if (rem) mark=ptr-1;
        }
     }
     slash=(*ptr=='/');
  }
  if (rem) mark[2]=0;
  return buf;
}

