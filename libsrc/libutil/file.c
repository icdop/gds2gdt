#include "header.h"
#include "file.h"

int DOS_FILE_FLAG=0;
static char tmp_filename[MAX_FILENAME_LEN];

char *check_filename(fname)
char *fname;
{
  static char* newname=NULL;
  char *ptr1,*ptr2;

  if (fname==NULL) return NULL;
  if (newname!=NULL) free(newname);
  newname=(char*)strdup(fname);  /* prevent if fname is a constant string */
  ptr1=fname; ptr2=newname;
  while (*ptr1) {
    if (!isspace(*ptr1)&&(*ptr1!='\"')){
       if (strchr("/\\",*ptr1)) *ptr2=(DOS_FILE_FLAG)?'\\':'/';
       ptr2++;
    }
    ptr1++;
  }
  *ptr2=0;
  return newname;
}

FILE *open_file(fname,mode)
char *fname,*mode;
{
  if ((fname==NULL)||(mode==NULL)) return NULL;
  return fopen(check_filename(fname),mode);
}

int backup_exist_file(filename) 
char *filename;
{
  FILE *fp;
  int i;
  if (fp=fopen(filename,"r")) {
     fclose(fp);
     i=0;
     sprintf(tmp_filename,"%s", filename);
     while(fp=fopen(tmp_filename,"r")) {
       fclose(fp);
       i++;
       sprintf(tmp_filename,"%s,%d",filename,i);
     }
     fprintf(stderr,"\"%s\" already exist! rename to \"%s\".\n",filename,tmp_filename);
     sprintf(tmp_filename,"mv %s %s,%d",filename,filename,i);
     system(tmp_filename);
  }
  return 1;
}

char *extract_filename(name,file,ext)
char *name;
char *file;
char *ext;
{
  char *ptr;
  if (name==NULL) return NULL;
  *name=0;
  if (file==NULL) return name;
  strcpy(name,file);
  if (ptr=strrchr(name,'.')) 
     if (strcmp(ptr+1,ext)==0) *ptr=0;
  return name;
}

