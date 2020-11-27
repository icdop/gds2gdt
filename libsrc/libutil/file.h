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
