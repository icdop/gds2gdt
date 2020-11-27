/*
 *  Disply HEX value of Binary File
 *  --------------------------------
 *  Author: Albert Li
 *  Date  : 2017/09/01
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(argc,argv)
int argc;
char **argv;
{
 char filename[1024];
 FILE *fp;
 int i,j;
 unsigned char c;
 char word[40];
 
 if (argc<=1) {
    printf("Usage: showbin <file>\n");
    return -1;
 }
 strcpy(filename,argv[1]);
 fp=fopen(filename,"r");
 if (fp==NULL) return 0;
 i=0;
 while(!feof(fp)) {
   printf("%06X: ",i);
   for (j=0; j<16; j++,i++) {
     if (j==8) printf(" ");
     c=fgetc(fp);
     if (!feof(fp)) {
       word[j]=(isprint(c)&&!isspace(c)&&(c<128))?c:'.';
       printf("%02X ",(unsigned char)c);
     } else {
       printf("   ");
       word[j]=' ';
     }
   }
   word[j]=0;
   printf(" %s\n",word);
 }
 fclose(fp); 
 return 1;
}
